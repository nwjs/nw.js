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

#include "content/nw/src/api/dispatcher.h"

#include "content/nw/src/api/api_messages.h"
#include "content/public/renderer/render_view.h"
#include "content/renderer/v8_value_converter_impl.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebFrame.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebView.h"
#include "v8/include/v8.h"

namespace api {

Dispatcher::Dispatcher(content::RenderView* render_view)
    : content::RenderViewObserver(render_view) {
}

Dispatcher::~Dispatcher() {
}

void Dispatcher::DidClearWindowObject(WebKit::WebFrame* frame) {
}

bool Dispatcher::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(Dispatcher, message)
    IPC_MESSAGE_HANDLER(ShellViewMsg_Object_On_Event, OnEvent)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()

  return handled;
}


void Dispatcher::OnEvent(int object_id,
                         std::string event,
                         const base::ListValue& arguments) {
  v8::HandleScope scope;
  WebKit::WebView* web_view = render_view()->GetWebView();
  if (web_view == NULL)
    return;

  WebKit::WebFrame* web_frame = web_view->mainFrame();
  v8::Handle<v8::Context> context = web_frame->mainWorldScriptContext();
  v8::Context::Scope context_scope(context);

  V8ValueConverterImpl converter;
  v8::Handle<v8::Value> args = converter.ToV8Value(&arguments, context);
  DCHECK(!args.IsEmpty()) << "Invalid 'arguments' in Dispatcher::OnEvent";
  v8::Handle<v8::Value> argv[] = {
      v8::Integer::New(object_id), v8::String::New(event.c_str()), args };

  // nwDispatcher.handleEvent(object_id, event, arguments);
  v8::Handle<v8::Object> nwDispatcher = 
      context->Global()->Get(v8::String::New("nwDispatcher"))->ToObject();
  DCHECK(!nwDispatcher.IsEmpty()) << "Cannot get nwDispatcher from window";
  v8::Handle<v8::Function> handleEvent = v8::Handle<v8::Function>::Cast(
      nwDispatcher->Get(v8::String::New("handleEvent")));
  handleEvent->Call(nwDispatcher, 3, argv);
}

}  // namespace api
