// Copyright (c) 2012 Intel Corp
// Copyright (c) 2012 The Chromium Authors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell co
// pies of the Software, and to permit persons to whom the Software is furnished
//  to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in al
// l copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IM
// PLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNES
// S FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
//  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WH
// ETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#define V8_USE_UNSAFE_HANDLES

#include "content/nw/src/renderer/shell_content_renderer_client.h"

#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/posix/global_descriptors.h"
#include "base/strings/string16.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "components/autofill/content/renderer/page_click_tracker.h"
#include "components/autofill/content/renderer/autofill_agent.h"
#include "components/autofill/content/renderer/password_autofill_agent.h"
#include "components/autofill/content/renderer/password_generation_agent.h"
#include "content/common/view_messages.h"
#include "content/nw/src/api/dispatcher.h"
#include "content/nw/src/api/api_messages.h"
#include "content/nw/src/api/window_bindings.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_version.h"
#include "components/autofill/content/renderer/autofill_agent.h"
#include "components/autofill/content/renderer/password_autofill_agent.h"
#include "content/nw/src/renderer/nw_render_view_observer.h"
#include "chrome/renderer/pepper/pepper_helper.h"
#include "content/nw/src/renderer/prerenderer/prerenderer_client.h"
#include "content/nw/src/renderer/printing/print_web_view_helper.h"
#include "content/nw/src/renderer/shell_render_process_observer.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/content_constants.h"
#include "content/public/renderer/render_frame_observer_tracker.h"
#include "content/public/renderer/render_thread.h"
#include "content/public/renderer/render_view.h"
#include "content/renderer/render_view_impl.h"
#include "ipc/ipc_descriptors.h"
#include "net/proxy/proxy_bypass_rules.h"
#include "third_party/node/src/node.h"
#undef CHECK
#include "third_party/node/src/node_internals.h"
#include "third_party/node/src/req_wrap.h"
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebSecurityOrigin.h"
#include "third_party/WebKit/public/web/WebSecurityPolicy.h"
#include "third_party/WebKit/public/web/WebView.h"
#include "gin/public/context_holder.h"
#include "gin/public/gin_embedders.h"

//#include "third_party/WebKit/Source/WebKit/chromium/public/platform/WebString.h"
#include "content/common/dom_storage/dom_storage_map.h"

#include "nw/id/commit.h"

#include "extensions/common/extensions_client.h"
#include "extensions/common/switches.h"
#include "extensions/renderer/default_dispatcher_delegate.h"
#include "extensions/renderer/dispatcher.h"
#include "extensions/renderer/extension_helper.h"
#include "extensions/renderer/guest_view/extensions_guest_view_container.h"
#include "extensions/renderer/guest_view/mime_handler_view/mime_handler_view_container.h"
#include "content/nw/src/common/shell_extensions_client.h"
#include "content/nw/src/renderer/shell_extensions_renderer_client.h"


using content::RenderView;
using content::RenderViewImpl;
using autofill::AutofillAgent;
using autofill::PasswordAutofillAgent;
using autofill::PasswordGenerationAgent;
using net::ProxyBypassRules;
using blink::WebFrame;
using blink::WebLocalFrame;
using blink::WebView;
using blink::WebString;
using blink::WebSecurityPolicy;
using extensions::ExtensionsClient;

