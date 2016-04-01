#include "extensions/browser/app_window/app_window.h"

#include "nw_content.h"
#include "nw_base.h"
#include "base/files/file_enumerator.h"

#include "base/command_line.h"
#include "base/files/file_util.h"
#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/path_service.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/thread_restrictions.h"

#include "chrome/browser/browser_process.h"
#include "chrome/browser/devtools/devtools_window.h"
#include "chrome/browser/io_thread.h"
#include "chrome/browser/lifetime/application_lifetime.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "chrome/common/chrome_paths.h"

#include "components/crx_file/id_util.h"
#include "components/content_settings/core/browser/content_settings_utils.h"

#include "content/browser/dom_storage/dom_storage_area.h"
#include "content/common/dom_storage/dom_storage_map.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/child_process_security_policy.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/notification_types.h"
#include "content/public/browser/plugin_service.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/child/v8_value_converter.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/result_codes.h"
#include "content/public/common/user_agent.h"
#include "content/public/renderer/render_view.h"

#include "content/nw/src/nw_content_verifier_delegate.h"
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/api/object_manager.h"
#include "content/nw/src/policy_cert_verifier.h"

#include "extensions/browser/guest_view/web_view/web_view_renderer_state.h"
#include "extensions/browser/guest_view/web_view/web_view_guest.h"
#include "extensions/browser/event_router.h"
#include "extensions/browser/extension_prefs.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/renderer/dispatcher.h"
#include "extensions/renderer/renderer_extension_registry.h"
#include "extensions/renderer/script_context.h"
#include "extensions/common/extension.h"
#include "extensions/common/feature_switch.h"
#include "extensions/common/features/feature.h"
#include "extensions/common/manifest.h"
#include "extensions/common/manifest_constants.h"
#include "extensions/common/manifest_handlers/webview_info.h"

#include "extensions/grit/extensions_renderer_resources.h"

#include "net/cert/x509_certificate.h"

#include "storage/common/database/database_identifier.h"

#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/public/web/WebFrame.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebSecurityPolicy.h"
#include "third_party/WebKit/public/web/WebView.h"
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebScriptSource.h"

#include "chrome/common/chrome_constants.h"

#include "ui/base/resource/resource_bundle.h"
#include "ui/base/ui_base_types.h"
#include "ui/gfx/image/image.h"
#include "ui/gfx/image/image_skia_rep.h"
#include "ui/gfx/codec/png_codec.h"

// NEED TO STAY SYNC WITH NODE
#ifndef NODE_CONTEXT_EMBEDDER_DATA_INDEX
#define NODE_CONTEXT_EMBEDDER_DATA_INDEX 32
#endif

#include "third_party/node/src/node_webkit.h"
#include "third_party/WebKit/public/web/WebScopedMicrotaskSuppression.h"
#include "nw/id/commit.h"
#include "content/nw/src/nw_version.h"

#undef LOG
#undef ASSERT
//#undef FROM_HERE

#if defined(OS_WIN)
#define _USE_MATH_DEFINES
#include <math.h>
#endif
//#include "third_party/WebKit/Source/config.h"
#include "third_party/WebKit/Source/core/frame/Frame.h"
#include "third_party/WebKit/Source/web/WebLocalFrameImpl.h"
#include "V8HTMLElement.h"

#include "content/renderer/render_view_impl.h"
#include "base/logging.h"

using content::RenderView;
using content::RenderViewImpl;
using content::DevToolsAgentHost;
using content::RenderFrameHost;
using content::RenderProcessHost;
using content::WebContents;
using extensions::ScriptContext;
using extensions::Extension;
using extensions::EventRouter;
using extensions::Manifest;
using extensions::Feature;
using extensions::ExtensionPrefs;
using extensions::ExtensionRegistry;
using extensions::RendererExtensionRegistry;
using extensions::Dispatcher;
using extensions::ContentVerifierDelegate;
using extensions::NWContentVerifierDelegate;
using extensions::WebviewInfo;
using extensions::WebViewGuest;
using blink::WebScriptSource;

namespace manifest_keys = extensions::manifest_keys;

