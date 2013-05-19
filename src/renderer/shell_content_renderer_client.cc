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

#include "content/nw/src/renderer/shell_content_renderer_client.h"

#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/file_util.h"
#include "base/logging.h"
#include "base/string16.h"
#include "base/utf_string_conversions.h"
#include "components/autofill/renderer/page_click_tracker.h"
#include "content/nw/src/api/dispatcher.h"
#include "content/nw/src/api/api_messages.h"
#include "content/nw/src/api/window_bindings.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_version.h"
#include "components/autofill/renderer/autofill_agent.h"
#include "components/autofill/renderer/password_autofill_agent.h"
#include "content/nw/src/renderer/nw_render_view_observer.h"
#include "content/nw/src/renderer/prerenderer/prerenderer_client.h"
#include "content/nw/src/renderer/printing/print_web_view_helper.h"
#include "content/nw/src/renderer/shell_render_process_observer.h"
#include "content/public/common/content_switches.h"
#include "content/public/renderer/render_view.h"
#include "content/renderer/render_view_impl.h"
#include "net/proxy/proxy_bypass_rules.h"
#include "third_party/node/src/node.h"
#include "third_party/node/src/req_wrap.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebDocument.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebFrame.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebSecurityOrigin.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebSecurityPolicy.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebView.h"
//#include "third_party/WebKit/Source/WebKit/chromium/public/platform/WebString.h"

using content::RenderView;
using content::RenderViewImpl;
using autofill::AutofillAgent;
using autofill::PasswordAutofillAgent;
using net::ProxyBypassRules;
using WebKit::WebFrame;
using WebKit::WebView;

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

ShellContentRendererClient::ShellContentRendererClient() {
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
  // Initialize node after render thread is started.
  if (!snapshot_path.empty()) {
    v8::V8::Initialize(snapshot_path.c_str());
  }else
    v8::V8::Initialize();
  v8::HandleScope scope;

  // Install window bindings into node. The Window API is implemented in node's
  // context, so when a Shell changes to a new location and destroy previous
  // window context, our Window API can still work.
  window_bindings_.reset(new api::WindowBindings());
  v8::RegisterExtension(window_bindings_.get());
  const char* names[] = { "window_bindings.js" };
  v8::ExtensionConfiguration extension_configuration(1, names);

  node::g_context = v8::Context::New(&extension_configuration);
  node::g_context->SetSecurityToken(v8::String::NewSymbol("nw-token", 8));
  node::g_context->Enter();

  // Setup node.js.
  node::SetupContext(argc, argv, node::g_context->Global());

  // Start observers.
  shell_observer_.reset(new ShellRenderProcessObserver());
}

void ShellContentRendererClient::RenderViewCreated(RenderView* render_view) {
  new api::Dispatcher(render_view);
  new nw::NwRenderViewObserver(render_view);
  new prerender::PrerendererClient(render_view);
#if defined(ENABLE_PRINTING)
  new printing::PrintWebViewHelper(render_view);
#endif

  // PageClickTracker* page_click_tracker = new PageClickTracker(render_view);
  PasswordAutofillAgent* password_autofill_agent =
      new PasswordAutofillAgent(render_view);
  new AutofillAgent(render_view, password_autofill_agent);
  //page_click_tracker->AddListener(autofill_agent);
}

void ShellContentRendererClient::DidCreateScriptContext(
    WebKit::WebFrame* frame,
    v8::Handle<v8::Context> context,
    int extension_group,
    int world_id) {
  GURL url(frame->document().url());
  VLOG(1) << "DidCreateScriptContext: " << url;
  InstallNodeSymbols(frame, context, url);
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
    (force_on || url.SchemeIsFile() || is_nw_protocol);
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
      node::MakeCallback(node::process, "on", 2, argv);
    }
  }

  if (use_node) {
    v8::Local<v8::Script> script = v8::Script::New(v8::String::New(
        // Make node's relative modules work
        "if (!process.mainModule.filename) {"
#if defined(OS_WIN)
        "process.mainModule.filename = decodeURIComponent(window.location.pathname.substr(1));"
#else
        "process.mainModule.filename = decodeURIComponent(window.location.pathname);"
#endif
        "process.mainModule.paths = global.require('module')._nodeModulePaths(process.cwd());"
        "process.mainModule.loaded = true;"
        "}"
    ));
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
    ));
    script->Run();
  }
}

// static
v8::Handle<v8::Value> ShellContentRendererClient::ReportException(
    const v8::Arguments& args) {
  v8::HandleScope handle_scope;

  // Do nothing if user is listening to uncaughtException.
  v8::Local<v8::Value> listeners_v =
      node::process->Get(v8::String::New("listeners"));
  v8::Local<v8::Function> listeners = 
      v8::Local<v8::Function>::Cast(listeners_v);

  v8::Local<v8::Value> argv[1] = { v8::String::New("uncaughtException") };
  v8::Local<v8::Value> ret = listeners->Call(node::process, 1, argv);
  v8::Local<v8::Array> listener_array = v8::Local<v8::Array>::Cast(ret);

  uint32_t length = listener_array->Length();
  if (length > 1)
    return v8::Undefined(); 

  // Print stacktrace.
  v8::Local<v8::String> stack_symbol = v8::String::New("stack");
  std::string error;

  v8::Local<v8::Object> exception = args[0]->ToObject();
  if (exception->Has(stack_symbol))
    error = *v8::String::Utf8Value(exception->Get(stack_symbol));
  else
    error = *v8::String::Utf8Value(exception);

  RenderView* render_view = GetCurrentRenderView();
  if (!render_view)
    return v8::Undefined(); 

  render_view->Send(new ShellViewHostMsg_UncaughtException(
      render_view->GetRoutingID(),
      error));

  return v8::Undefined(); 
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

}  // namespace content
