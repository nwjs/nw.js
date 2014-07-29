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

#include "content/nw/src/api/dispatcher.h"

#include "content/nw/src/api/api_messages.h"
#include "content/public/renderer/render_view.h"
#include "content/renderer/v8_value_converter_impl.h"
#include "third_party/node/src/node.h"
#undef CHECK
#include "third_party/node/src/req_wrap.h"
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/public/web/WebFrame.h"
#include "third_party/WebKit/public/web/WebView.h"
#include "v8/include/v8.h"

#undef LOG
#undef ASSERT
#undef FROM_HERE

#if defined(OS_WIN)
#define _USE_MATH_DEFINES
#include <math.h>
#endif
#include "third_party/WebKit/Source/config.h"
#include "third_party/WebKit/Source/core/frame/Frame.h"
#include "third_party/WebKit/Source/web/WebFrameImpl.h"
#include "V8HTMLElement.h"

namespace nwapi {

static inline v8::Local<v8::String> v8_str(const char* x) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  return v8::String::NewFromUtf8(isolate, x);
}

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

void Dispatcher::DraggableRegionsChanged(blink::WebFrame* frame) {
  blink::WebVector<blink::WebDraggableRegion> webregions =
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
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);
  blink::WebView* web_view = render_view()->GetWebView();
  if (web_view == NULL)
    return;

  DVLOG(1) << "Dispatcher::OnEvent(object_id=" << object_id << ", event=\"" << event << "\")";

  content::V8ValueConverterImpl converter;
  v8::Local<v8::Context> context =
    v8::Local<v8::Context>::New(isolate, node::g_context);

  v8::Handle<v8::Value> args = converter.ToV8Value(&arguments, context);
  DCHECK(!args.IsEmpty()) << "Invalid 'arguments' in Dispatcher::OnEvent";
  v8::Handle<v8::Value> argv[] = {
    v8::Integer::New(isolate, object_id), v8_str(event.c_str()), args };

  // __nwObjectsRegistry.handleEvent(object_id, event, arguments);
  v8::Handle<v8::Value> val =
    context->Global()->Get(v8_str("__nwObjectsRegistry"));
  if (val->IsNull() || val->IsUndefined())
    return; // need to find out why it's undefined here in debugger
  v8::Handle<v8::Object> objects_registry = val->ToObject();
  DVLOG(1) << "handleEvent(object_id=" << object_id << ", event=\"" << event << "\")";
  node::MakeCallback(isolate, objects_registry, "handleEvent", 3, argv);
}

v8::Handle<v8::Object> Dispatcher::GetObjectRegistry() {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::Local<v8::Context> context =
    v8::Local<v8::Context>::New(isolate, node::g_context);
  // need to enter node context to access the registry in
  // some cases, e.g. normal frame in #1519
  context->Enter();
  v8::Handle<v8::Value> registry =
    context->Global()->Get(v8_str("__nwObjectsRegistry"));
  context->Exit();
  ASSERT(!(registry->IsNull() || registry->IsUndefined()));
  // if (registry->IsNull() || registry->IsUndefined())
  //   return v8::Undefined();
  return registry->ToObject();
}

v8::Handle<v8::Value> Dispatcher::GetWindowId(blink::WebFrame* frame) {
  v8::Handle<v8::Value> v8win = frame->mainWorldScriptContext()->Global();
  v8::Handle<v8::Value> val = v8win->ToObject()->Get(v8_str("__nwWindowId"));

  return val;
}

void Dispatcher::ZoomLevelChanged() {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope scope(isolate);

  blink::WebView* web_view = render_view()->GetWebView();
  float zoom_level = web_view->zoomLevel();

  v8::Handle<v8::Value> val = GetWindowId(web_view->mainFrame());

  if (val.IsEmpty())
    return;
  if (val->IsNull() || val->IsUndefined())
    return;

  v8::Handle<v8::Object> objects_registry = GetObjectRegistry();
  if (objects_registry->IsUndefined())
    return;

  v8::Local<v8::Array> args = v8::Array::New(isolate);
  args->Set(0, v8::Number::New(isolate, zoom_level));
  v8::Handle<v8::Value> argv[] = {val, v8_str("zoom"), args };

  node::MakeCallback(isolate, objects_registry, "handleEvent", 3, argv);
}