namespace content {

namespace {

// TODO: promote ExtensionFrameHelper to a common place and share with this.
class ShellFrameHelper
    : public content::RenderFrameObserver,
      public content::RenderFrameObserverTracker<ShellFrameHelper> {
 public:
  ShellFrameHelper(content::RenderFrame* render_frame,
                   extensions::Dispatcher* extension_dispatcher);
  ~ShellFrameHelper() override;

  // RenderFrameObserver implementation.
  void WillReleaseScriptContext(v8::Handle<v8::Context>, int world_id) override;

 private:
  extensions::Dispatcher* extension_dispatcher_;

  DISALLOW_COPY_AND_ASSIGN(ShellFrameHelper);
};

ShellFrameHelper::ShellFrameHelper(content::RenderFrame* render_frame,
                                   extensions::Dispatcher* extension_dispatcher)
    : content::RenderFrameObserver(render_frame),
      content::RenderFrameObserverTracker<ShellFrameHelper>(render_frame),
      extension_dispatcher_(extension_dispatcher) {
}

ShellFrameHelper::~ShellFrameHelper() {
}

void ShellFrameHelper::WillReleaseScriptContext(v8::Handle<v8::Context> context,
                                                int world_id) {
  extension_dispatcher_->WillReleaseScriptContext(
      render_frame()->GetWebFrame(), context, world_id);
}

RenderView* GetCurrentRenderView() {
  WebLocalFrame* frame = WebLocalFrame::frameForCurrentContext();
  DCHECK(frame);
  if (!frame)
    return NULL;

  WebView* view = frame->view();
  if (!view)
    return NULL;  // can happen during closing.

  RenderView* render_view = RenderView::FromWebView(view);
  DCHECK(render_view);
  return render_view;
}

}  // namespace

ShellContentRendererClient::ShellContentRendererClient()
  :in_nav_cb_(false), creating_first_context_(true) {
}

ShellContentRendererClient::~ShellContentRendererClient() {
}

void ShellContentRendererClient::RenderThreadStarted() {
  content::RenderThread* thread = content::RenderThread::Get();

  // Change working directory.
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  if (command_line->HasSwitch(switches::kWorkingDirectory)) {
    base::SetCurrentDirectory(
        command_line->GetSwitchValuePath(switches::kWorkingDirectory));
  }

  if (command_line->HasSwitch(switches::kDomStorageQuota)) {
    std::string quota_str = command_line->GetSwitchValueASCII(switches::kDomStorageQuota);
    int quota = 0;
    if (base::StringToInt(quota_str, &quota) && quota > 0) {
      content::DOMStorageMap::SetQuotaOverride(quota * 1024 * 1024);
    }
  }

  if (command_line->HasSwitch(extensions::switches::kExtensionProcess)) {
    extensions_client_.reset(CreateExtensionsClient());
    extensions::ExtensionsClient::Set(extensions_client_.get());

    extensions_renderer_client_.reset(new extensions::ShellExtensionsRendererClient);
    extensions::ExtensionsRendererClient::Set(extensions_renderer_client_.get());

    extension_dispatcher_delegate_.reset(new extensions::DefaultDispatcherDelegate());

    // Must be initialized after ExtensionsRendererClient.
    extension_dispatcher_.reset(
                                new extensions::Dispatcher(extension_dispatcher_delegate_.get()));
    thread->AddObserver(extension_dispatcher_.get());
  }

  // Start observers.
  shell_observer_.reset(new ShellRenderProcessObserver());

  WebString file_scheme(base::ASCIIToUTF16("file"));
  WebString app_scheme(base::ASCIIToUTF16("app"));
  // file: resources should be allowed to receive CORS requests.
  WebSecurityPolicy::registerURLSchemeAsCORSEnabled(file_scheme);
  WebSecurityPolicy::registerURLSchemeAsCORSEnabled(app_scheme);

}

void ShellContentRendererClient::RenderViewCreated(RenderView* render_view) {
  new nwapi::Dispatcher(render_view);
  new nw::NwRenderViewObserver(render_view);
  new prerender::PrerendererClient(render_view);
#if defined(ENABLE_PRINTING)
  new printing::PrintWebViewHelper(render_view, true, true, NULL);
#endif

  if (extension_dispatcher_.get()) {
    new extensions::ExtensionHelper(render_view, extension_dispatcher_.get());
    extension_dispatcher_->OnRenderViewCreated(render_view);
  }

  // The PageClickTracker is a RenderViewObserver, and hence will be freed when
  // the RenderView is destroyed.
  // FIXME: new autofill::PageClickTracker(render_view, autofill_agent);

  // PasswordAutofillAgent* password_autofill_agent =
  //     new PasswordAutofillAgent(render_view);
  // new AutofillAgent(render_view, password_autofill_agent);
}

void ShellContentRendererClient::DidCreateScriptContext(
    blink::WebFrame* frame,
    v8::Handle<v8::Context> context,
    int extension_group,
    int world_id) {
  GURL url(frame->document().url());
  VLOG(1) << "DidCreateScriptContext: " << url;
  InstallNodeSymbols(frame, context, url);
  creating_first_context_ = false;

#if 1 //defined(NW_IMPLEMENTATION)
    v8::Local<v8::Context> node_context =
        v8::Local<v8::Context>::New(context->GetIsolate(), node::g_context);
    //need to stay sync with V8PerContextData.h
    int v8ContextPerContextDataIndex = static_cast<int>(gin::kPerContextDataStartIndex + gin::kEmbedderBlink);
    node_context->SetAlignedPointerInEmbedderData(v8ContextPerContextDataIndex,
                                                  context->GetAlignedPointerFromEmbedderData(v8ContextPerContextDataIndex));
#endif


  if (extension_dispatcher_.get())
    extension_dispatcher_->DidCreateScriptContext(frame, context, extension_group, world_id);
}

bool ShellContentRendererClient::goodForNode(blink::WebFrame* frame)
{
  RenderViewImpl* rv = RenderViewImpl::FromWebView(frame->view());
  GURL url(frame->document().url());

  // the way to tell the loading URL is not reliable in some cases
  // like navigation and window.open. Fortunately we are in
  // willHandleNavigationPolicy callback so we can use the request url
  // from there

  if (url.is_empty() && in_nav_cb_) {
    ASSERT(!in_nav_url_.empty());
    url = GURL(in_nav_url_);
  }

  ProxyBypassRules rules;
  rules.ParseFromString(rv->renderer_preferences_.nw_remote_page_rules);
  bool force_on = rules.Matches(url);
  bool is_nw_protocol = url.SchemeIs("nw") || !url.is_valid();
  bool use_node =
    base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kNodejs) &&
    !frame->isNwDisabledChildFrame() &&
    (force_on || url.SchemeIsFile() || is_nw_protocol
     || url.SchemeIs("chrome-extension")
     || url.SchemeIs("app"));
  return use_node;
}

