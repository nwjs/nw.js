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

#include "content/nw/src/renderer/shell_render_process_observer.h"

#include "base/file_util.h"
#include "content/public/renderer/render_thread.h"
#include "content/nw/src/api/api_messages.h"
#include "content/nw/src/api/dispatcher_bindings.h"
#include "webkit/glue/webkit_glue.h"
#include "webkit/support/gc_extension.h"
#include "third_party/node/src/node.h"
#include "third_party/node/src/req_wrap.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebRuntimeFeatures.h"
#include "v8/include/v8.h"

using WebKit::WebRuntimeFeatures;

namespace content {

ShellRenderProcessObserver::ShellRenderProcessObserver() {
  RenderThread::Get()->AddObserver(this);
}

ShellRenderProcessObserver::~ShellRenderProcessObserver() {
}

bool ShellRenderProcessObserver::OnControlMessageReceived(
    const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(ShellRenderProcessObserver, message)
    IPC_MESSAGE_HANDLER(ShellViewMsg_Open, OnOpen)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()

  return handled;
}

void ShellRenderProcessObserver::OnRenderProcessWillShutdown() {
  // process.emit('exit');
  node::EmitExit(node::process);
  node::RunAtExit();
}

void ShellRenderProcessObserver::WebKitInitialized() {
  RenderThread::Get()->RegisterExtension(new api::DispatcherBindings());
  WebRuntimeFeatures::enableCSSRegions(true);
}

void ShellRenderProcessObserver::OnOpen(const std::string& path) {
  v8::HandleScope handle_scope;

  // the App object is stored in process["_nw_app"].
  v8::Local<v8::Object> process = node::g_context->Global()->Get(
      node::process_symbol)->ToObject();
  v8::Local<v8::String> app_symbol = v8::String::NewSymbol("_nw_app");
  if (process->Has(app_symbol)) {
    // process["_nw_app"].emit(path).
    v8::Local<v8::Object> app = process->Get(app_symbol)->ToObject();
    v8::Local<v8::Function> emit = v8::Local<v8::Function>::Cast(
        app->Get(v8::String::New("emit")));
    v8::Local<v8::Value> argv[] = {
        v8::String::New("open"), v8::String::New(path.c_str())
    };
    emit->Call(app, 2, argv);
  }
}

}  // namespace content