CallTickCallbackFn g_call_tick_callback_fn = nullptr;
SetupNWNodeFn g_setup_nwnode_fn = nullptr;
IsNodeInitializedFn g_is_node_initialized_fn = nullptr;
SetNWTickCallbackFn g_set_nw_tick_callback_fn = nullptr;
StartNWInstanceFn g_start_nw_instance_fn = nullptr;
GetNodeContextFn g_get_node_context_fn = nullptr;
SetNodeContextFn g_set_node_context_fn = nullptr;
GetNodeEnvFn g_get_node_env_fn = nullptr;
GetCurrentEnvironmentFn g_get_current_env_fn = nullptr;
EmitExitFn g_emit_exit_fn = nullptr;
RunAtExitFn g_run_at_exit_fn = nullptr;

namespace nw {


namespace {

extensions::Dispatcher* g_dispatcher = NULL;
bool g_reloading_app = false;
bool g_pinning_renderer = true;
int g_cdt_process_id = -1;
std::string g_extension_id;
bool g_skip_render_widget_hidden = false;

static inline v8::Local<v8::String> v8_str(const char* x) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  return v8::String::NewFromUtf8(isolate, x);
}

v8::Handle<v8::Value> CallNWTickCallback(void* env, const v8::Handle<v8::Value> ret) {
  blink::WebScopedMicrotaskSuppression suppression;
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

} //namespace

bool GetPackageImage(nw::Package* package,
                     const FilePath& icon_path,
                     gfx::Image* image) {
  FilePath path = package->ConvertToAbsoutePath(icon_path);

  // Read the file from disk.
  std::string file_contents;
  if (path.empty() || !base::ReadFileToString(path, &file_contents))
    return false;

  // Decode the bitmap using WebKit's image decoder.
  const unsigned char* data =
      reinterpret_cast<const unsigned char*>(file_contents.data());
  scoped_ptr<SkBitmap> decoded(new SkBitmap());
  // Note: This class only decodes bitmaps from extension resources. Chrome
  // doesn't (for security reasons) directly load extension resources provided
  // by the extension author, but instead decodes them in a separate
  // locked-down utility process. Only if the decoding succeeds is the image
  // saved from memory to disk and subsequently used in the Chrome UI.
  // Chrome is therefore decoding bitmaps here that were generated by Chrome.
  gfx::PNGCodec::Decode(data, file_contents.length(), decoded.get());
  if (decoded->empty())
    return false;  // Unable to decode.

  *image = gfx::Image::CreateFrom1xBitmap(*decoded);
  return true;
}

void SetTrustAnchors(IOThread* io_thread, const net::CertificateList& trust_anchors) {
  PolicyCertVerifier* verifier =
    (PolicyCertVerifier*)io_thread->globals()->cert_verifier.get();
  verifier->SetTrustAnchors(trust_anchors);
}

void MainPartsPreMainMessageLoopRunHook() {
  nw::Package* package = nw::package();
  const base::ListValue *additional_trust_anchors = NULL;
  if (package->root()->GetList("additional_trust_anchors", &additional_trust_anchors)) {
    net::CertificateList trust_anchors;
    for (size_t i = 0; i<additional_trust_anchors->GetSize(); i++) {
      std::string certificate_string;
      if (!additional_trust_anchors->GetString(i, &certificate_string)) {
        // LOG(WARNING)
        //   << "Could not get string from entry " << i;
        continue;
      }

      net::CertificateList loaded =
        net::X509Certificate::CreateCertificateListFromBytes(
        certificate_string.c_str(), certificate_string.size(),
        net::X509Certificate::FORMAT_AUTO);
      if (loaded.empty() && !certificate_string.empty()) {
        // LOG(WARNING)
        //   << "Could not load certificate from entry " << i;
        continue;
      }

      trust_anchors.insert(trust_anchors.end(), loaded.begin(), loaded.end());
    }
    if (!trust_anchors.empty()) {
      // LOG(INFO)
      //   << "Added " << trust_anchors.size() << " certificates to trust anchors.";
      content::BrowserThread::PostTask(
        content::BrowserThread::IO,
        FROM_HERE,
        base::Bind(SetTrustAnchors, g_browser_process->io_thread(), trust_anchors));
    }
  }
}

int MainPartsPreCreateThreadsHook() {
  base::ThreadRestrictions::ScopedAllowIO allow_io;
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  nw::Package* package = InitNWPackage();
  if (package && !package->path().empty()) {
    base::FilePath path = package->path().NormalizePathSeparators();

    command_line->AppendSwitchPath("nwapp", path);
    int dom_storage_quota_mb;
    if (package->root()->GetInteger("dom_storage_quota", &dom_storage_quota_mb)) {
      content::DOMStorageMap::SetQuotaOverride(dom_storage_quota_mb * 1024 * 1024);
    }

    base::FilePath user_data_dir;
    std::string name;
    package->root()->GetString("name", &name);
    if (!name.empty() && PathService::Get(chrome::DIR_USER_DATA, &user_data_dir)) {
      base::FilePath old_dom_storage_dir = user_data_dir
        .Append(FILE_PATH_LITERAL("Local Storage"));
      base::FileEnumerator enum0(old_dom_storage_dir, false, base::FileEnumerator::FILES, FILE_PATH_LITERAL("*_0.localstorage"));
      base::FilePath old_dom_storage = enum0.Next();
      if (!old_dom_storage.empty()) {
        std::string id = crx_file::id_util::GenerateId(name);
        GURL origin("chrome-extension://" + id + "/");
        base::FilePath new_storage_dir = user_data_dir.Append(FILE_PATH_LITERAL("Default"))
          .Append(FILE_PATH_LITERAL("Local Storage"));
        base::CreateDirectory(new_storage_dir);

        base::FilePath new_dom_storage = new_storage_dir
          .Append(content::DOMStorageArea::DatabaseFileNameFromOrigin(origin));
        base::FilePath new_dom_journal = new_dom_storage.ReplaceExtension(FILE_PATH_LITERAL("localstorage-journal"));
        base::FilePath old_dom_journal = old_dom_storage.ReplaceExtension(FILE_PATH_LITERAL("localstorage-journal"));
        base::Move(old_dom_journal, new_dom_journal);
        base::Move(old_dom_storage, new_dom_storage);
        LOG_IF(INFO, true) << "Migrate DOM storage from " << old_dom_storage.AsUTF8Unsafe() << " to " << new_dom_storage.AsUTF8Unsafe();
      }
      base::FilePath old_indexeddb = user_data_dir
        .Append(FILE_PATH_LITERAL("IndexedDB"))
        .Append(FILE_PATH_LITERAL("file__0.indexeddb.leveldb"));
      if (base::PathExists(old_indexeddb)) {
        std::string id = crx_file::id_util::GenerateId(name);
        GURL origin("chrome-extension://" + id + "/");
        base::FilePath new_indexeddb_dir = user_data_dir.Append(FILE_PATH_LITERAL("Default"))
          .Append(FILE_PATH_LITERAL("IndexedDB"))
          .AppendASCII(storage::GetIdentifierFromOrigin(origin))
          .AddExtension(FILE_PATH_LITERAL(".indexeddb.leveldb"));
        base::CreateDirectory(new_indexeddb_dir.DirName());
        base::CopyDirectory(old_indexeddb, new_indexeddb_dir.DirName(), true);
        base::Move(new_indexeddb_dir.DirName().Append(FILE_PATH_LITERAL("file__0.indexeddb.leveldb")), new_indexeddb_dir);
        LOG_IF(INFO, true) << "Migrated IndexedDB from " << old_indexeddb.AsUTF8Unsafe() << " to " << new_indexeddb_dir.AsUTF8Unsafe();
      }
    }

  }
  return content::RESULT_CODE_NORMAL_EXIT;
}

void MainPartsPostDestroyThreadsHook() {
  ReleaseNWPackage();
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
    blink::WebScopedMicrotaskSuppression suppression;
    v8::Context::Scope cscope(v8_context);
    // v8::Handle<v8::Value> result;
    frame->executeScriptAndReturnValue(WebScriptSource(jscript));
  }
}