void ShellContentRendererClient::SetupNodeUtil(
    blink::WebFrame* frame,
    v8::Handle<v8::Context> context) {
  RenderViewImpl* rv = RenderViewImpl::FromWebView(frame->view());
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  std::string root_path = rv->renderer_preferences_.nw_app_root_path.AsUTF8Unsafe();
#if defined(OS_WIN)
  base::ReplaceChars(root_path, "\\", "\\\\", &root_path);
#endif
  base::ReplaceChars(root_path, "'", "\\'", &root_path);
  v8::Local<v8::Script> script = v8::Script::Compile(v8::String::NewFromUtf8(isolate, (
        // Make node's relative modules work
        "if (typeof process != 'undefined' && (!process.mainModule.filename || process.mainModule.filename === 'blank')) {"
        "  var root = '" + root_path + "';"
#if defined(OS_WIN)
        "process.mainModule.filename = decodeURIComponent(window.location.pathname === 'blank' ? 'blank': window.location.pathname.substr(1));"
#else
        "process.mainModule.filename = decodeURIComponent(window.location.pathname);"
#endif
        "if (window.location.href.indexOf('app://') === 0) {process.mainModule.filename = root + '/' + process.mainModule.filename}"
        "process.mainModule.paths = global.require('module')._nodeModulePaths(process.cwd());"
        "process.mainModule.loaded = true;"
        "}").c_str()),
  v8::String::NewFromUtf8(isolate, "process_main"));
  CHECK(*script);
  script->Run();
}

bool ShellContentRendererClient::WillSetSecurityToken(
    blink::WebFrame* frame,
    v8::Handle<v8::Context> context) {
  GURL url(frame->document().url());
  VLOG(1) << "WillSetSecurityToken: " << url;
  if (goodForNode(frame)) {
    VLOG(1) << "GOOD FOR NODE";
    // Override context's security token
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scope(isolate);
    v8::Local<v8::Context> g_context =
      v8::Local<v8::Context>::New(isolate, node::g_context);
    context->SetSecurityToken(g_context->GetSecurityToken());
    frame->document().securityOrigin().grantUniversalAccess();

    int ret = 0;
    RenderViewImpl* rv = RenderViewImpl::FromWebView(frame->view());
    rv->Send(new ViewHostMsg_GrantUniversalPermissions(rv->GetRoutingID(), &ret));
    SetupNodeUtil(frame, context);
    return true;
  }

  // window.open frame was enabled for Node earlier
  if (frame->isNodeJS()) {
    frame->setNodeJS(false);
    UninstallNodeSymbols(frame, context);
  }
  return false;
}

