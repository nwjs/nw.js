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
#include "base/file_util.h"
#include "base/logging.h"
#include "base/posix/global_descriptors.h"
#include "base/strings/string16.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "components/autofill/content/renderer/page_click_tracker.h"
#include "content/common/view_messages.h"
#include "content/nw/src/api/dispatcher.h"
#include "content/nw/src/api/api_messages.h"
#include "content/nw/src/api/window_bindings.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_version.h"
#include "components/autofill/content/renderer/autofill_agent.h"
#include "components/autofill/content/renderer/password_autofill_agent.h"
#include "content/nw/src/renderer/autofill_agent.h"
#include "content/nw/src/renderer/nw_render_view_observer.h"
#include "content/nw/src/renderer/prerenderer/prerenderer_client.h"
#include "content/nw/src/renderer/printing/print_web_view_helper.h"
#include "content/nw/src/renderer/shell_render_process_observer.h"
#include "content/public/common/content_switches.h"
#include "content/public/renderer/render_view.h"
#include "content/renderer/render_view_impl.h"
#include "ipc/ipc_descriptors.h"
#include "net/proxy/proxy_bypass_rules.h"
#include "third_party/node/src/node.h"
#undef CHECK
#include "third_party/node/src/node_internals.h"
#include "third_party/node/src/req_wrap.h"
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/public/web/WebFrame.h"
#include "third_party/WebKit/public/web/WebSecurityOrigin.h"
#include "third_party/WebKit/public/web/WebSecurityPolicy.h"
#include "third_party/WebKit/public/web/WebView.h"
//#include "third_party/WebKit/Source/WebKit/chromium/public/platform/WebString.h"
#include "content/common/dom_storage/dom_storage_map.h"

using content::RenderView;
using content::RenderViewImpl;
using autofill::AutofillAgent;
using autofill::PasswordAutofillAgent;
using net::ProxyBypassRules;
using WebKit::WebFrame;
using WebKit::WebView;
using WebKit::WebString;
using WebKit::WebSecurityPolicy;