void DocumentHook2(bool start, content::RenderFrame* frame, Dispatcher* dispatcher) {
  // ignore the first invocation of this hook for iframe
  // or we'll trigger creating a context with invalid type
  // there will follow another one with valid url
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
  if (!script_context || !script_context->extension())
    return;
  if (script_context->extension()->GetType() == Manifest::TYPE_NWJS_APP &&
      script_context->context_type() == Feature::BLESSED_EXTENSION_CONTEXT) {
    std::vector<v8::Handle<v8::Value> > arguments;
    v8::Local<v8::Value> window =
      web_frame->mainWorldScriptContext()->Global();
    arguments.push_back(v8::Boolean::New(isolate, start));
    arguments.push_back(window);
    script_context->module_system()->CallModuleMethod("nw.Window", "onDocumentStartEnd", &arguments);
  }
}

void DocumentElementHook(blink::WebLocalFrame* frame,
                         const extensions::Extension* extension,
                         const GURL& effective_document_url) {
  // ignore the first invocation of this hook for iframe
  // or we'll trigger creating a context with invalid type
  // there will follow another one with valid url
  GURL frame_url = ScriptContext::GetDataSourceURLForFrame(frame);
  if (frame->parent() && (!frame_url.is_valid() || frame_url.is_empty()))
    return;
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope hscope(isolate);
  frame->document().securityOrigin().grantUniversalAccess();
  frame->setNodeJS(true);
  std::string path = effective_document_url.path();
  v8::Local<v8::Context> v8_context = frame->mainWorldScriptContext();
  std::string root_path = extension->path().AsUTF8Unsafe();
  base::FilePath root(extension->path());
  if (!v8_context.IsEmpty()) {
    blink::WebScopedMicrotaskSuppression suppression;
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
    blink::WebScopedMicrotaskSuppression suppression;
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
    blink::WebScopedMicrotaskSuppression suppression;
    v8::Context::Scope cscope(v8_context);
    // v8::Handle<v8::Value> result;
    frame->executeScriptAndReturnValue(WebScriptSource(jscript));
  }
}

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
      blink::WebScopedMicrotaskSuppression suppression;

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
                                                       "process.versions['chromium'] = '" + chrome::kChromeVersion + "';").c_str()
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
    blink::WebScopedMicrotaskSuppression suppression;
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

