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
#include "base/file_path.h"
#include "base/file_util.h"
#include "base/logging.h"
#include "base/utf_string_conversions.h"
#include "content/nw/src/api/dispatcher.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_version.h"
#include "content/nw/src/renderer/prerenderer/prerenderer_client.h"
#include "content/nw/src/renderer/shell_render_process_observer.h"
#include "content/nw/src/renderer/shell_render_view_observer.h"
#include "third_party/node/src/node.h"
#include "third_party/node/src/req_wrap.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebSecurityOrigin.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebSecurityPolicy.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/platform/WebString.h"
#include "v8/include/v8.h"

namespace content {

ShellContentRendererClient::ShellContentRendererClient() {
}

ShellContentRendererClient::~ShellContentRendererClient() {
}

void ShellContentRendererClient::RenderThreadStarted() {
  // Change working directory
  CommandLine* command_line = CommandLine::ForCurrentProcess();
  if (command_line->HasSwitch(switches::kWorkingDirectory)) {
    file_util::SetCurrentDirectory(
        command_line->GetSwitchValuePath(switches::kWorkingDirectory));
  }

  // Initialize node after render thread is started
  v8::V8::Initialize();
  v8::HandleScope scope;

  node::g_context = v8::Context::New();
  node::g_context->Enter();

  char* argv[] = { (char*)"node", NULL };
  node::SetupContext(1, argv, node::g_context->Global());

  // Start observers
  shell_observer_.reset(new ShellRenderProcessObserver());
  api_dispatcher_.reset(new api::Dispatcher());
}

void ShellContentRendererClient::RenderViewCreated(RenderView* render_view) {
  new prerender::PrerendererClient(render_view);
  new ShellRenderViewObserver(render_view);
}

void ShellContentRendererClient::DidCreateScriptContext(
    WebKit::WebFrame* frame,
    v8::Handle<v8::Context> context,
    int extension_group,
    int world_id) {
  InstallNodeSymbols(context);

  api_dispatcher_->DidCreateScriptContext(
      frame, context, extension_group, world_id);
}

void ShellContentRendererClient::WillReleaseScriptContext(
    WebKit::WebFrame* frame,
    v8::Handle<v8::Context> context,
    int world_id) {
  api_dispatcher_->WillReleaseScriptContext(frame, context, world_id);
}

bool ShellContentRendererClient::WillSetSecurityToken(
    WebKit::WebFrame* frame,
    v8::Handle<v8::Context> context) {
  // Override context's security token
  context->SetSecurityToken(node::g_context->GetSecurityToken());

  return true;
}

void ShellContentRendererClient::InstallNodeSymbols(
    v8::Handle<v8::Context> context) {
  // Do we integrate node?
  bool use_node =
      CommandLine::ForCurrentProcess()->HasSwitch(switches::kmNodejs);

  // Test if protocol is file:
  v8::Local<v8::Script> protocol_script = v8::Script::New(v8::String::New(
      "(function(){ return window.location.protocol == 'file:' })();"
  ));
  bool is_file_protocol = protocol_script->Run()->BooleanValue();

  // Disable nodejs in no-file protocols
  use_node = is_file_protocol ? use_node : false;

  // Test if protocol is 'nw:'
  // test for 'about:blank' is also here becuase window.open would open 'about:blank' first
  protocol_script = v8::Script::New(v8::String::New(
      "(function(){ return window.location.protocol == 'nw:' || window.location == 'about:blank'; })();"
  ));
  bool is_nw_protocol = protocol_script->Run()->BooleanValue();

  if (use_node || is_nw_protocol) {
    v8::Local<v8::Array> symbols = v8::Array::New(5);
    symbols->Set(0, v8::String::New("require"));
    symbols->Set(1, v8::String::New("global"));
    symbols->Set(2, v8::String::New("process"));
    symbols->Set(3, v8::String::New("Buffer"));
    symbols->Set(4, v8::String::New("root"));

    v8::Local<v8::Object> nodeGlobal = node::g_context->Global();
    v8::Local<v8::Object> v8Global = context->Global();
    for (unsigned i = 0; i < symbols->Length(); ++i) {
      v8::Local<v8::Value> key = symbols->Get(i);
      v8Global->Set(key, nodeGlobal->Get(key));
    }
  }

  if (use_node) {
    v8::Local<v8::Script> script = v8::Script::New(v8::String::New(
        // Make node's relative modules work
#if defined(OS_WIN)
        "process.mainModule.filename = decodeURIComponent(window.location.pathname.substr(1));"
#else
        "process.mainModule.filename = decodeURIComponent(window.location.pathname);"
#endif
        "process.mainModule.paths = require('module')._nodeModulePaths(process.cwd());"
    ));
    script->Run();
  }

  if (use_node || is_nw_protocol) {
    v8::Local<v8::Script> script = v8::Script::New(v8::String::New(
        // Use WebKit's console globally
        "global.console = console;"

        // Don't exit on exception
        "process.on('uncaughtException', function (err) {"
          "console.log(err.stack);"
        "});"

        // Save node-webkit version
        "process.versions['node-webkit'] = '" NW_VERSION_STRING "';"
    ));
    script->Run();
  }
}

}  // namespace content
