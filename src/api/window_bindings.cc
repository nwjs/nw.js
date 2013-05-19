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
#include "content/common/child_thread.h"
#include "content/nw/src/api/bindings_common.h"
#include "content/renderer/render_view_impl.h"
#include "grit/nw_resources.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebFrame.h"

namespace api {

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
WindowBindings::GetNativeFunction(v8::Handle<v8::String> name) {
  if (name->Equals(v8::String::New("BindToShell")))
    return v8::FunctionTemplate::New(BindToShell);
  else if (name->Equals(v8::String::New("CallObjectMethod")))
    return v8::FunctionTemplate::New(CallObjectMethod);
  else if (name->Equals(v8::String::New("CallObjectMethodSync")))
    return v8::FunctionTemplate::New(CallObjectMethodSync);
  else if (name->Equals(v8::String::New("GetWindowObject")))
    return v8::FunctionTemplate::New(GetWindowObject);

  return v8::FunctionTemplate::New();
}

// static
v8::Handle<v8::Value>
WindowBindings::BindToShell(const v8::Arguments& args) {
  int routing_id = args[0]->Int32Value();
  int object_id = args[1]->Int32Value();

  remote::AllocateObject(routing_id, object_id, "Window", v8::Object::New());

  return v8::Undefined();
}

// static
v8::Handle<v8::Value>
WindowBindings::CallObjectMethod(const v8::Arguments& args) {
  v8::Local<v8::Object> self = args[0]->ToObject();
  int routing_id = self->Get(v8::String::New("routing_id"))->Int32Value();
  int object_id = self->Get(v8::String::New("id"))->Int32Value();
  std::string method = *v8::String::Utf8Value(args[1]);

  return remote::CallObjectMethod(
      routing_id, object_id, "Window", method, args[2]);
}

// static
v8::Handle<v8::Value>
WindowBindings::CallObjectMethodSync(const v8::Arguments& args) {
  v8::HandleScope scope;

  v8::Local<v8::Object> self = args[0]->ToObject();
  int routing_id = self->Get(v8::String::New("routing_id"))->Int32Value();
  int object_id = self->Get(v8::String::New("id"))->Int32Value();
  std::string method = *v8::String::Utf8Value(args[1]);

  if (method == "GetZoomLevel") {
    content::RenderViewImpl* render_view = static_cast<content::RenderViewImpl*>(
                                                                                 content::RenderViewImpl::FromRoutingID(routing_id));
    if (!render_view)
      return v8::ThrowException(v8::Exception::Error(v8::String::New(
        "Unable to get render view in GetZoomLevel")));

    float zoom_level = render_view->GetWebView()->zoomLevel();

    v8::Local<v8::Array> array = v8::Array::New();
    array->Set(0, v8::Number::New(zoom_level));
    return scope.Close(array);
  }

  if (method == "SetZoomLevel") {
    content::RenderViewImpl* render_view = static_cast<content::RenderViewImpl*>(
                                                                                 content::RenderViewImpl::FromRoutingID(routing_id));
    if (!render_view)
      return v8::ThrowException(v8::Exception::Error(v8::String::New(
        "Unable to get render view in SetZoomLevel")));

    double zoom_level = args[2]->ToNumber()->Value();
    render_view->OnSetZoomLevel(zoom_level);
    return v8::Undefined();
  }

  return remote::CallObjectMethodSync(
      routing_id, object_id, "Window", method, args[2]);
}

// static
v8::Handle<v8::Value>
WindowBindings::GetWindowObject(const v8::Arguments& args) {
  int routing_id = args[0]->Int32Value();

  // Dark magic to digg out the RenderView from its id.
  content::RenderViewImpl* render_view = static_cast<content::RenderViewImpl*>(
                                                                               content::RenderViewImpl::FromRoutingID(routing_id));
  if (!render_view)
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
        "Unable to get render view in GetWindowObject")));

  // Return the window object.
  return render_view->GetWebView()->mainFrame()->mainWorldScriptContext()->
      Global();
}

}  // namespace api