namespace content {

namespace {

RenderView* GetCurrentRenderView() {
  WebFrame* frame = WebFrame::frameForCurrentContext();
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
  :creating_first_context_(true) {
}

ShellContentRendererClient::~ShellContentRendererClient() {
}

void ShellContentRendererClient::RenderThreadStarted() {
  // Change working directory.
  CommandLine* command_line = CommandLine::ForCurrentProcess();
  if (command_line->HasSwitch(switches::kWorkingDirectory)) {
    file_util::SetCurrentDirectory(
        command_line->GetSwitchValuePath(switches::kWorkingDirectory));
  }

  int argc = 1;
  char* argv[] = { const_cast<char*>("node"), NULL, NULL };
  std::string node_main;

  // Check if there is a 'node-main'.
  if (command_line->HasSwitch(switches::kNodeMain)) {
    argc++;
    node_main = command_line->GetSwitchValueASCII(switches::kNodeMain);
    argv[1] = const_cast<char*>(node_main.c_str());
  }

  // Initialize uv.
  node::SetupUv(argc, argv);

  std::string snapshot_path;
  if (command_line->HasSwitch(switches::kSnapshot)) {
    snapshot_path = command_line->GetSwitchValuePath(switches::kSnapshot).AsUTF8Unsafe();
  }

  if (command_line->HasSwitch(switches::kDomStorageQuota)) {
    std::string quota_str = command_line->GetSwitchValueASCII(switches::kDomStorageQuota);
    int quota = 0;
    if (base::StringToInt(quota_str, &quota) && quota > 0) {
      content::DOMStorageMap::SetQuotaOverride(quota * 1024 * 1024);
    }
  }
  // Initialize node after render thread is started.
  if (!snapshot_path.empty()) {
    v8::V8::Initialize(snapshot_path.c_str());
  }else
    v8::V8::Initialize();
  v8::HandleScope scope;

  // Install window bindings into node. The Window API is implemented in node's
  // context, so when a Shell changes to a new location and destroy previous
  // window context, our Window API can still work.
  window_bindings_.reset(new nwapi::WindowBindings());
  v8::RegisterExtension(window_bindings_.get());
  const char* names[] = { "window_bindings.js" };
  v8::ExtensionConfiguration extension_configuration(1, names);

  node::g_context.Reset(v8::Isolate::GetCurrent(),
                        v8::Context::New(v8::Isolate::GetCurrent(),
                                         &extension_configuration));
  node::g_context->SetSecurityToken(v8::String::NewSymbol("nw-token", 8));
  node::g_context->Enter();

  node::g_context->SetEmbedderData(0, v8::String::NewSymbol("node"));

  // Setup node.js.
  v8::Local<v8::Context> context =
    v8::Local<v8::Context>::New(node::g_context->GetIsolate(), node::g_context);

  node::SetupContext(argc, argv, context);

#if !defined(OS_WIN)
  v8::Local<v8::Script> script = v8::Script::New(v8::String::New((
      "process.__nwfds_to_close = [" +
      base::StringPrintf("%d", base::GlobalDescriptors::GetInstance()->Get(kPrimaryIPCChannel)) +
      "];"
    ).c_str()));
  CHECK(*script);
  script->Run();
#endif
  // Start observers.
  shell_observer_.reset(new ShellRenderProcessObserver());

  WebString file_scheme(ASCIIToUTF16("file"));
  WebString app_scheme(ASCIIToUTF16("app"));
  // file: resources should be allowed to receive CORS requests.
  WebSecurityPolicy::registerURLSchemeAsCORSEnabled(file_scheme);
  WebSecurityPolicy::registerURLSchemeAsCORSEnabled(app_scheme);

}

void ShellContentRendererClient::RenderViewCreated(RenderView* render_view) {
  new nwapi::Dispatcher(render_view);
  new nw::NwRenderViewObserver(render_view);
  new prerender::PrerendererClient(render_view);
#if defined(ENABLE_PRINTING)
  new printing::PrintWebViewHelper(render_view);
#endif

  nw::AutofillAgent* autofill_agent = new nw::AutofillAgent(render_view);

  // The PageClickTracker is a RenderViewObserver, and hence will be freed when
  // the RenderView is destroyed.
  new autofill::PageClickTracker(render_view, autofill_agent);

  // PasswordAutofillAgent* password_autofill_agent =
  //     new PasswordAutofillAgent(render_view);
  // new AutofillAgent(render_view, password_autofill_agent);
}

void ShellContentRendererClient::DidCreateScriptContext(
    WebKit::WebFrame* frame,
    v8::Handle<v8::Context> context,
    int extension_group,
    int world_id) {
  GURL url(frame->document().url());
  VLOG(1) << "DidCreateScriptContext: " << url;
  InstallNodeSymbols(frame, context, url);
  creating_first_context_ = false;
}

bool ShellContentRendererClient::goodForNode(WebKit::WebFrame* frame)
{
  RenderViewImpl* rv = RenderViewImpl::FromWebView(frame->view());
  GURL url(frame->document().url());
  ProxyBypassRules rules;
  rules.ParseFromString(rv->renderer_preferences_.nw_remote_page_rules);
  bool force_on = rules.Matches(url);
  bool is_nw_protocol = url.SchemeIs("nw") || !url.is_valid();
  bool use_node =
    CommandLine::ForCurrentProcess()->HasSwitch(switches::kNodejs) &&
    !frame->isNwDisabledChildFrame() &&
    (force_on || url.SchemeIsFile() || is_nw_protocol || url.SchemeIs("app"));
  return use_node;
}

bool ShellContentRendererClient::WillSetSecurityToken(
    WebKit::WebFrame* frame,
    v8::Handle<v8::Context> context) {
  GURL url(frame->document().url());
  VLOG(1) << "WillSetSecurityToken: " << url;
  if (goodForNode(frame)) {
    // Override context's security token
    context->SetSecurityToken(node::g_context->GetSecurityToken());
    frame->document().securityOrigin().grantUniversalAccess();

    int ret = 0;
    RenderViewImpl* rv = RenderViewImpl::FromWebView(frame->view());
    rv->Send(new ViewHostMsg_GrantUniversalPermissions(rv->GetRoutingID(), &ret));

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
    WebKit::WebFrame* frame,
    v8::Handle<v8::Context> context,
    const GURL& url) {
  v8::HandleScope handle_scope;

  static bool installed_once = false;

  v8::Local<v8::Object> nodeGlobal = node::g_context->Global();
  v8::Local<v8::Object> v8Global = context->Global();

  // Use WebKit's console globally
  nodeGlobal->Set(v8::String::New("console"),
                  v8Global->Get(v8::String::New("console")));

  // Do we integrate node?
  bool use_node = goodForNode(frame);

  // Test if protocol is 'nw:'
  // test for 'about:blank' is also here becuase window.open would
  // open 'about:blank' first // FIXME
  bool is_nw_protocol = url.SchemeIs("nw") || !url.is_valid();

  if (use_node || is_nw_protocol) {
    frame->setNodeJS(true);

    v8::Local<v8::Array> symbols = v8::Array::New(4);
    symbols->Set(0, v8::String::New("global"));
    symbols->Set(1, v8::String::New("process"));
    symbols->Set(2, v8::String::New("Buffer"));
    symbols->Set(3, v8::String::New("root"));

    for (unsigned i = 0; i < symbols->Length(); ++i) {
      v8::Local<v8::Value> key = symbols->Get(i);
      v8Global->Set(key, nodeGlobal->Get(key));
    }

    if (!installed_once) {
      installed_once = true;

      // The following listener on process should not be added each
      // time when a document is created, or it will leave the
      // reference to the closure created by the call back and leak
      // memory (see #203)

      nodeGlobal->Set(v8::String::New("window"), v8Global);

      // Listen uncaughtException with ReportException.
      v8::Local<v8::Function> cb = v8::FunctionTemplate::New(ReportException)->
        GetFunction();
      v8::Local<v8::Value> argv[] = { v8::String::New("uncaughtException"), cb };
      node::MakeCallback(node::g_env->process_object(), "on", 2, argv);
    }
  }

  if (use_node) {
    RenderViewImpl* rv = RenderViewImpl::FromWebView(frame->view());
    std::string root_path = rv->renderer_preferences_.nw_app_root_path.AsUTF8Unsafe();
#if defined(OS_WIN)
    ReplaceChars(root_path, "\\", "\\\\", &root_path);
#endif
    ReplaceChars(root_path, "'", "\\'", &root_path);
    v8::Local<v8::Script> script = v8::Script::New(v8::String::New((
        // Make node's relative modules work
        "if (!process.mainModule.filename) {"
        "  var root = '" + root_path + "';"
#if defined(OS_WIN)
        "process.mainModule.filename = decodeURIComponent(window.location.pathname.substr(1));"
#else
        "process.mainModule.filename = decodeURIComponent(window.location.pathname);"
#endif
        "if (window.location.href.indexOf('app://') === 0) {process.mainModule.filename = root + '/' + process.mainModule.filename}"
        "process.mainModule.paths = global.require('module')._nodeModulePaths(process.cwd());"
        "process.mainModule.loaded = true;"
        "}").c_str()
    ));
    CHECK(*script);
    script->Run();
  }

  if (use_node || is_nw_protocol) {
    v8::Local<v8::Script> script = v8::Script::New(v8::String::New(
        // Overload require
        "window.require = function(name) {"
        "  if (name == 'nw.gui')"
        "    return nwDispatcher.requireNwGui();"
        "  return global.require(name);"
        "};"

        // Save node-webkit version
        "process.versions['node-webkit'] = '" NW_VERSION_STRING "';"
        "process.versions['chromium'] = '" CHROME_VERSION "';"
    ));
    script->Run();
    v8::Local<v8::Script> script2 = v8::Script::New(v8::String::New(
        "  nwDispatcher.requireNwGui().Window.get();"
    ));
    script2->Run();
  } else {
    int ret;
    RenderViewImpl* render_view = RenderViewImpl::FromWebView(frame->view());

    if (frame->parent() == NULL && creating_first_context_) {
      // do this only for top frames, or initialization of iframe
      // could override parent settings here
      render_view->Send(new ShellViewHostMsg_SetForceClose(
            render_view->GetRoutingID(), true, &ret));
    }
  }
}

// static
void ShellContentRendererClient::ReportException(
            const v8::FunctionCallbackInfo<v8::Value>&  args) {
  v8::HandleScope handle_scope;

  // Do nothing if user is listening to uncaughtException.
  v8::Local<v8::Value> listeners_v =
    node::g_env->process_object()->Get(v8::String::New("listeners"));
  v8::Local<v8::Function> listeners =
      v8::Local<v8::Function>::Cast(listeners_v);

  v8::Local<v8::Value> argv[1] = { v8::String::New("uncaughtException") };
  v8::Local<v8::Value> ret = listeners->Call(node::g_env->process_object(), 1, argv);
  v8::Local<v8::Array> listener_array = v8::Local<v8::Array>::Cast(ret);

  uint32_t length = listener_array->Length();
  if (length > 1) {
    args.GetReturnValue().Set(v8::Undefined());
    return;
  }

  // Print stacktrace.
  v8::Local<v8::String> stack_symbol = v8::String::New("stack");
  std::string error;

  v8::Local<v8::Object> exception = args[0]->ToObject();
  if (exception->Has(stack_symbol))
    error = *v8::String::Utf8Value(exception->Get(stack_symbol));
  else
    error = *v8::String::Utf8Value(exception);

  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    args.GetReturnValue().Set(v8::Undefined());
    return;
  }

  render_view->Send(new ShellViewHostMsg_UncaughtException(
      render_view->GetRoutingID(),
      error));

  args.GetReturnValue().Set(v8::Undefined());
}

void ShellContentRendererClient::UninstallNodeSymbols(
    WebKit::WebFrame* frame,
    v8::Handle<v8::Context> context) {
  v8::HandleScope handle_scope;

  v8::Local<v8::Object> v8Global = context->Global();
  v8::Local<v8::Array> symbols = v8::Array::New(5);
  symbols->Set(0, v8::String::New("global"));
  symbols->Set(1, v8::String::New("process"));
  symbols->Set(2, v8::String::New("Buffer"));
  symbols->Set(3, v8::String::New("root"));
  symbols->Set(4, v8::String::New("require"));

  for (unsigned i = 0; i < symbols->Length(); ++i) {
    v8::Local<v8::String> key = symbols->Get(i)->ToString();
    if(v8Global->Has(key))
      v8Global->Delete(key);
  }
}

void ShellContentRendererClient::willHandleNavigationPolicy(
    RenderView* rv,
    WebKit::WebFrame* frame,
    const WebKit::WebURLRequest& request,
    WebKit::WebNavigationPolicy* policy) {

  nwapi::Dispatcher::willHandleNavigationPolicy(rv, frame, request, policy);
}

}  // namespace content