void AmendManifestContentScriptList(base::DictionaryValue* manifest,
                                    const std::string& name,
                                    const std::string& run_at) {
  base::ListValue* scripts = NULL;
  base::Value* temp_value = NULL;
  bool amend = false;

  if (manifest->Get(manifest_keys::kContentScripts, &temp_value))
      if (temp_value->GetAsList(&scripts))
        amend = true;
  if (!scripts)
    scripts = new base::ListValue();

  std::string js;
  if (manifest->GetString(name, &js)) {
    base::ListValue* js_list = new base::ListValue();
    js_list->Append(new base::StringValue(js));

    base::ListValue* matches = new base::ListValue();
    matches->Append(new base::StringValue("<all_urls>"));

    base::DictionaryValue* content_script = new base::DictionaryValue();
    content_script->Set("js", js_list);
    content_script->Set("matches", matches);
    content_script->SetString("run_at", run_at);
    content_script->SetBoolean("in_main_world", true);

    scripts->Append(content_script);

    if (!amend)
      manifest->Set(manifest_keys::kContentScripts, scripts);
  }
}

void AmendManifestStringList(base::DictionaryValue* manifest,
                   const std::string& path,
                   const std::string& string_value) {
  base::ListValue* pattern_list = NULL;
  base::Value* temp_pattern_value = NULL;
  bool amend = false;

  if (manifest->Get(path, &temp_pattern_value))
      if (temp_pattern_value->GetAsList(&pattern_list))
        amend = true;
  if (!pattern_list)
    pattern_list = new base::ListValue();

  pattern_list->Append(new base::StringValue(string_value));
  if (!amend)
    manifest->Set(path, pattern_list);
}

void AmendManifestList(base::DictionaryValue* manifest,
                   const std::string& path,
                   const base::ListValue& list_value) {
  base::ListValue* pattern_list = NULL;

  if (manifest->GetList(path, &pattern_list)) {
    base::ListValue::const_iterator it;
    for(it = list_value.begin(); it != list_value.end(); ++it) {
      pattern_list->Append(*it);
    }
  } else {
    pattern_list = list_value.DeepCopy();
    manifest->Set(path, pattern_list);
  }
}

