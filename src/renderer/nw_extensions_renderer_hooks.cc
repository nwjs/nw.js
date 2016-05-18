#include "nw_extensions_renderer_hooks.h"

// nw
#include "content/nw/src/nw_version.h"

// base
#include "base/command_line.h"
#include "base/files/file_util.h"
#include "base/json/json_writer.h"
#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"
#include "ui/base/resource/resource_bundle.h"

// content
#include "content/public/child/v8_value_converter.h"
#include "content/public/common/content_switches.h"
#include "content/public/renderer/render_view.h"
#include "content/renderer/render_view_impl.h"

// extensions
#include "extensions/renderer/dispatcher.h"
#include "extensions/renderer/renderer_extension_registry.h"
#include "extensions/renderer/script_context.h"
#include "extensions/common/extension.h"
#include "extensions/common/manifest.h"
#include "extensions/common/manifest_constants.h"

#include "extensions/grit/extensions_renderer_resources.h"

// third_party/WebKit/
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebScriptSource.h"
#include "third_party/WebKit/public/web/WebView.h"
#include "third_party/WebKit/Source/platform/ScriptForbiddenScope.h"

#include "third_party/node/src/node_webkit.h"

// gen
#include "nw/id/commit.h"

// NEED TO STAY SYNC WITH NODE
#ifndef NODE_CONTEXT_EMBEDDER_DATA_INDEX
#define NODE_CONTEXT_EMBEDDER_DATA_INDEX 32
#endif

#if defined(OS_WIN)
#define _USE_MATH_DEFINES
#include <math.h>
#endif

using content::RenderViewImpl;

using extensions::ScriptContext;
using extensions::Extension;
using extensions::RendererExtensionRegistry;
using extensions::Dispatcher;

using blink::WebScriptSource;

namespace manifest_keys = extensions::manifest_keys;

#define NW_HOOK_MAP(type, sym, fn) extern type fn;
#include "content/nw/src/common/node_hooks.h"
#undef NW_HOOK_MAP

namespace nw {

namespace {
std::string g_extension_id;
extensions::Dispatcher* g_dispatcher = NULL;
bool g_skip_render_widget_hidden = false;

static inline v8::Local<v8::String> v8_str(const char* x) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  return v8::String::NewFromUtf8(isolate, x);
}

v8::Handle<v8::Value> CallNWTickCallback(void* env, const v8::Handle<v8::Value> ret) {
  v8::MicrotasksScope microtasks(v8::Isolate::GetCurrent(), v8::MicrotasksScope::kDoNotRunMicrotasks);
  g_call_tick_callback_fn(env);
  return Undefined(v8::Isolate::GetCurrent());
}

v8::Handle<v8::Value> CreateNW(ScriptContext* context,
                               v8::Handle<v8::Object> node_global,
                               v8::Handle<v8::Context> node_context) {
  v8::Handle<v8::String> nw_string(
      v8::String::NewFromUtf8(context->isolate(), "nw"));
  v8::Handle<v8::Object> global(context->v8_context()->Global());
  v8::Handle<v8::Value> nw(global->Get(nw_string));
  if (nw->IsUndefined()) {
    nw = v8::Object::New(context->isolate());;
    //node_context->Enter();
    global->Set(nw_string, nw);
    //node_context->Exit();
  }
  return nw;
}

// Returns |value| cast to an object if possible, else an empty handle.
v8::Handle<v8::Object> AsObjectOrEmpty(v8::Handle<v8::Value> value) {
  return value->IsObject() ? value.As<v8::Object>() : v8::Handle<v8::Object>();
}

}

const char* GetChromiumVersion();

