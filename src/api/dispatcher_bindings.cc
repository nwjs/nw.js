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

#include "content/nw/src/api/dispatcher_bindings.h"

#include "base/logging.h"
#include "base/values.h"
#include "content/public/renderer/render_view.h"
#include "content/public/renderer/render_thread.h"
#include "content/public/renderer/v8_value_converter.h"
#include "content/nw/src/api/api_messages.h"
#include "grit/nw_resources.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebFrame.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebView.h"
#include "ui/base/resource/resource_bundle.h"

using content::RenderView;
using content::V8ValueConverter;
using WebKit::WebFrame;
using WebKit::WebView;

namespace api {

namespace {

base::StringPiece GetStringResource(int resource_id) {
  return ResourceBundle::GetSharedInstance().GetRawDataResource(
      resource_id, ui::SCALE_FACTOR_NONE);
}

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

DispatcherBindings::DispatcherBindings()
    : v8::Extension("dispatcher_bindings.js",
                    GetStringResource(
                        IDR_NW_API_DISPATCHER_BINDINGS_JS).data(),
                    0,     // num dependencies.
                    NULL,  // dependencies array.
                    GetStringResource(
                        IDR_NW_API_DISPATCHER_BINDINGS_JS).size()) {
}

DispatcherBindings::~DispatcherBindings() {
}

// static
v8::Handle<v8::FunctionTemplate>
DispatcherBindings::GetNativeFunction(v8::Handle<v8::String> name) {
  if (name->Equals(v8::String::New("GetConstructorName")))
    return v8::FunctionTemplate::New(GetConstructorName);
  else if (name->Equals(v8::String::New("GetNextObjectId")))
    return v8::FunctionTemplate::New(GetNextObjectId);
  else if (name->Equals(v8::String::New("AllocateObject")))
    return v8::FunctionTemplate::New(AllocateObject);

  NOTREACHED();
  return v8::FunctionTemplate::New();
}

// static
v8::Handle<v8::Value>
DispatcherBindings::GetConstructorName(const v8::Arguments& args) {
  return args[0]->ToObject()->GetConstructorName();
}

// static
v8::Handle<v8::Value>
DispatcherBindings::GetNextObjectId(const v8::Arguments& args) {
  static int next_object_id = 1;
  return v8::Integer::New(next_object_id++);
}

// static
v8::Handle<v8::Value>
DispatcherBindings::AllocateObject(const v8::Arguments& args) {
  if (args.Length() < 3) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "AllocateObject requries 3 arguments")));
  }

  int object_id = args[0]->Int32Value();
  std::string name = *v8::String::Utf8Value(args[1]);

  scoped_ptr<V8ValueConverter> converter(V8ValueConverter::create());
  converter->SetStripNullFromObjects(true);

  scoped_ptr<base::Value> value_option(
      converter->FromV8Value(args[2], v8::Context::GetCurrent()));
  if (!value_option.get() ||
      !value_option->IsType(base::Value::TYPE_DICTIONARY)) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "Unable to convert 'option' passed to AllocateObject")));
  }

  // Get the current RenderView so that we can send a routed IPC message from
  // the correct source.
  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "Unable to get render view in AllocateObject")));
  }

  render_view->Send(new ShellViewHostMsg_Allocate_Object(
        render_view->GetRoutingID(),
        object_id,
        name,
        *static_cast<base::DictionaryValue*>(value_option.get())));
  return v8::Undefined();
}

// static
v8::Handle<v8::Value>
DispatcherBindings::DeallocateObject(const v8::Arguments& args) {
  if (args.Length() < 1) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "DeallocateObject requries 1 arguments")));
  }

  int object_id = args[0]->Int32Value();

  // Get the current RenderView so that we can send a routed IPC message from
  // the correct source.
  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "Unable to get render view in DeallocateObject")));
  }

  render_view->Send(new ShellViewHostMsg_Deallocate_Object(
        render_view->GetRoutingID(), object_id));
  return v8::Undefined();
}

// static
v8::Handle<v8::Value>
DispatcherBindings::CallObjectMethod(const v8::Arguments& args) {
  if (args.Length() < 4) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "CallObjectMethod requries 4 arguments")));
  }

  int object_id = args[0]->Int32Value();
  std::string type = *v8::String::Utf8Value(args[1]);
  std::string method = *v8::String::Utf8Value(args[2]);

  scoped_ptr<V8ValueConverter> converter(V8ValueConverter::create());

  scoped_ptr<base::Value> value_args(
      converter->FromV8Value(args[3], v8::Context::GetCurrent()));
  if (!value_args.get() ||
      !value_args->IsType(base::Value::TYPE_LIST)) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "Unable to convert 'args' passed to CallObjectMethod")));
  }

  // Get the current RenderView so that we can send a routed IPC message from
  // the correct source.
  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "Unable to get render view in CallObjectMethod")));
  }

  render_view->Send(new ShellViewHostMsg_Call_Object_Method(
        render_view->GetRoutingID(),
        object_id,
        type,
        method,
        *static_cast<base::ListValue*>(value_args.get())));
  return v8::Undefined();
}

}  // namespace api
