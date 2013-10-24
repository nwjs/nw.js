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

#include "content/nw/src/api/bindings_common.h"

#include "base/logging.h"
#include "base/values.h"
#include "content/nw/src/api/api_messages.h"
#include "content/public/renderer/render_view.h"
#include "content/public/renderer/render_thread.h"
#include "content/public/renderer/v8_value_converter.h"
#include "third_party/WebKit/public/web/WebView.h"
#include "third_party/WebKit/public/web/WebFrame.h"
#include "ui/base/resource/resource_bundle.h"

using content::RenderView;
using content::RenderThread;
using content::V8ValueConverter;
using WebKit::WebFrame;
using WebKit::WebView;

namespace {
RenderView* GetRenderView(v8::Handle<v8::Context> ctx) {
  WebFrame* frame = WebFrame::frameForContext(ctx);
  if (!frame)
    return NULL;

  WebView* view = frame->view();
  if (!view)
    return NULL;  // can happen during closing.

  RenderView* render_view = RenderView::FromWebView(view);
  return render_view;
}

}

RenderView* GetCurrentRenderView() {
  v8::Local<v8::Context> ctx = v8::Context::GetCurrent();
  return GetRenderView(ctx);
}

RenderView* GetEnteredRenderView() {
  v8::Local<v8::Context> ctx = v8::Context::GetEntered();
  return GetRenderView(ctx);
}

base::StringPiece GetStringResource(int resource_id) {
  return ResourceBundle::GetSharedInstance().GetRawDataResource(resource_id);
}

namespace remote {

v8::Handle<v8::Value> AllocateObject(int routing_id,
                                     int object_id,
                                     const std::string& type,
                                     v8::Handle<v8::Value> options) {
  v8::HandleScope handle_scope;

  scoped_ptr<V8ValueConverter> converter(V8ValueConverter::create());
  converter->SetStripNullFromObjects(true);

  scoped_ptr<base::Value> value_option(
      converter->FromV8Value(options, v8::Context::GetCurrent()));
  if (!value_option.get() ||
      !value_option->IsType(base::Value::TYPE_DICTIONARY))
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
        "Unable to convert 'option' passed to AllocateObject")));

  DVLOG(1) << "remote::AllocateObject(routing_id=" << routing_id << ", object_id=" << object_id << ")";

  RenderThread::Get()->Send(new ShellViewHostMsg_Allocate_Object(
      routing_id,
      object_id,
      type,
      *static_cast<base::DictionaryValue*>(value_option.get())));
  return v8::Undefined();
}

v8::Handle<v8::Value> DeallocateObject(int routing_id,
                                       int object_id) {
  RenderThread::Get()->Send(new ShellViewHostMsg_Deallocate_Object(
      routing_id, object_id));
  return v8::Undefined();
}

v8::Handle<v8::Value> CallObjectMethod(int routing_id,
                                       int object_id,
                                       const std::string& type,
                                       const std::string& method,
                                       v8::Handle<v8::Value> args) {
  scoped_ptr<V8ValueConverter> converter(V8ValueConverter::create());

  scoped_ptr<base::Value> value_args(
      converter->FromV8Value(args, v8::Context::GetCurrent()));
  if (!value_args.get() ||
      !value_args->IsType(base::Value::TYPE_LIST))
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
        "Unable to convert 'args' passed to CallObjectMethod")));

  RenderThread::Get()->Send(new ShellViewHostMsg_Call_Object_Method(
      routing_id,
      object_id,
      type,
      method,
      *static_cast<base::ListValue*>(value_args.get())));
  return v8::Undefined();
}

v8::Handle<v8::Value> CallObjectMethodSync(int routing_id,
                                           int object_id,
                                           const std::string& type,
                                           const std::string& method,
                                           v8::Handle<v8::Value> args) {
  scoped_ptr<V8ValueConverter> converter(V8ValueConverter::create());

  scoped_ptr<base::Value> value_args(
      converter->FromV8Value(args, v8::Context::GetCurrent()));
  if (!value_args.get() ||
      !value_args->IsType(base::Value::TYPE_LIST))
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
        "Unable to convert 'args' passed to CallObjectMethodSync")));

  base::ListValue result;
  RenderThread::Get()->Send(new ShellViewHostMsg_Call_Object_Method_Sync(
      routing_id,
      object_id,
      type,
      method,
      *static_cast<base::ListValue*>(value_args.get()),
      &result));
  return converter->ToV8Value(&result, v8::Context::GetCurrent());
}

}  // namespace remote