void LoadNWAppAsExtensionHook(base::DictionaryValue* manifest, std::string* error) {
  if (!manifest)
    return;

  std::string main_url, bg_script, icon_path;
  base::Value *node_remote = NULL;

  nw::Package* package = nw::package();
  manifest->SetBoolean(manifest_keys::kNWJSInternalFlag, true);
  if (error && !package->cached_error_content().empty()) {
    *error = package->cached_error_content();
    return;
  }

  manifest->Set(manifest_keys::kNWJSInternalManifest, package->root()->DeepCopy());

  if (manifest->GetString(manifest_keys::kNWJSMain, &main_url)) {
    if (base::EndsWith(main_url, ".js", base::CompareCase::INSENSITIVE_ASCII)) {
      AmendManifestStringList(manifest, manifest_keys::kPlatformAppBackgroundScripts, main_url);
      manifest->SetString(manifest_keys::kNWJSInternalMainFilename, main_url);
    }else
      AmendManifestStringList(manifest, manifest_keys::kPlatformAppBackgroundScripts, "nwjs/default.js");

    std::string bg_script;
    if (manifest->GetString("bg-script", &bg_script))
      AmendManifestStringList(manifest, manifest_keys::kPlatformAppBackgroundScripts, bg_script);

    AmendManifestStringList(manifest, manifest_keys::kPermissions, "developerPrivate");
    AmendManifestStringList(manifest, manifest_keys::kPermissions, "management");
    AmendManifestStringList(manifest, manifest_keys::kPermissions, "<all_urls>");
  }

  AmendManifestContentScriptList(manifest, "inject_js_start", "document_start");
  AmendManifestContentScriptList(manifest, "inject_js_end",   "document_end");

  if (manifest->GetString("window.icon", &icon_path)) {
    gfx::Image app_icon;
    if (GetPackageImage(package, base::FilePath::FromUTF8Unsafe(icon_path), &app_icon)) {
      int width = app_icon.Width();
      std::string key = "icons." + base::IntToString(width);
      manifest->SetString(key, icon_path);
    }
  }
  if (manifest->Get(switches::kmRemotePages, &node_remote)) {
    //FIXME: node-remote spec different with kWebURLs
    std::string node_remote_string;
    base::ListValue* node_remote_list = NULL;
    if (node_remote->GetAsString(&node_remote_string)) {
      node_remote_list = new base::ListValue();
      node_remote_list->Append(new base::StringValue(node_remote_string));
    } else if (node_remote->GetAsList(&node_remote_list)) {
      // do nothing
    }
    if (node_remote_list)
      AmendManifestList(manifest, manifest_keys::kWebURLs, *node_remote_list);
  }

  if (NWContentVerifierDelegate::GetDefaultMode() == ContentVerifierDelegate::ENFORCE_STRICT)
    manifest->SetBoolean(manifest_keys::kNWJSContentVerifyFlag, true);
}

void RendererProcessTerminatedHook(content::RenderProcessHost* process,
                                   const content::NotificationDetails& details) {
  content::RenderProcessHost::RendererClosedDetails* process_details =
    content::Details<content::RenderProcessHost::RendererClosedDetails>(details).ptr();
  int exit_code = process_details->exit_code;
#if defined(OS_POSIX)
  if (WIFEXITED(exit_code))
    exit_code = WEXITSTATUS(exit_code);
#endif
  SetExitCode(exit_code);
}

void OnRenderProcessShutdownHook(extensions::ScriptContext* context) {
  blink::WebScopedMicrotaskSuppression suppression;
  void* env = g_get_current_env_fn(context->v8_context());
  if (g_is_node_initialized_fn()) {
    g_emit_exit_fn(env);
    g_run_at_exit_fn(env);
  }
}