void ShellContentRendererClient::InstallNodeSymbols(
    blink::WebFrame* frame,
    v8::Handle<v8::Context> context,
    const GURL& url) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);
  v8::Local<v8::Context> g_context =
    v8::Local<v8::Context>::New(isolate, node::g_context);

  static bool installed_once = false;

  v8::Local<v8::Object> nodeGlobal = g_context->Global();
  v8::Local<v8::Object> v8Global = context->Global();

  // Use WebKit's console globally
  nodeGlobal->Set(v8::String::NewFromUtf8(isolate, "console"),
                  v8Global->Get(v8::String::NewFromUtf8(isolate, "console")));

  // Do we integrate node?
  bool use_node = goodForNode(frame);

  // Test if protocol is 'nw:'
  // test for 'about:blank' is also here becuase window.open would
  // open 'about:blank' first // FIXME
  bool is_nw_protocol = url.SchemeIs("nw") || !url.is_valid();
  bool is_ext_protocol = url.SchemeIs("chrome-extension");
  if (use_node || is_nw_protocol) {
    frame->setNodeJS(true);

    v8::Local<v8::Array> symbols = v8::Array::New(isolate, 4);
    symbols->Set(0, v8::String::NewFromUtf8(isolate, "global"));
    symbols->Set(1, v8::String::NewFromUtf8(isolate, "process"));
    symbols->Set(2, v8::String::NewFromUtf8(isolate, "Buffer"));
    symbols->Set(3, v8::String::NewFromUtf8(isolate, "root"));

    g_context->Enter();
    for (unsigned i = 0; i < symbols->Length(); ++i) {
      v8::Local<v8::Value> key = symbols->Get(i);
      v8::Local<v8::Value> val = nodeGlobal->Get(key);
      v8Global->Set(key, val);
    }
    g_context->Exit();
    context->SetAlignedPointerInEmbedderData(NODE_CONTEXT_EMBEDDER_DATA_INDEX, node::g_env);
    if (!installed_once) {
      installed_once = true;

      // The following listener on process should not be added each
      // time when a document is created, or it will leave the
      // reference to the closure created by the call back and leak
      // memory (see #203)

      nodeGlobal->Set(v8::String::NewFromUtf8(isolate, "window"), v8Global);

      // Listen uncaughtException with ReportException.
      v8::Local<v8::Function> cb = v8::FunctionTemplate::New(isolate, ReportException)->
        GetFunction();
      v8::Local<v8::Value> argv[] = { v8::String::NewFromUtf8(isolate, "uncaughtException"), cb };
      node::MakeCallback(isolate, node::g_env->process_object(), "on", 2, argv);
    }
  }

  if (use_node) {
    SetupNodeUtil(frame, context);
  }

  if (use_node || is_nw_protocol) {
    v8::Context::Scope cscope(context);
    {
      v8::TryCatch try_catch;
      v8::Local<v8::Script> script = v8::Script::Compile(v8::String::NewFromUtf8(isolate,
        // Overload require
        "window.require = function(name) { \n"
        "  if (name == 'nw.gui') \n"
        "    return nwDispatcher.requireNwGui(); \n"
        "  return global.require(name); \n"
        "}; \n"

        // Save node-webkit version
        "process.versions['node-webkit'] = '" NW_VERSION_STRING "';"
        "process.versions['nw-commit-id'] = '" NW_COMMIT_HASH "';"
        "process.versions['chromium'] = '" CHROME_VERSION "';"
                                                                                 ));
      script->Run();
      if (try_catch.HasCaught()) {
        v8::Handle<v8::Message> message = try_catch.Message();
        LOG(FATAL) << *v8::String::Utf8Value(message->Get());
      }
    }
    if (!is_ext_protocol) {
      v8::TryCatch try_catch;
      v8::Local<v8::Script> script2 = v8::Script::Compile(v8::String::NewFromUtf8(isolate,
        "  nwDispatcher.requireNwGui().Window.get();"
                                                                                  ),
                                                          v8::String::NewFromUtf8(isolate, "initial_require"));
      script2->Run();
      if (try_catch.HasCaught()) {
        v8::Handle<v8::Message> message = try_catch.Message();
        LOG(FATAL) << *v8::String::Utf8Value(message->Get());
      }
    }
  } else {
#if 0
    int ret;
    RenderViewImpl* render_view = RenderViewImpl::FromWebView(frame->view());

    if (frame->parent() == NULL && creating_first_context_) {
      // do this only for top frames, or initialization of iframe
      // could override parent settings here
      render_view->Send(new ShellViewHostMsg_SetForceClose(
            render_view->GetRoutingID(), true, &ret));
    }
#endif
  }
}

