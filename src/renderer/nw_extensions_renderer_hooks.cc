#include "nw_extensions_renderer_hooks.h"

// nw
#include "content/nw/src/nw_version.h"

// base
#include "base/command_line.h"
#include "base/files/file_util.h"
#include "base/json/json_writer.h"
#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/thread.h"
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
#include "third_party/WebKit/public/web/WebKit.h"

#include "third_party/node-nw/src/node_webkit.h"

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

#if defined(COMPONENT_BUILD) && defined(WIN32)
#define NW_HOOK_MAP(type, sym, fn) BASE_EXPORT type fn;
#define BLINK_HOOK_MAP(type, sym, fn) BLINK_EXPORT type fn;
#define PLATFORM_HOOK_MAP(type, sym, fn) PLATFORM_EXPORT type fn;
#else
#define NW_HOOK_MAP(type, sym, fn) extern type fn;
#define BLINK_HOOK_MAP(type, sym, fn) extern type fn;
#define PLATFORM_HOOK_MAP(type, sym, fn) extern type fn;
#endif
#include "content/nw/src/common/node_hooks.h"
#undef NW_HOOK_MAP

namespace nw {

namespace {
std::string g_extension_id, g_extension_root;
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

const std::string& get_main_extension_id() {
  return g_extension_id;
}

const char* GetChromiumVersion();

// renderer

void WebWorkerNewThreadHook(const char* name, base::Thread::Options* options) {
  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  if (!command_line.HasSwitch(switches::kEnableNodeWorker))
    return;
  if (!strcmp(name, "DedicatedWorker Thread") || !strcmp(name, "SharedWorker Thread"))
    options->message_loop_type = base::MessageLoop::TYPE_NODE;
}

void WebWorkerStartThreadHook(blink::Frame* frame, const char* path, std::string* script, bool* isNodeJS) {
  std::string root_path;
  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  if (!command_line.HasSwitch(switches::kEnableNodeWorker)) {
    *isNodeJS = false;
    return;
  }
  if (frame) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scope(isolate);
    blink::WebFrame* web_frame = blink::WebFrame::FromFrame(frame);
    v8::Local<v8::Context> v8_context = web_frame->MainWorldScriptContext();
    ScriptContext* script_context =
      g_dispatcher->script_context_set().GetByV8Context(v8_context);
    if (!script_context || !script_context->extension())
      return;
    root_path = g_extension_root;
  } else {
    root_path = *script;
  }
  std::string url_path(path);
#if defined(OS_WIN)
    base::ReplaceChars(root_path, "\\", "\\\\", &root_path);
#endif
    base::ReplaceChars(root_path, "'", "\\'", &root_path);
  *script  = "global.__filename = '" + url_path + "';";
  *script += "global.__dirname  = '" + root_path + "';";
  *script += "{ let root = '" + root_path + "';"
    "  let p = '" + url_path + "';"
    "process.mainModule.filename = root + p;"
    "process.mainModule.paths = global.require('module')._nodeModulePaths(process.cwd());"
    "process.mainModule.loaded = true;"
    "}";
}

void ContextCreationHook(blink::WebLocalFrame* frame, ScriptContext* context) {
  v8::Isolate* isolate = context->isolate();

  const Extension* extension = context->extension();
  std::string extension_root;

  if (g_extension_id.empty() && extension)
    g_extension_id = extension->id();

  bool nodejs_enabled = true;
  const base::CommandLine& command_line =
    *base::CommandLine::ForCurrentProcess();
  bool worker_support = command_line.HasSwitch(switches::kEnableNodeWorker);

  if (extension) {
    extension->manifest()->GetBoolean(manifest_keys::kNWJSEnableNode, &nodejs_enabled);
    extension_root = extension->path().AsUTF8Unsafe();
  } else {
    extension_root = command_line.GetSwitchValuePath(switches::kNWAppPath).AsUTF8Unsafe();
  }
  g_extension_root = extension_root;

  if (!nodejs_enabled)
    return;

  blink::set_web_worker_hooks((void*)WebWorkerStartThreadHook);
  g_web_worker_thread_new_fn = (VoidPtr2Fn)WebWorkerNewThreadHook;
  if (!g_is_node_initialized_fn())
    g_setup_nwnode_fn(0, nullptr, worker_support);

  bool mixed_context = false;
  bool node_init_run = false;
  bool nwjs_guest = nwjs_guest = command_line.HasSwitch("nwjs-guest");
  
  if (extension)
    extension->manifest()->GetBoolean(manifest_keys::kNWJSMixedContext, &mixed_context);
  // handle navigation in webview #5622
  if (nwjs_guest)
    mixed_context = true;
  v8::Local<v8::Context> node_context;
  g_get_node_context_fn(&node_context);
  if (node_context.IsEmpty() || mixed_context) {
    node_init_run = true;
    {
      int argc = 1;
      char argv0[] = "node";
      char* argv[3];
      argv[0] = argv0;
      argv[1] = argv[2] = nullptr;
      std::string main_fn;

      if (extension && extension->manifest()->GetString("node-main", &main_fn)) {
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

      if (extension && extension->manifest()->GetString(manifest_keys::kNWJSInternalMainFilename, &main_fn)) {
        v8::Local<v8::Script> script = v8::Script::Compile(v8::String::NewFromUtf8(isolate,
         ("global.__filename = '" + main_fn + "';").c_str()));
        script->Run();
      }
      {
        std::string root_path = extension_root;
#if defined(OS_WIN)
        base::ReplaceChars(root_path, "\\", "\\\\", &root_path);
#endif
        base::ReplaceChars(root_path, "'", "\\'", &root_path);
        v8::Local<v8::Script> script = v8::Script::Compile(v8::String::NewFromUtf8(isolate,
         ("global.__dirname = '" + root_path + "';").c_str()));
        script->Run();
      }
      bool content_verification = false;
      if (extension && extension->manifest()->GetBoolean(manifest_keys::kNWJSContentVerifyFlag,
                                                       &content_verification) && content_verification) {
        v8::Local<v8::Script> script =
          v8::Script::Compile(v8::String::NewFromUtf8(isolate,
                                                      (std::string("global.__nwjs_cv = true;") +
                                                       "global.__nwjs_ext_id = '" + extension->id() + "';").c_str()));

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

  v8::Local<v8::Array> symbols = v8::Array::New(isolate, 4);
  symbols->Set(0, v8::String::NewFromUtf8(isolate, "global"));
  symbols->Set(1, v8::String::NewFromUtf8(isolate, "process"));
  symbols->Set(2, v8::String::NewFromUtf8(isolate, "Buffer"));
  symbols->Set(3, v8::String::NewFromUtf8(isolate, "require"));

  g_context->Enter();
  for (unsigned i = 0; i < symbols->Length(); ++i) {
    v8::Local<v8::Value> key = symbols->Get(i);
    v8::Local<v8::Value> val = node_global->Get(key);
    nw->Set(key, val);
    if (nwjs_guest && !node_init_run) {
      //running in nwjs webview and node was initialized in
      //chromedriver automation extension
      context->v8_context()->Global()->Set(key, val);
    }
  }
  g_context->Exit();

  std::string set_nw_script = "'use strict';";
  {
    v8::MicrotasksScope microtasks(isolate, v8::MicrotasksScope::kDoNotRunMicrotasks);
    v8::Context::Scope cscope(context->v8_context());
    // Make node's relative modules work
    std::string root_path = extension_root;
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

base::FilePath GetRootPathRenderer() {
  base::FilePath ret;
  const Extension* extension = RendererExtensionRegistry::Get()->GetByID(g_extension_id);
  if (!extension)
    return ret;
  if (!(extension->is_extension() || extension->is_platform_app()))
    return ret;
  return extension->path();
}

void TryInjectStartScript(blink::WebLocalFrame* frame, const Extension* extension, bool start) {
  RenderViewImpl* rv = RenderViewImpl::FromWebView(frame->View());
  if (!rv)
    return;

  std::string js_fn = start ? rv->renderer_preferences().nw_inject_js_doc_start :
                              rv->renderer_preferences().nw_inject_js_doc_end;
  if (js_fn.empty())
    return;
  base::FilePath fpath = base::FilePath::FromUTF8Unsafe(js_fn);
  if (!fpath.IsAbsolute()) {
    const Extension* extension = nullptr;
    if (!extension) {
      extension = RendererExtensionRegistry::Get()->GetByID(g_extension_id);
      if (!extension)
        return;
    }
    if (!(extension->is_extension() || extension->is_platform_app()))
      return;
    base::FilePath root(extension->path());
    fpath = root.AppendASCII(js_fn);
  }
  v8::Local<v8::Context> v8_context = frame->MainWorldScriptContext();
  std::string content;
  if (!base::ReadFileToString(fpath, &content)) {
    //LOG(WARNING) << "Failed to load js script file: " << js_file.value();
    return;
  }
  if (!v8_context.IsEmpty()) {
    v8::MicrotasksScope microtasks(v8::Isolate::GetCurrent(), v8::MicrotasksScope::kDoNotRunMicrotasks);
    blink::ScriptForbiddenScope::AllowUserAgentScript script;
    v8::Context::Scope cscope(v8_context);
    // v8::Handle<v8::Value> result;
    frame->ExecuteScriptAndReturnValue(WebScriptSource(blink::WebString::FromUTF8(content)));
  }
}

void DocumentFinishHook(blink::WebLocalFrame* frame,
                         const extensions::Extension* extension,
                         const GURL& effective_document_url) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope hscope(isolate);

  TryInjectStartScript(frame, extension, false);
}

void DocumentHook2(bool start, content::RenderFrame* frame, Dispatcher* dispatcher) {
  // ignore the first invocation of this hook for iframe
  // or we'll trigger creating a context with invalid type
  // there will follow another one with valid url
  blink::ScriptForbiddenScope::AllowUserAgentScript script;
  blink::WebLocalFrame* web_frame = frame->GetWebFrame();
  GURL frame_url = ScriptContext::GetDataSourceURLForFrame(web_frame);
  if (web_frame->Parent() && (!frame_url.is_valid() || frame_url.is_empty()))
    return;
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);
  v8::Local<v8::Context> v8_context = frame->GetRenderView()
      ->GetWebView()->MainFrame()->MainWorldScriptContext();
  ScriptContext* script_context =
      dispatcher->script_context_set().GetByV8Context(v8_context);
  if (start)
    TryInjectStartScript(web_frame, script_context ? script_context->extension() : nullptr, true);
  if (!script_context)
    return;
  std::vector<v8::Handle<v8::Value> > arguments;
  v8::Local<v8::Value> window =
    web_frame->MainWorldScriptContext()->Global();
  arguments.push_back(v8::Boolean::New(isolate, start));
  arguments.push_back(window);
  script_context->module_system()->CallModuleMethodSafe("nw.Window", "onDocumentStartEnd", &arguments);
}

void DocumentElementHook(blink::WebLocalFrame* frame,
                         const extensions::Extension* extension,
                         const GURL& effective_document_url) {
  // ignore the first invocation of this hook for iframe
  // or we'll trigger creating a context with invalid type
  // there will follow another one with valid url
  blink::ScriptForbiddenScope::AllowUserAgentScript script;
  GURL frame_url = ScriptContext::GetDataSourceURLForFrame(frame);
  if (frame->Parent() && (!frame_url.is_valid() || frame_url.is_empty()))
    return;
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope hscope(isolate);
  frame->GetDocument().GetSecurityOrigin().grantUniversalAccess();
  frame->setNodeJS(true);
  std::string path = effective_document_url.path();
  v8::Local<v8::Context> v8_context = frame->MainWorldScriptContext();
  std::string root_path = g_extension_root;
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
  RenderViewImpl* rv = RenderViewImpl::FromWebView(frame->View());
  if (!rv)
    return;

  ui::ResourceBundle* resource_bundle = &ResourceBundle::GetSharedInstance();
  base::StringPiece resource =
      resource_bundle->GetRawDataResource(IDR_NW_PRE13_SHIM_JS);
  if (resource.empty())
    return;
  if (!v8_context.IsEmpty()) {
    v8::MicrotasksScope microtasks(v8::Isolate::GetCurrent(), v8::MicrotasksScope::kDoNotRunMicrotasks);
    v8::Context::Scope cscope(v8_context);
    frame->ExecuteScriptAndReturnValue(WebScriptSource(blink::WebString::FromUTF8(resource.as_string())));
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
      rv->GetWebView()->MainFrame()->MainWorldScriptContext();
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
  arguments.push_back(v8_str(request.Url().GetString().Utf8().c_str()));
  arguments.push_back(policy_obj);
  if (new_win) {
    script_context->module_system()->CallModuleMethodSafe("nw.Window",
                                                      "onNewWinPolicy", &arguments);
  } else {
    const char* req_context = nullptr;
    switch (request.GetRequestContext()) {
    case blink::WebURLRequest::kRequestContextHyperlink:
      req_context = "hyperlink";
      break;
    case blink::WebURLRequest::kRequestContextFrame:
      req_context = "form";
      break;
    case blink::WebURLRequest::kRequestContextLocation:
      req_context = "location";
      break;
    default:
      break;
    }
    if (req_context) {
      arguments.push_back(v8_str(req_context));
      script_context->module_system()->CallModuleMethodSafe("nw.Window",
                                                        "onNavigation", &arguments);
    }
  }

  std::unique_ptr<content::V8ValueConverter> converter(content::V8ValueConverter::create());
  v8::Local<v8::Value> manifest_v8 = policy_obj->Get(v8_str("manifest"));
  std::unique_ptr<base::Value> manifest_val(converter->FromV8Value(manifest_v8, v8_context));
  std::string manifest_str;
  if (manifest_val.get() && base::JSONWriter::Write(*manifest_val, &manifest_str)) {
    *manifest = blink::WebString::FromUTF8(manifest_str.c_str());
  }

  v8::Local<v8::Value> val = policy_obj->Get(v8_str("val"));
  if (!val->IsString())
    return;
  v8::String::Utf8Value policy_str(val);
  if (!strcmp(*policy_str, "ignore"))
    *policy = blink::kWebNavigationPolicyIgnore;
  else if (!strcmp(*policy_str, "download"))
    *policy = blink::kWebNavigationPolicyDownload;
  else if (!strcmp(*policy_str, "current"))
    *policy = blink::kWebNavigationPolicyCurrentTab;
  else if (!strcmp(*policy_str, "new-window"))
    *policy = blink::kWebNavigationPolicyNewWindow;
  else if (!strcmp(*policy_str, "new-popup"))
    *policy = blink::kWebNavigationPolicyNewPopup;
}

typedef bool (*RenderWidgetWasHiddenHookFn)(content::RenderWidget*);
#if defined(COMPONENT_BUILD)
CONTENT_EXPORT RenderWidgetWasHiddenHookFn gRenderWidgetWasHiddenHook;
#else
extern RenderWidgetWasHiddenHookFn gRenderWidgetWasHiddenHook;
#endif

bool RenderWidgetWasHiddenHook(content::RenderWidget* rw) {
  return g_skip_render_widget_hidden;
}

void ExtensionDispatcherCreated(extensions::Dispatcher* dispatcher) {
  g_dispatcher = dispatcher;
  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  if (command_line.HasSwitch(switches::kDisableRAFThrottling))
    g_skip_render_widget_hidden = true;
  nw::gRenderWidgetWasHiddenHook = RenderWidgetWasHiddenHook;
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