// renderer
void ContextCreationHook(blink::WebLocalFrame* frame, ScriptContext* context) {
  v8::Isolate* isolate = context->isolate();

  if (g_extension_id.empty())
    g_extension_id = context->extension()->id();

  bool nodejs_enabled = true;
  context->extension()->manifest()->GetBoolean(manifest_keys::kNWJSEnableNode, &nodejs_enabled);

  if (!nodejs_enabled)
    return;

  if (!g_is_node_initialized_fn())
    g_setup_nwnode_fn(0, nullptr);

  bool mixed_context = false;
  context->extension()->manifest()->GetBoolean(manifest_keys::kNWJSMixedContext, &mixed_context);
  v8::Local<v8::Context> node_context;
  g_get_node_context_fn(&node_context);
  if (node_context.IsEmpty() || mixed_context) {
    {
      int argc = 1;
      char argv0[] = "node";
      char* argv[3];
      argv[0] = argv0;
      argv[1] = argv[2] = nullptr;
      std::string main_fn;

      if (context->extension()->manifest()->GetString("node-main", &main_fn)) {
        argc = 2;
        argv[1] = strdup(main_fn.c_str());
      }

      v8::Isolate* isolate = v8::Isolate::GetCurrent();
      v8::HandleScope scope(isolate);
      v8::MicrotasksScope microtasks(isolate, v8::MicrotasksScope::kDoNotRunMicrotasks);

      g_set_nw_tick_callback_fn(&CallNWTickCallback);
      v8::Local<v8::Context> dom_context = context->v8_context();
      if (!mixed_context)
        g_set_node_context_fn(isolate, &dom_context);
      dom_context->SetSecurityToken(v8::String::NewFromUtf8(isolate, "nw-token"));
      dom_context->Enter();

      g_start_nw_instance_fn(argc, argv, dom_context);
      {
        v8::Local<v8::Script> script =
          v8::Script::Compile(v8::String::NewFromUtf8(isolate,
                                                      (std::string("process.versions['nw'] = '" NW_VERSION_STRING "';") +
                                                       "process.versions['node-webkit'] = '" NW_VERSION_STRING "';"
                                                       "process.versions['nw-commit-id'] = '" NW_COMMIT_HASH "';"
                                                       "process.versions['chromium'] = '" + GetChromiumVersion() + "';").c_str()
         ));
        script->Run();
      }

      if (context->extension()->manifest()->GetString(manifest_keys::kNWJSInternalMainFilename, &main_fn)) {
        v8::Local<v8::Script> script = v8::Script::Compile(v8::String::NewFromUtf8(isolate,
         ("global.__filename = '" + main_fn + "';").c_str()));
        script->Run();
      }
      {
        std::string root_path = context->extension()->path().AsUTF8Unsafe();
#if defined(OS_WIN)
        base::ReplaceChars(root_path, "\\", "\\\\", &root_path);
#endif
        base::ReplaceChars(root_path, "'", "\\'", &root_path);
        v8::Local<v8::Script> script = v8::Script::Compile(v8::String::NewFromUtf8(isolate,
         ("global.__dirname = '" + root_path + "';").c_str()));
        script->Run();
      }
      bool content_verification = false;
      if (context->extension()->manifest()->GetBoolean(manifest_keys::kNWJSContentVerifyFlag,
                                                       &content_verification) && content_verification) {
        v8::Local<v8::Script> script =
          v8::Script::Compile(v8::String::NewFromUtf8(isolate,
                                                      (std::string("global.__nwjs_cv = true;") +
                                                       "global.__nwjs_ext_id = '" + context->extension()->id() + "';").c_str()));

        script->Run();
      }
      
      dom_context->Exit();
    }
  }

  v8::Local<v8::Context> node_context2;
  if (mixed_context)
    node_context2 = context->v8_context();
  else
    g_get_node_context_fn(&node_context2);
  v8::Local<v8::Context> g_context =
    v8::Local<v8::Context>::New(isolate, node_context2);
  v8::Local<v8::Object> node_global = g_context->Global();

  if (!mixed_context) {
    context->v8_context()->SetAlignedPointerInEmbedderData(NODE_CONTEXT_EMBEDDER_DATA_INDEX, g_get_node_env_fn());
    context->v8_context()->SetSecurityToken(g_context->GetSecurityToken());
  }
  v8::Handle<v8::Object> nw = AsObjectOrEmpty(CreateNW(context, node_global, g_context));

  v8::Local<v8::Array> symbols = v8::Array::New(isolate, 5);
  symbols->Set(0, v8::String::NewFromUtf8(isolate, "global"));
  symbols->Set(1, v8::String::NewFromUtf8(isolate, "process"));
  symbols->Set(2, v8::String::NewFromUtf8(isolate, "Buffer"));
  symbols->Set(3, v8::String::NewFromUtf8(isolate, "root"));
  symbols->Set(4, v8::String::NewFromUtf8(isolate, "require"));

  g_context->Enter();
  for (unsigned i = 0; i < symbols->Length(); ++i) {
    v8::Local<v8::Value> key = symbols->Get(i);
    v8::Local<v8::Value> val = node_global->Get(key);
    nw->Set(key, val);
  }
  g_context->Exit();

  std::string set_nw_script = "'use strict';";
  {
    v8::MicrotasksScope microtasks(isolate, v8::MicrotasksScope::kDoNotRunMicrotasks);
    v8::Context::Scope cscope(context->v8_context());
    // Make node's relative modules work
    std::string root_path = context->extension()->path().AsUTF8Unsafe();
    GURL frame_url = ScriptContext::GetDataSourceURLForFrame(frame);
    std::string url_path = frame_url.path();
#if defined(OS_WIN)
    base::ReplaceChars(root_path, "\\", "\\\\", &root_path);
#endif
    base::ReplaceChars(root_path, "'", "\\'", &root_path);
    v8::Local<v8::Script> script = v8::Script::Compile(v8::String::NewFromUtf8(isolate, (
        set_nw_script +
        "nw.global.XMLHttpRequest = XMLHttpRequest;" +
        // Make node's relative modules work
        "if (typeof nw.process != 'undefined' && "
        "(!nw.process.mainModule.filename || nw.process.mainModule.filename === 'blank' ||"
        "nw.process.mainModule.filename.indexOf('_generated_background_page.html') >= 0)) {"
        "  let root = '" + root_path + "';"
        "  let p = '" + url_path + "';"
        "nw.process.mainModule.filename = root + p;"
        "nw.process.mainModule.paths = nw.global.require('module')._nodeModulePaths(nw.process.cwd());"
        "nw.process.mainModule.loaded = true;"
        "}").c_str()),
    v8::String::NewFromUtf8(isolate, "process_main"));
    CHECK(*script);
    script->Run();
  }

}