void Dispatcher::DidCreateDocumentElement(blink::WebFrame* frame) {
  documentCallback("document-start", frame);
}

void Dispatcher::DidFinishDocumentLoad(blink::WebFrame* frame) {
  documentCallback("document-end", frame);
}

void Dispatcher::documentCallback(const char* ev, blink::WebFrame* frame) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  blink::WebView* web_view = render_view()->GetWebView();
  v8::HandleScope scope(isolate);

  if (!web_view)
    return;
  v8::Context::Scope cscope (web_view->mainFrame()->mainWorldScriptContext());

  v8::Handle<v8::Value> val = GetWindowId(web_view->mainFrame());
  if (val.IsEmpty())
    return;
  if (val->IsNull() || val->IsUndefined())
    return;

  v8::Handle<v8::Object> objects_registry = GetObjectRegistry();
  if (objects_registry->IsUndefined())
    return;

  v8::Local<v8::Array> args = v8::Array::New(isolate);
  v8::Handle<v8::Value> element = v8::Null(isolate);
  WebCore::LocalFrame* core_frame = blink::toWebFrameImpl(frame)->frame();
  if (core_frame->ownerElement()) {
    element = WebCore::toV8((WebCore::HTMLElement*)core_frame->ownerElement(),
                            frame->mainWorldScriptContext()->Global(),
                            frame->mainWorldScriptContext()->GetIsolate());
  }
  args->Set(0, element);
  v8::Handle<v8::Value> argv[] = {val, v8_str(ev), args };

  node::MakeCallback(isolate, objects_registry, "handleEvent", 3, argv);
}

void Dispatcher::willHandleNavigationPolicy(
    content::RenderView* rv,
    blink::WebFrame* frame,
    const blink::WebURLRequest& request,
    blink::WebNavigationPolicy* policy) {

  blink::WebView* web_view = rv->GetWebView();

  if (!web_view)
    return;

  v8::Isolate* isolate = v8::Isolate::GetCurrent();

  v8::Handle<v8::Value> id_val;
  if (web_view->mainFrame() && !web_view->mainFrame()->mainWorldScriptContext().IsEmpty())
    id_val = nwapi::Dispatcher::GetWindowId(web_view->mainFrame());
  if (id_val.IsEmpty())
    return;
  if (id_val->IsUndefined() || id_val->IsNull())
    return;

  v8::Handle<v8::Object> objects_registry = nwapi::Dispatcher::GetObjectRegistry();
  if (objects_registry->IsUndefined())
    return;

  v8::Context::Scope cscope (web_view->mainFrame()->mainWorldScriptContext());

  v8::Local<v8::Array> args = v8::Array::New(isolate);
  v8::Handle<v8::Value> element = v8::Null(isolate);
  v8::Handle<v8::Object> policy_obj = v8::Object::New(isolate);

  WebCore::LocalFrame* core_frame = blink::toWebFrameImpl(frame)->frame();
  if (core_frame->ownerElement()) {
    element = WebCore::toV8((WebCore::HTMLElement*)core_frame->ownerElement(),
                            frame->mainWorldScriptContext()->Global(),
                            frame->mainWorldScriptContext()->GetIsolate());
  }
  args->Set(0, element);
  args->Set(1, v8_str(request.url().string().utf8().c_str()));
  args->Set(2, policy_obj);

  v8::Handle<v8::Value> argv[] = {id_val, v8_str("new-win-policy"), args };

  node::MakeCallback(isolate, objects_registry, "handleEvent", 3, argv);
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

}  // namespace nwapi
