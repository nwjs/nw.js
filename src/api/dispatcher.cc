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
#include "third_party/node/src/node.h"
#include "third_party/node/src/req_wrap.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebDocument.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebFrame.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebView.h"
#include "v8/include/v8.h"

namespace api {

Dispatcher::Dispatcher(content::RenderView* render_view)
    : content::RenderViewObserver(render_view) {
}

Dispatcher::~Dispatcher() {
}

bool Dispatcher::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(Dispatcher, message)
    IPC_MESSAGE_HANDLER(ShellViewMsg_Object_On_Event, OnEvent)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()

  return handled;
}

void Dispatcher::DraggableRegionsChanged(WebKit::WebFrame* frame) {
  WebKit::WebVector<WebKit::WebDraggableRegion> webregions =
      frame->document().draggableRegions();
  std::vector<extensions::DraggableRegion> regions;
  for (size_t i = 0; i < webregions.size(); ++i) {
    extensions::DraggableRegion region;
    region.bounds = webregions[i].bounds;
    region.draggable = webregions[i].draggable;
    regions.push_back(region);
  }
  Send(new ShellViewHostMsg_UpdateDraggableRegions(routing_id(), regions));
}

void Dispatcher::OnEvent(int object_id,
                         std::string event,
                         const base::ListValue& arguments) {
  v8::HandleScope scope;
  WebKit::WebView* web_view = render_view()->GetWebView();
  if (web_view == NULL)
    return;

  content::V8ValueConverterImpl converter;
  v8::Handle<v8::Value> args = converter.ToV8Value(&arguments, node::g_context);
  DCHECK(!args.IsEmpty()) << "Invalid 'arguments' in Dispatcher::OnEvent";
  v8::Handle<v8::Value> argv[] = {
      v8::Integer::New(object_id), v8::String::New(event.c_str()), args };

  // __nwObjectsRegistry.handleEvent(object_id, event, arguments);
  v8::Handle<v8::Object> objects_registry = 
      node::g_context->Global()->Get(v8::String::New("__nwObjectsRegistry"))->
          ToObject();
  node::MakeCallback(objects_registry, "handleEvent", 3, argv);
}

}  // namespace api