void DocumentHook2(bool start, content::RenderFrame* frame, Dispatcher* dispatcher) {
  // ignore the first invocation of this hook for iframe
  // or we'll trigger creating a context with invalid type
  // there will follow another one with valid url
  blink::ScriptForbiddenScope::AllowUserAgentScript script;
  blink::WebLocalFrame* web_frame = frame->GetWebFrame();
  GURL frame_url = ScriptContext::GetDataSourceURLForFrame(web_frame);
  if (web_frame->parent() && (!frame_url.is_valid() || frame_url.is_empty()))
    return;
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);
  v8::Local<v8::Context> v8_context = frame->GetRenderView()
      ->GetWebView()->mainFrame()->mainWorldScriptContext();
  ScriptContext* script_context =
      dispatcher->script_context_set().GetByV8Context(v8_context);
  if (!script_context)
    return;
  std::vector<v8::Handle<v8::Value> > arguments;
  v8::Local<v8::Value> window =
    web_frame->mainWorldScriptContext()->Global();
  arguments.push_back(v8::Boolean::New(isolate, start));
  arguments.push_back(window);
  script_context->module_system()->CallModuleMethod("nw.Window", "onDocumentStartEnd", &arguments);
}

void DocumentElementHook(blink::WebLocalFrame* frame,
                         const extensions::Extension* extension,
                         const GURL& effective_document_url) {
  // ignore the first invocation of this hook for iframe
  // or we'll trigger creating a context with invalid type
  // there will follow another one with valid url
  blink::ScriptForbiddenScope::AllowUserAgentScript script;
  GURL frame_url = ScriptContext::GetDataSourceURLForFrame(frame);
  if (frame->parent() && (!frame_url.is_valid() || frame_url.is_empty()))
    return;
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope hscope(isolate);
  frame->document().getSecurityOrigin().grantUniversalAccess();
  frame->setNodeJS(true);
  std::string path = effective_document_url.path();
  v8::Local<v8::Context> v8_context = frame->mainWorldScriptContext();
  std::string root_path = extension->path().AsUTF8Unsafe();
  base::FilePath root(extension->path());
  if (!v8_context.IsEmpty()) {
    v8::MicrotasksScope microtasks(v8::Isolate::GetCurrent(), v8::MicrotasksScope::kDoNotRunMicrotasks);
    v8::Context::Scope cscope(v8_context);
    // Make node's relative modules work
#if defined(OS_WIN)
    base::ReplaceChars(root_path, "\\", "\\\\", &root_path);
#endif
    base::ReplaceChars(root_path, "'", "\\'", &root_path);

    v8::Local<v8::Script> script2 = v8::Script::Compile(v8::String::NewFromUtf8(isolate, (
        "'use strict';"
        "if (typeof nw != 'undefined' && typeof __filename == 'undefined') {"
        "  let root = '" + root_path + "';"
        "  let path = '" + path      + "';"
        "nw.__filename = root + path;"
        "nw.__dirname = root;"
        "}").c_str()),
    v8::String::NewFromUtf8(isolate, "process_main2"));
    CHECK(*script2);
    script2->Run();
  }
  RenderViewImpl* rv = RenderViewImpl::FromWebView(frame->view());
  if (!rv)
    return;

  ui::ResourceBundle* resource_bundle = &ResourceBundle::GetSharedInstance();
  base::StringPiece resource =
      resource_bundle->GetRawDataResource(IDR_NW_PRE13_SHIM_JS);
  if (resource.empty())
    return;
  base::string16 jscript = base::UTF8ToUTF16(resource.as_string());
  if (!v8_context.IsEmpty()) {
    v8::MicrotasksScope microtasks(v8::Isolate::GetCurrent(), v8::MicrotasksScope::kDoNotRunMicrotasks);
    v8::Context::Scope cscope(v8_context);
    frame->executeScriptAndReturnValue(WebScriptSource(jscript));
  }

  std::string js_fn = rv->renderer_preferences().nw_inject_js_doc_start;
  if (js_fn.empty())
    return;
  base::FilePath js_file = root.AppendASCII(js_fn);
  std::string content;
  if (!base::ReadFileToString(js_file, &content)) {
    //LOG(WARNING) << "Failed to load js script file: " << js_file.value();
    return;
  }
  jscript = base::UTF8ToUTF16(content);
  if (!v8_context.IsEmpty()) {
    v8::MicrotasksScope microtasks(v8::Isolate::GetCurrent(), v8::MicrotasksScope::kDoNotRunMicrotasks);
    v8::Context::Scope cscope(v8_context);
    // v8::Handle<v8::Value> result;
    frame->executeScriptAndReturnValue(WebScriptSource(jscript));
  }
}