void KickNextTick() {
  blink::WebScopedMicrotaskSuppression suppression;
  void* env = g_get_node_env_fn();
  if (env)
    g_call_tick_callback_fn(env);
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
    switch (request.requestContext()) {
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

void CalcNewWinParams(content::WebContents* new_contents, void* params,
                      std::string* nw_inject_js_doc_start,
                      std::string* nw_inject_js_doc_end) {
  extensions::AppWindow::CreateParams ret;
  scoped_ptr<base::Value> val;
  scoped_ptr<base::DictionaryValue> manifest = MergeManifest();

  bool resizable;
  if (manifest->GetBoolean(switches::kmResizable, &resizable)) {
    ret.resizable = resizable;
  }
  bool fullscreen;
  if (manifest->GetBoolean(switches::kmFullscreen, &fullscreen) && fullscreen) {
    ret.state = ui::SHOW_STATE_FULLSCREEN;
  }
  int width = 0, height = 0;
  if (manifest->GetInteger(switches::kmWidth, &width))
    ret.content_spec.bounds.set_width(width);
  if (manifest->GetInteger(switches::kmHeight, &height))
    ret.content_spec.bounds.set_height(height);

  int x = 0, y = 0;
  if (manifest->GetInteger(switches::kmX, &x))
    ret.window_spec.bounds.set_x(x);
  if (manifest->GetInteger(switches::kmY, &y))
    ret.window_spec.bounds.set_y(y);
  bool top;
  if (manifest->GetBoolean(switches::kmAlwaysOnTop, &top) && top) {
    ret.always_on_top = true;
  }
  bool frame;
  if (manifest->GetBoolean(switches::kmFrame, &frame) && !frame) {
    ret.frame = extensions::AppWindow::FRAME_NONE;
  }
  bool all_workspaces;
  if (manifest->GetBoolean(switches::kmVisibleOnAllWorkspaces, &all_workspaces)
    && all_workspaces) {
    ret.visible_on_all_workspaces = true;
  }
  gfx::Size& minimum_size = ret.content_spec.minimum_size;
  int min_height = 0, min_width = 0;
  if (manifest->GetInteger(switches::kmMinWidth, &min_width))
    minimum_size.set_width(min_width);
  if (manifest->GetInteger(switches::kmMinHeight, &min_height))
    minimum_size.set_height(min_height);
  int max_height = 0, max_width = 0;
  gfx::Size& maximum_size = ret.content_spec.maximum_size;
  if (manifest->GetInteger(switches::kmMaxWidth, &max_width))
    maximum_size.set_width(max_width);
  if (manifest->GetInteger(switches::kmMaxHeight, &max_height))
    maximum_size.set_height(max_height);

  *(extensions::AppWindow::CreateParams*)params = ret;

  manifest->GetString(switches::kmInjectJSDocStart, nw_inject_js_doc_start);
  manifest->GetString(switches::kmInjectJSDocEnd, nw_inject_js_doc_end);
}

bool GetImage(Package* package, const FilePath& icon_path, gfx::Image* image) {
  FilePath path = package->ConvertToAbsoutePath(icon_path);

  // Read the file from disk.
  std::string file_contents;
  if (path.empty() || !base::ReadFileToString(path, &file_contents))
    return false;

  // Decode the bitmap using WebKit's image decoder.
  const unsigned char* data =
      reinterpret_cast<const unsigned char*>(file_contents.data());
  scoped_ptr<SkBitmap> decoded(new SkBitmap());
  // Note: This class only decodes bitmaps from extension resources. Chrome
  // doesn't (for security reasons) directly load extension resources provided
  // by the extension author, but instead decodes them in a separate
  // locked-down utility process. Only if the decoding succeeds is the image
  // saved from memory to disk and subsequently used in the Chrome UI.
  // Chrome is therefore decoding bitmaps here that were generated by Chrome.
  gfx::PNGCodec::Decode(data, file_contents.length(), decoded.get());
  if (decoded->empty())
    return false;  // Unable to decode.

  *image = gfx::Image::CreateFrom1xBitmap(*decoded);
  return true;
}

scoped_ptr<base::DictionaryValue> MergeManifest() {
  // Following attributes will not be inherited from package.json 
  // Keep this list consistent with documents in `Manifest Format.md`
  static std::vector<const char*> non_inherited_attrs = {
                                                    switches::kmFullscreen,
                                                    switches::kmKiosk,
                                                    switches::kmPosition,
                                                    switches::kmResizable,
                                                    switches::kmShow
                                                    };
  scoped_ptr<base::DictionaryValue> manifest;

  // retrieve `window` manifest set by `new-win-policy`
  std::string manifest_str = base::UTF16ToUTF8(nw::GetCurrentNewWinManifest());
  scoped_ptr<base::Value> val = base::JSONReader().ReadToValue(manifest_str);
  if (val && val->IsType(base::Value::Type::TYPE_DICTIONARY)) {
    manifest.reset(static_cast<base::DictionaryValue*>(val.release()));
  } else {
    manifest.reset(new base::DictionaryValue());
  }

  // merge with default `window` manifest in package.json if exists
  nw::Package* pkg = nw::package();
  if (pkg) {
    base::DictionaryValue* manifest_window = pkg->window();
    if (manifest_window) {
      scoped_ptr<base::DictionaryValue> manifest_window_cloned = manifest_window->DeepCopyWithoutEmptyChildren();
      // filter out non inherited attributes
      std::vector<const char*>::iterator it;
      for(it = non_inherited_attrs.begin(); it != non_inherited_attrs.end(); it++) {
        manifest_window_cloned->RemoveWithoutPathExpansion(*it, NULL);
      }
      // overwrite default `window` manifest with the one passed by `new-win-policy`
      manifest_window_cloned->MergeDictionary(manifest.get());
      return manifest_window_cloned;
    }
  }

  return manifest;
}

bool ExecuteAppCommandHook(int command_id, extensions::AppWindow* app_window) {
#if defined(OS_MACOSX)
  return false;
#else
  //nw::ObjectManager* obj_manager = nw::ObjectManager::Get(app_window->browser_context());
  //Menu* menu = (Menu*)obj_manager->GetApiObject(command_id);
  Menu* menu = app_window->menu_;
  if (!menu)
    return false;
  menu->menu_delegate_->ExecuteCommand(command_id, 0);
  return true;
#endif //OSX
}

void SendEventToApp(const std::string& event_name, scoped_ptr<base::ListValue> event_args) {
  Profile* profile = ProfileManager::GetActiveUserProfile();
  const extensions::ExtensionSet& extensions =
    ExtensionRegistry::Get(profile)->enabled_extensions();
  ExtensionPrefs* extension_prefs = ExtensionPrefs::Get(profile);

  for (extensions::ExtensionSet::const_iterator it = extensions.begin();
       it != extensions.end(); ++it) {
    const Extension* extension = it->get();
    if (extension_prefs->IsExtensionRunning(extension->id()) &&
        extension->location() == extensions::Manifest::COMMAND_LINE) {
      scoped_ptr<extensions::Event> event(new extensions::Event(extensions::events::UNKNOWN,
                                                                event_name,
                                                                std::move(event_args)));
      event->restrict_to_browser_context = profile;
      EventRouter::Get(profile)
        ->DispatchEventToExtension(extension->id(), std::move(event));
    }
  }
}

bool ProcessSingletonNotificationCallbackHook(const base::CommandLine& command_line,
                                              const base::FilePath& current_directory) {
  nw::Package* package = nw::package();
  bool single_instance = true;
  package->root()->GetBoolean(switches::kmSingleInstance, &single_instance);
  if (single_instance) {
#if defined(OS_WIN)
    std::string cmd = base::UTF16ToUTF8(command_line.GetCommandLineString());
#else
    std::string cmd = command_line.GetCommandLineString();
#endif
    scoped_ptr<base::ListValue> arguments(new base::ListValue());
    arguments->AppendString(cmd);
    SendEventToApp("nw.App.onOpen", std::move(arguments));
  }
    
  return single_instance;
}

#if defined(OS_MACOSX)
bool ApplicationShouldHandleReopenHook(bool hasVisibleWindows) {
  scoped_ptr<base::ListValue> arguments(new base::ListValue());
  SendEventToApp("nw.App.onReopen",std::move(arguments));
  return true;
}

void OSXOpenURLsHook(const std::vector<GURL>& startup_urls) {
  scoped_ptr<base::ListValue> arguments(new base::ListValue());
  for (size_t i = 0; i < startup_urls.size(); i++)
    arguments->AppendString(startup_urls[i].spec());
  SendEventToApp("nw.App.onOpen", std::move(arguments));
}
#endif

static std::string g_user_agent;

void SetUserAgentOverride(const std::string& agent,
                          const std::string& name,
                          const std::string& version) {
  g_user_agent = agent;
  base::ReplaceSubstringsAfterOffset(&g_user_agent, 0, "%name", name);
  base::ReplaceSubstringsAfterOffset(&g_user_agent, 0, "%ver", version);
  base::ReplaceSubstringsAfterOffset(&g_user_agent, 0, "%nwver", NW_VERSION_STRING);
  base::ReplaceSubstringsAfterOffset(&g_user_agent, 0, "%webkit_ver", content::GetWebKitVersion());
  base::ReplaceSubstringsAfterOffset(&g_user_agent, 0, "%osinfo", content::BuildOSInfo());
}

bool GetUserAgentFromManifest(std::string* agent) {
  if (!g_user_agent.empty()) {
    *agent = g_user_agent;
    return true;
  }
  nw::Package* package = nw::package();
  if (!package)
    return false;
  if (package->root()->GetString(switches::kmUserAgent, &g_user_agent)) {
    std::string name, version;
    package->root()->GetString(switches::kmName, &name);
    package->root()->GetString("version", &version);
    SetUserAgentOverride(g_user_agent, name, version);
    *agent = g_user_agent;
    return true;
  }
  return false;
}

void ReloadExtensionHook(const extensions::Extension* extension) {
  g_reloading_app = true;
  chrome::IncrementKeepAliveCount(); //keep alive count is paired with
                                     //appwindow's lifetime, so it's
                                     //needed to keep alive during
                                     //reload
}

void CreateAppWindowHook(extensions::AppWindow* app_window) {
  if (g_reloading_app) {
    g_reloading_app = false;
    chrome::DecrementKeepAliveCount();
  }
}

bool IsReloadingApp() {
  return g_reloading_app;
}

void OverrideWebkitPrefsHook(content::RenderViewHost* rvh, content::WebPreferences* web_prefs) {
  nw::Package* package = nw::package();
  if (!package)
    return;
  base::DictionaryValue* webkit;
  web_prefs->plugins_enabled = true;
  if (package->root()->GetDictionary(switches::kmWebkit, &webkit)) {
    webkit->GetBoolean("double_tap_to_zoom_enabled", &web_prefs->double_tap_to_zoom_enabled);
    webkit->GetBoolean("plugin",                     &web_prefs->plugins_enabled);
  }
  FilePath plugins_dir = package->path();

  plugins_dir = plugins_dir.AppendASCII("plugins");

  content::PluginService* plugin_service = content::PluginService::GetInstance();
  plugin_service->AddExtraPluginDir(plugins_dir);
}

bool CheckStoragePartitionMatches(int render_process_id, const GURL& url) {
  return render_process_id == g_cdt_process_id && url.SchemeIs(content_settings::kChromeDevToolsScheme);
}

bool RphGuestFilterURLHook(RenderProcessHost* rph, const GURL* url)  {
  extensions::WebViewRendererState* renderer_state =
      extensions::WebViewRendererState::GetInstance();
  std::string owner_extension;
  int process_id = rph->GetID();
  if (!renderer_state->GetOwnerInfo(process_id, nullptr, &owner_extension))
    return false;
  const Extension* extension =
    ExtensionRegistry::Get(rph->GetBrowserContext())->enabled_extensions().GetByID(owner_extension);
  if (!extension)
    return false;
  bool file_scheme = false;
  if (WebviewInfo::IsURLWebviewAccessible(extension,
                                          WebViewGuest::GetPartitionID(rph),
                                          *url, &file_scheme)) {
    if (file_scheme) {
      content::ChildProcessSecurityPolicy::GetInstance()->GrantScheme(
          process_id, url::kFileScheme);
    }
    return true;
  }
  return false;
}

bool ShouldServiceRequestHook(int child_id, const GURL& url) {
  RenderProcessHost* rph = RenderProcessHost::FromID(child_id);
  if (!rph)
    return false;
  return RphGuestFilterURLHook(rph, &url);
}

void ShowDevtools(bool show, content::WebContents* web_contents, content::WebContents* container) {
  content::RenderFrameHost* rfh = web_contents->GetMainFrame();
  if (container) {
    scoped_refptr<DevToolsAgentHost> agent_host(DevToolsAgentHost::GetOrCreateFor(rfh));
    g_cdt_process_id = container->GetRenderProcessHost()->GetID();
    content::ChildProcessSecurityPolicy::GetInstance()->GrantAll(g_cdt_process_id);
    
    DevToolsWindow* window = DevToolsWindow::FindDevToolsWindow(agent_host.get());
    if (!window) {
      Profile* profile = Profile::FromBrowserContext(
             web_contents->GetBrowserContext());
      window = DevToolsWindow::Create(profile, GURL(), nullptr, false, std::string(), false, std::string(), container);
      if (!window)
        return;
      window->bindings_->AttachTo(agent_host);
    }
    return;
  }

  if (show) {
    DevToolsWindow::InspectElement(rfh, 0, 0);
    return;
  }
  scoped_refptr<DevToolsAgentHost> agent(
      DevToolsAgentHost::GetOrCreateFor(rfh));
  DevToolsWindow* window = DevToolsWindow::FindDevToolsWindow(agent.get());
  if (window)
    window->InspectedContentsClosing();
}

bool PinningRenderer() {
  return g_pinning_renderer;
}

void SetPinningRenderer(bool pin) {
  g_pinning_renderer = pin;
}

bool RenderWidgetWasHiddenHook(content::RenderWidget* rw) {
  return g_skip_render_widget_hidden;
}

} //namespace nw
