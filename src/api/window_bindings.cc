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


#include "content/nw/src/api/window_bindings.h"

#include "base/values.h"
#include "content/child/child_thread.h"
#include "content/nw/src/api/bindings_common.h"
#include "content/nw/src/api/dispatcher.h"
#include "content/renderer/render_view_impl.h"
#include "grit/nw_resources.h"
#undef LOG
using namespace blink;
#if defined(OS_WIN)
#define _USE_MATH_DEFINES
#include <math.h>
#endif

#undef FROM_HERE

#include "third_party/WebKit/Source/config.h"
#include "third_party/WebKit/Source/core/html/HTMLIFrameElement.h"
#include "third_party/WebKit/Source/core/dom/Document.h"
#include "third_party/WebKit/Source/core/frame/LocalFrame.h"
#include "third_party/WebKit/public/web/WebFrame.h"
#include "third_party/WebKit/public/web/WebView.h"
#include "third_party/WebKit/Source/web/WebLocalFrameImpl.h"
#include "third_party/WebKit/public/web/WebScriptSource.h"

#undef BLINK_IMPLEMENTATION
#define BLINK_IMPLEMENTATION 1
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/Source/platform/heap/Handle.h"
#include "third_party/WebKit/Source/core/inspector/InspectorInstrumentation.h"
#include "third_party/WebKit/Source/core/inspector/InspectorResourceAgent.h"

#undef CHECK
#include "V8HTMLIFrameElement.h"

using blink::WebScriptSource;
using blink::WebFrame;
using blink::InstrumentingAgents;
using blink::InspectorResourceAgent;

namespace nwapi {

WindowBindings::WindowBindings()
    : v8::Extension("window_bindings.js",
                    GetStringResource(
                        IDR_NW_API_WINDOW_BINDINGS_JS).data(),
                    0,     // num dependencies.
                    NULL,  // dependencies array.
                    GetStringResource(
                        IDR_NW_API_WINDOW_BINDINGS_JS).size()) {
}

WindowBindings::~WindowBindings() {
}

v8::Handle<v8::FunctionTemplate>
WindowBindings::GetNativeFunctionTemplate(
                                  v8::Isolate* isolate,
                                  v8::Handle<v8::String> name) {
  if (name->Equals(v8::String::NewFromUtf8(isolate, "BindToShell")))
    return v8::FunctionTemplate::New(isolate, BindToShell);
  else if (name->Equals(v8::String::NewFromUtf8(isolate, "CallObjectMethod")))
    return v8::FunctionTemplate::New(isolate, CallObjectMethod);
  else if (name->Equals(v8::String::NewFromUtf8(isolate, "CallObjectMethodSync")))
    return v8::FunctionTemplate::New(isolate, CallObjectMethodSync);
  else if (name->Equals(v8::String::NewFromUtf8(isolate, "GetWindowObject")))
    return v8::FunctionTemplate::New(isolate, GetWindowObject);
  else if (name->Equals(v8::String::NewFromUtf8(isolate, "AllocateId")))
    return v8::FunctionTemplate::New(isolate, AllocateId);

  return v8::FunctionTemplate::New(isolate);
}

// static
void
WindowBindings::BindToShell(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  int routing_id = args[0]->Int32Value();
  int object_id = args[1]->Int32Value();

  remote::AllocateObject(routing_id, object_id, "Window", v8::Object::New(isolate));

  args.GetReturnValue().Set(v8::Undefined(isolate));
}

void
WindowBindings::AllocateId(const v8::FunctionCallbackInfo<v8::Value>& args) {
  content::RenderViewImpl* render_view = static_cast<content::RenderViewImpl*>(GetEnteredRenderView());
  int routing_id = render_view->GetRoutingID();

  args.GetReturnValue().Set(remote::AllocateId(routing_id));
}

// static
void
WindowBindings::CallObjectMethod(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::Local<v8::Object> self = args[0]->ToObject();
  int routing_id = self->Get(v8::String::NewFromUtf8(isolate, "routing_id"))->Int32Value();
  int object_id = self->Get(v8::String::NewFromUtf8(isolate, "id"))->Int32Value();
  std::string method = *v8::String::Utf8Value(args[1]);
  content::RenderViewImpl* render_view = static_cast<content::RenderViewImpl*>(
                                                                                 content::RenderViewImpl::FromRoutingID(routing_id));
  if (!render_view)
    render_view = static_cast<content::RenderViewImpl*>(GetEnteredRenderView());

  if (!render_view) {
    std::string msg = "Unable to get render view in " + method;
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, msg.c_str()))));
    return;
  }

  WebFrame* main_frame = render_view->GetWebView()->mainFrame();
  if (method == "EvaluateScript") {
    v8::Handle<v8::Value> result;
    v8::Handle<v8::Object> frm = v8::Handle<v8::Object>::Cast(args[2]);
    WebFrame* web_frame = NULL;
    if (frm->IsNull()) {
      web_frame = main_frame;
    }else{
      blink::HTMLIFrameElement* iframe = blink::V8HTMLIFrameElement::toNative(frm);
      web_frame = blink::WebFrame::fromFrame(iframe->contentFrame());
    }
#if defined(OS_WIN)
    base::string16 jscript((WCHAR*)*v8::String::Value(args[3]));
#else
    base::string16 jscript = *v8::String::Value(args[3]);
#endif
    if (web_frame) {
      result = web_frame->executeScriptAndReturnValue(WebScriptSource(jscript));
    }
    args.GetReturnValue().Set(result);
    return;
  } else if (method == "setDevToolsJail") {
    v8::Handle<v8::Object> frm = v8::Handle<v8::Object>::Cast(args[2]);
    if (frm->IsNull()) {
      main_frame->setDevtoolsJail(NULL);
    }else{
      blink::HTMLIFrameElement* iframe = blink::V8HTMLIFrameElement::toNative(frm);
      main_frame->setDevtoolsJail(blink::WebFrame::fromFrame(iframe->contentFrame()));
    }
    args.GetReturnValue().Set(v8::Undefined(isolate));
    return;
  } else if (method == "setCacheDisabled") {
    RefPtrWillBePersistent<blink::Document> document = static_cast<PassRefPtrWillBeRawPtr<blink::Document> >(main_frame->document());
    InstrumentingAgents* instrumentingAgents = instrumentationForPage(document->page());
    if (instrumentingAgents) {
      bool disable = args[2]->ToBoolean()->Value();
      InspectorResourceAgent* resAgent = instrumentingAgents->inspectorResourceAgent();
      resAgent->setCacheDisabled(NULL, disable);
      args.GetReturnValue().Set(true);
    } else
      args.GetReturnValue().Set(false);
    return;
  }

  args.GetReturnValue().Set(remote::CallObjectMethod(render_view->GetRoutingID(),
                                                     object_id,
                                                     "Window", method, args[2]));
}