void willHandleNavigationPolicy(content::RenderView* rv,
                                blink::WebFrame* frame,
                                const blink::WebURLRequest& request,
                                blink::WebNavigationPolicy* policy,
                                blink::WebString* manifest,
                                bool new_win) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);
  v8::Handle<v8::Context> v8_context =
      rv->GetWebView()->mainFrame()->mainWorldScriptContext();
  ScriptContext* script_context =
      g_dispatcher->script_context_set().GetByV8Context(v8_context);
  //check extension for remote pages, which doesn't have appWindow object
  if (!script_context || !script_context->extension())
    return;
  v8::Context::Scope cscope (v8_context);
  v8::Handle<v8::Value> element = v8::Null(isolate);
  v8::Handle<v8::Object> policy_obj = v8::Object::New(isolate);

#if 0
  blink::LocalFrame* core_frame = blink::toWebLocalFrameImpl(frame)->frame();
  if (core_frame->deprecatedLocalOwner()) {
    element = blink::toV8((blink::HTMLElement*)core_frame->deprecatedLocalOwner(),
                            frame->mainWorldScriptContext()->Global(),
                            frame->mainWorldScriptContext()->GetIsolate());
  }
#endif
  std::vector<v8::Handle<v8::Value> > arguments;
  arguments.push_back(element);
  arguments.push_back(v8_str(request.url().string().utf8().c_str()));
  arguments.push_back(policy_obj);
  if (new_win) {
    script_context->module_system()->CallModuleMethod("nw.Window",
                                                      "onNewWinPolicy", &arguments);
  } else {
    const char* req_context = nullptr;
    switch (request.getRequestContext()) {
    case blink::WebURLRequest::RequestContextHyperlink:
      req_context = "hyperlink";
      break;
    case blink::WebURLRequest::RequestContextFrame:
      req_context = "form";
      break;
    case blink::WebURLRequest::RequestContextLocation:
      req_context = "location";
      break;
    default:
      break;
    }
    if (req_context) {
      arguments.push_back(v8_str(req_context));
      script_context->module_system()->CallModuleMethod("nw.Window",
                                                        "onNavigation", &arguments);
    }
  }

  scoped_ptr<content::V8ValueConverter> converter(content::V8ValueConverter::create());
  v8::Local<v8::Value> manifest_v8 = policy_obj->Get(v8_str("manifest"));
  scoped_ptr<base::Value> manifest_val(converter->FromV8Value(manifest_v8, v8_context));
  std::string manifest_str;
  if (manifest_val.get() && base::JSONWriter::Write(*manifest_val, &manifest_str)) {
    *manifest = blink::WebString::fromUTF8(manifest_str.c_str());
  }

  v8::Local<v8::Value> val = policy_obj->Get(v8_str("val"));
  if (!val->IsString())
    return;
  v8::String::Utf8Value policy_str(val);
  if (!strcmp(*policy_str, "ignore"))
    *policy = blink::WebNavigationPolicyIgnore;
  else if (!strcmp(*policy_str, "download"))
    *policy = blink::WebNavigationPolicyDownload;
  else if (!strcmp(*policy_str, "current"))
    *policy = blink::WebNavigationPolicyCurrentTab;
  else if (!strcmp(*policy_str, "new-window"))
    *policy = blink::WebNavigationPolicyNewWindow;
  else if (!strcmp(*policy_str, "new-popup"))
    *policy = blink::WebNavigationPolicyNewPopup;
}