// static
void ShellContentRendererClient::ReportException(
            const v8::FunctionCallbackInfo<v8::Value>&  args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  // Do nothing if user is listening to uncaughtException.
  v8::Local<v8::Value> listeners_v =
    node::g_env->process_object()->Get(v8::String::NewFromUtf8(isolate, "listeners"));
  v8::Local<v8::Function> listeners =
      v8::Local<v8::Function>::Cast(listeners_v);

  v8::Local<v8::Value> argv[1] = { v8::String::NewFromUtf8(isolate, "uncaughtException") };
  v8::Local<v8::Value> ret = listeners->Call(node::g_env->process_object(), 1, argv);
  v8::Local<v8::Array> listener_array = v8::Local<v8::Array>::Cast(ret);

  uint32_t length = listener_array->Length();
  if (length > 1) {
    args.GetReturnValue().Set(v8::Undefined(isolate));
    return;
  }

  // Print stacktrace.
  v8::Local<v8::String> stack_symbol = v8::String::NewFromUtf8(isolate, "stack");
  std::string error;

  v8::Local<v8::Object> exception = args[0]->ToObject();
  if (exception->Has(stack_symbol))
    error = *v8::String::Utf8Value(exception->Get(stack_symbol));
  else
    error = *v8::String::Utf8Value(exception);

  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    args.GetReturnValue().Set(v8::Undefined(isolate));
    return;
  }

  render_view->Send(new ShellViewHostMsg_UncaughtException(
      render_view->GetRoutingID(),
      error));

  args.GetReturnValue().Set(v8::Undefined(isolate));
}

void ShellContentRendererClient::UninstallNodeSymbols(
    blink::WebFrame* frame,
    v8::Handle<v8::Context> context) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  v8::Local<v8::Object> v8Global = context->Global();
  v8::Local<v8::Array> symbols = v8::Array::New(isolate, 5);
  symbols->Set(0, v8::String::NewFromUtf8(isolate, "global"));
  symbols->Set(1, v8::String::NewFromUtf8(isolate, "process"));
  symbols->Set(2, v8::String::NewFromUtf8(isolate, "Buffer"));
  symbols->Set(3, v8::String::NewFromUtf8(isolate, "root"));
  symbols->Set(4, v8::String::NewFromUtf8(isolate, "require"));

  for (unsigned i = 0; i < symbols->Length(); ++i) {
    v8::Local<v8::String> key = symbols->Get(i)->ToString();
    if(v8Global->Has(key))
      v8Global->Delete(key);
  }
}

void ShellContentRendererClient::willHandleNavigationPolicy(
    RenderView* rv,
    blink::WebFrame* frame,
    const blink::WebURLRequest& request,
    blink::WebNavigationPolicy* policy,
    blink::WebString* manifest) {

  nwapi::Dispatcher::willHandleNavigationPolicy(rv, frame, request, policy, manifest);
}

void ShellContentRendererClient::windowOpenBegin(const blink::WebURL& url) {
  in_nav_cb_ = true;
  in_nav_url_ =url.string().utf8();
}

void ShellContentRendererClient::windowOpenEnd() {
  in_nav_cb_ = false;
}

ExtensionsClient* ShellContentRendererClient::CreateExtensionsClient() {
  return new extensions::ShellExtensionsClient;
}

void ShellContentRendererClient::RenderFrameCreated(
    content::RenderFrame* render_frame) {
  // ShellFrameHelper destroys itself when the RenderFrame is
  // destroyed.
  if (extension_dispatcher_.get())
    new ShellFrameHelper(render_frame, extension_dispatcher_.get());
#if defined(ENABLE_PLUGINS)
  new PepperHelper(render_frame);
#endif

  PasswordGenerationAgent* password_generation_agent =
      new PasswordGenerationAgent(render_frame);
  PasswordAutofillAgent* password_autofill_agent =
      new PasswordAutofillAgent(render_frame);
  new AutofillAgent(render_frame,
                    password_autofill_agent,
                    password_generation_agent);

  // TODO(jamescook): Do we need to add a new PepperHelper(render_frame) here?
  // It doesn't seem necessary for either Pepper or NaCl.
  // http://crbug.com/403004
}

content::BrowserPluginDelegate*
ShellContentRendererClient::CreateBrowserPluginDelegate(
    content::RenderFrame* render_frame,
    const std::string& mime_type,
    const GURL& original_url) {
  if (mime_type == content::kBrowserPluginMimeType) {
    return new extensions::ExtensionsGuestViewContainer(render_frame);
  } else {
    return new extensions::MimeHandlerViewContainer(
        render_frame, mime_type, original_url);
  }
}

bool ShellContentRendererClient::ShouldForwardToGuestContainer(
    const IPC::Message& msg) {
  return extensions::GuestViewContainer::HandlesMessage(msg);
}


}  // namespace content