// static
void
WindowBindings::CallObjectMethodSync(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::EscapableHandleScope scope(isolate);

  v8::Local<v8::Object> self = args[0]->ToObject();
  int routing_id = self->Get(v8::String::NewFromUtf8(isolate, "routing_id"))->Int32Value();
  int object_id = self->Get(v8::String::NewFromUtf8(isolate, "id"))->Int32Value();
  std::string method = *v8::String::Utf8Value(args[1]);
  content::RenderViewImpl* render_view = static_cast<content::RenderViewImpl*>(
                                                                                 content::RenderViewImpl::FromRoutingID(routing_id));
  if (!render_view) {
    std::string msg = "Unable to get render view in " + method;
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, msg.c_str()))));
    return;
  }

  if (method == "GetZoomLevel") {
    float zoom_level = render_view->GetWebView()->zoomLevel();

    v8::Local<v8::Array> array = v8::Array::New(isolate);
    array->Set(0, v8::Number::New(isolate, zoom_level));
    args.GetReturnValue().Set(scope.Escape(array));
    return;
  }else if (method == "SetZoomLevel") {
    double zoom_level = args[2]->ToNumber()->Value();
    render_view->GetWebView()->setZoomLevel(zoom_level);
    nwapi::Dispatcher::ZoomLevelChanged(render_view->GetWebView());
    args.GetReturnValue().Set(v8::Undefined(isolate));
    return;
  }
  args.GetReturnValue().Set(remote::CallObjectMethodSync(routing_id, object_id, "Window", method, args[2]));
}

// static
void
WindowBindings::GetWindowObject(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  int routing_id = args[0]->Int32Value();

  // Dark magic to digg out the RenderView from its id.
  content::RenderViewImpl* render_view = static_cast<content::RenderViewImpl*>(
                                                                               content::RenderViewImpl::FromRoutingID(routing_id));
  if (!render_view) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, "Unable to get render view in GetWindowObject"))));
    return;
  }
  // Return the window object.
  args.GetReturnValue().Set(render_view->GetWebView()->mainFrame()->mainWorldScriptContext()->Global());
}

}  // namespace nwapi