void ExtensionDispatcherCreated(extensions::Dispatcher* dispatcher) {
  g_dispatcher = dispatcher;
  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  if (command_line.HasSwitch(switches::kDisableRAFThrottling))
    g_skip_render_widget_hidden = true;
}

bool RenderWidgetWasHiddenHook(content::RenderWidget* rw) {
  return g_skip_render_widget_hidden;
}

void DocumentFinishHook(blink::WebFrame* frame,
                         const extensions::Extension* extension,
                         const GURL& effective_document_url) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope hscope(isolate);
  std::string path = effective_document_url.path();
  v8::Local<v8::Context> v8_context = frame->mainWorldScriptContext();
  RenderViewImpl* rv = RenderViewImpl::FromWebView(frame->view());
  if (!rv)
    return;
  if (!extension) {
    extension = RendererExtensionRegistry::Get()->GetByID(g_extension_id);
    if (!extension)
      return;
  }
  if (!(extension->is_extension() || extension->is_platform_app()))
    return;
  std::string root_path = extension->path().AsUTF8Unsafe();
  base::FilePath root(extension->path());
  std::string js_fn = rv->renderer_preferences().nw_inject_js_doc_end;
  if (js_fn.empty())
    return;
  base::FilePath js_file = root.AppendASCII(js_fn);
  std::string content;
  if (!base::ReadFileToString(js_file, &content)) {
    //LOG(WARNING) << "Failed to load js script file: " << js_file.value();
    return;
  }
  base::string16 jscript = base::UTF8ToUTF16(content);
  {
    v8::MicrotasksScope microtasks(v8::Isolate::GetCurrent(), v8::MicrotasksScope::kDoNotRunMicrotasks);
    blink::ScriptForbiddenScope::AllowUserAgentScript script;
    v8::Context::Scope cscope(v8_context);
    // v8::Handle<v8::Value> result;
    frame->executeScriptAndReturnValue(WebScriptSource(jscript));
  }
}

void OnRenderProcessShutdownHook(extensions::ScriptContext* context) {
  v8::MicrotasksScope microtasks(v8::Isolate::GetCurrent(), v8::MicrotasksScope::kDoNotRunMicrotasks);
  blink::ScriptForbiddenScope::AllowUserAgentScript script;
  void* env = g_get_current_env_fn(context->v8_context());
  if (g_is_node_initialized_fn()) {
    g_emit_exit_fn(env);
    g_run_at_exit_fn(env);
  }
}

} // namespace nw