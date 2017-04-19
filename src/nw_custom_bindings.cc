// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "base/compiler_specific.h"

MSVC_PUSH_DISABLE_WARNING(4305)

#include "content/nw/src/nw_custom_bindings.h"

#include "content/renderer/render_view_impl.h"
#include "content/public/renderer/render_thread.h"

#include <string>

#include "base/bind.h"
#include "base/files/file_util.h"
#include "extensions/renderer/script_context.h"
#include "v8/include/v8.h"

using namespace blink;
#if defined(OS_WIN)
#define _USE_MATH_DEFINES
#include <math.h>
#endif

//#undef FROM_HERE
#undef TRACE_EVENT0
#undef TRACE_EVENT1
#undef TRACE_EVENT2
#undef TRACE_EVENT_WITH_FLOW0
#undef TRACE_EVENT_WITH_FLOW1
#undef TRACE_EVENT_WITH_FLOW2
#undef TRACE_EVENT_MARK_WITH_TIMESTAMP1
#undef TRACE_EVENT_COPY_MARK_WITH_TIMESTAMP
#undef TRACE_EVENT_COPY_NESTABLE_ASYNC_BEGIN_WITH_TIMESTAMP0
#undef TRACE_EVENT_COPY_NESTABLE_ASYNC_END_WITH_TIMESTAMP0
#undef TRACE_EVENT_ENTER_CONTEXT
#undef TRACE_EVENT_LEAVE_CONTEXT
#undef TRACE_STR_COPY
#undef TRACE_ID_MANGLE
#undef TRACE_ID_DONT_MANGLE
#undef TRACE_ID_WITH_SCOPE
#undef TRACE_EVENT_SCOPED_SAMPLING_STATE_FOR_BUCKET
#undef TRACE_EVENT_GET_SAMPLING_STATE_FOR_BUCKET
#undef TRACE_EVENT_SET_SAMPLING_STATE_FOR_BUCKET
#undef TRACE_EVENT_API_GET_CATEGORY_GROUP_ENABLED
#undef TRACE_EVENT_API_ADD_TRACE_EVENT
#undef TRACE_EVENT_API_UPDATE_TRACE_EVENT_DURATION
#undef INTERNAL_TRACE_EVENT_UID2
#undef INTERNAL_TRACE_EVENT_GET_CATEGORY_INFO
#undef INTERNAL_TRACE_EVENT_ADD
#undef INTERNAL_TRACE_EVENT_ADD_SCOPED
#undef INTERNAL_TRACE_EVENT_ADD_SCOPED_WITH_FLOW
#undef INTERNAL_TRACE_EVENT_ADD_WITH_ID
#undef INTERNAL_TRACE_EVENT_ADD_WITH_TIMESTAMP
#undef INTERNAL_TRACE_EVENT_SCOPED_CONTEXT
#undef INTERNAL_TRACE_EVENT_CATEGORY_GROUP_ENABLED_FOR_RECORDING_MODE
//#include "third_party/WebKit/Source/config.h"
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
#include "third_party/WebKit/public/web/WebSecurityPolicy.h"
#include "third_party/WebKit/Source/platform/heap/Handle.h"
//#include "third_party/WebKit/Source/core/inspector/InspectorInstrumentation.h"
//#include "third_party/WebKit/Source/core/inspector/InspectorResourceAgent.h"

//#undef CHECK
#include "V8HTMLIFrameElement.h"
#include "extensions/renderer/script_context_set.h"

using blink::WebFrame;

namespace extensions {

namespace {
bool MakePathAbsolute(base::FilePath* file_path) {
  DCHECK(file_path);

  base::FilePath current_directory;
  if (!base::GetCurrentDirectory(&current_directory))
    return false;

  if (file_path->IsAbsolute())
    return true;

  if (current_directory.empty()) {
    *file_path = base::MakeAbsoluteFilePath(*file_path);
    return true;
  }

  if (!current_directory.IsAbsolute())
    return false;

#ifdef OS_LINUX
  //linux might gives "/" as current_directory, return false
  if (current_directory.value().length() <= 1)
	return false;
#endif

  *file_path = current_directory.Append(*file_path);
  return true;
}

} // namespace
NWCustomBindings::NWCustomBindings(ScriptContext* context)
    : ObjectBackedNativeHandler(context) {
  RouteFunction("crashRenderer",
                base::Bind(&NWCustomBindings::CrashRenderer,
                           base::Unretained(this)));
  RouteFunction("evalScript",
                base::Bind(&NWCustomBindings::EvalScript,
                           base::Unretained(this)));
  RouteFunction("evalNWBin",
                base::Bind(&NWCustomBindings::EvalNWBin,
                           base::Unretained(this)));
  RouteFunction("getAbsolutePath",
                base::Bind(&NWCustomBindings::GetAbsolutePath,
                           base::Unretained(this)));
  RouteFunction("addOriginAccessWhitelistEntry",
                base::Bind(&NWCustomBindings::AddOriginAccessWhitelistEntry,
                           base::Unretained(this)));
  RouteFunction("removeOriginAccessWhitelistEntry",
                base::Bind(&NWCustomBindings::RemoveOriginAccessWhitelistEntry,
                           base::Unretained(this)));
  RouteFunction("getProxyForURL",
                base::Bind(&NWCustomBindings::GetProxyForURL,
                           base::Unretained(this)));
  RouteFunction("setDevToolsJail",
                base::Bind(&NWCustomBindings::SetDevToolsJail,
                           base::Unretained(this)));
  RouteFunction("getWidgetRoutingID",
                base::Bind(&NWCustomBindings::GetWidgetRoutingID,
                           base::Unretained(this)));
  RouteFunction("getRoutingID",
                base::Bind(&NWCustomBindings::GetRoutingID,
                           base::Unretained(this)));
  RouteFunction("callInWindow",
                base::Bind(&NWCustomBindings::CallInWindow,
                           base::Unretained(this)));
}

void NWCustomBindings::CallInWindow(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Local<v8::Context> target_context = args[0]->ToObject()->CreationContext();
  v8::Local<v8::String> method   = args[1]->ToString();
  ScriptContext* context = ScriptContextSet::GetContextByV8Context(target_context);
  v8::Context::Scope cscope(target_context);
  v8::MaybeLocal<v8::Value> val = args[0]->ToObject()->Get(target_context, method);
  v8::Local<v8::Function> func = val.ToLocalChecked().As<v8::Function>();
  v8::Local<v8::Value>* argv = new v8::Local<v8::Value>[args.Length() - 2];
  for (int i = 0; i < args.Length() - 2; i++)
    argv[i] = args[i + 2];
  context->CallFunction(func, args.Length() - 2, argv);
  delete[] argv;
}

void NWCustomBindings::GetRoutingID(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  int routing_id = context()->GetRenderFrame()->GetRenderView()->GetMainRenderFrame()->GetRoutingID();
  args.GetReturnValue().Set(v8::Integer::New(GetIsolate(), routing_id));
}

void NWCustomBindings::GetWidgetRoutingID(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  int routing_id = context()->GetRenderFrame()->GetRenderView()->GetRoutingID();
  args.GetReturnValue().Set(v8::Integer::New(GetIsolate(), routing_id));
}

void NWCustomBindings::CrashRenderer(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  //LOG(ERROR) << "crashing renderer";
  int* ptr = nullptr;
  *ptr = 1;
}

void NWCustomBindings::EvalScript(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  content::RenderFrame* render_frame = context()->GetRenderFrame();
  if (!render_frame)
    return;
  WebFrame* main_frame = render_frame->GetWebFrame();
  v8::Handle<v8::Value> result;
  v8::Handle<v8::Object> frm = v8::Handle<v8::Object>::Cast(args[0]);
  WebFrame* web_frame = NULL;
  if (frm->IsNull()) {
    web_frame = main_frame;
  }else{
    blink::HTMLIFrameElement* iframe = blink::V8HTMLIFrameElement::toImpl(frm);
    web_frame = blink::WebFrame::FromFrame(iframe->ContentFrame());
  }
#if defined(OS_WIN)
  base::string16 jscript((WCHAR*)*v8::String::Value(args[1]));
#else
  base::string16 jscript = *v8::String::Value(args[1]);
#endif
  if (web_frame) {
    result = web_frame->ExecuteScriptAndReturnValue(blink::WebScriptSource(blink::WebString::FromUTF16(jscript)));
  }
  args.GetReturnValue().Set(result);
  return;
}

void NWCustomBindings::EvalNWBin(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  content::RenderFrame* render_frame = context()->GetRenderFrame();
  if (!render_frame)
    return;

  if (!args[1]->IsArrayBuffer()) {
    return;
  }

  v8::Local<v8::ArrayBuffer> ab = args[1].As<v8::ArrayBuffer>();
  v8::ArrayBuffer::Contents contents = ab->GetContents();
  int64_t length = contents.ByteLength();
  uint8_t *data = reinterpret_cast<uint8_t*>(contents.Data());

  WebFrame* main_frame = render_frame->GetWebFrame();
  v8::Handle<v8::String> source_string = v8::String::NewFromUtf8(isolate, "");
  v8::ScriptCompiler::CachedData* cache;
  cache = new v8::ScriptCompiler::CachedData(
                                             data, length, v8::ScriptCompiler::CachedData::BufferNotOwned);
  v8::ScriptCompiler::Source source(source_string, cache);
  v8::Local<v8::UnboundScript> script;
  script = v8::ScriptCompiler::CompileUnboundScript(
                                                    isolate, &source, v8::ScriptCompiler::kConsumeCodeCache).ToLocalChecked();
  ASSERT(!cache->rejected);
  v8::Handle<v8::Value> result;
  v8::Handle<v8::Object> frm = v8::Handle<v8::Object>::Cast(args[0]);
  WebFrame* web_frame = NULL;
  if (frm->IsNull()) {
    web_frame = main_frame;
  }else{
    blink::HTMLIFrameElement* iframe = blink::V8HTMLIFrameElement::toImpl(frm);
    web_frame = blink::WebFrame::FromFrame(iframe->ContentFrame());
  }
  v8::Context::Scope cscope (web_frame->MainWorldScriptContext());
  v8::FixSourceNWBin(isolate, script);
  result = script->BindToCurrentContext()->Run();
  args.GetReturnValue().Set(result);
}

void NWCustomBindings::GetAbsolutePath(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  base::FilePath path = base::FilePath::FromUTF8Unsafe(*v8::String::Utf8Value(args[0]));
  MakePathAbsolute(&path);
#if defined(OS_POSIX)
  args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, path.value().c_str()));
#else
  args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, path.AsUTF8Unsafe().c_str()));
#endif
}

void NWCustomBindings::AddOriginAccessWhitelistEntry(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();

  std::string sourceOrigin        = *v8::String::Utf8Value(args[0]);
  std::string destinationProtocol = *v8::String::Utf8Value(args[1]);
  std::string destinationHost     = *v8::String::Utf8Value(args[2]);
  bool allowDestinationSubdomains = args[3]->ToBoolean()->Value();

  blink::WebSecurityPolicy::AddOriginAccessWhitelistEntry(GURL(sourceOrigin),
                                                          blink::WebString::FromUTF8(destinationProtocol),
                                                          blink::WebString::FromUTF8(destinationHost),
                                                          allowDestinationSubdomains);
  args.GetReturnValue().Set(v8::Undefined(isolate));
  return;
}

void NWCustomBindings::RemoveOriginAccessWhitelistEntry(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();

  std::string sourceOrigin        = *v8::String::Utf8Value(args[0]);
  std::string destinationProtocol = *v8::String::Utf8Value(args[1]);
  std::string destinationHost     = *v8::String::Utf8Value(args[2]);
  bool allowDestinationSubdomains = args[3]->ToBoolean()->Value();

  blink::WebSecurityPolicy::RemoveOriginAccessWhitelistEntry(GURL(sourceOrigin),
                                                          blink::WebString::FromUTF8(destinationProtocol),
                                                          blink::WebString::FromUTF8(destinationHost),
                                                          allowDestinationSubdomains);
  args.GetReturnValue().Set(v8::Undefined(isolate));
  return;
}

void NWCustomBindings::GetProxyForURL(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();

  std::string url = *v8::String::Utf8Value(args[0]);
  GURL gurl(url);
  if (!gurl.is_valid()) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
                                     "Invalid URL passed to App.getProxyForURL()"))));
    return;
  }
  std::string proxy;
  bool result = content::RenderThread::Get()->ResolveProxy(gurl, &proxy);
  if (!result) {
    args.GetReturnValue().Set(v8::Undefined(isolate));
    return;
  }
  args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, proxy.c_str()));
  return;
}

void NWCustomBindings::SetDevToolsJail(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  if (!args.Length())
    return;
  content::RenderFrame* render_frame = context()->GetRenderFrame();
  if (!render_frame)
    return;
  WebFrame* main_frame = render_frame->GetWebFrame();
  if (args[0]->IsNull() || args[0]->IsUndefined()) {
    main_frame->setDevtoolsJail(NULL);
  }else{
	  v8::Handle<v8::Object> frm = v8::Handle<v8::Object>::Cast(args[0]);
	  blink::HTMLIFrameElement* iframe = blink::V8HTMLIFrameElement::toImpl(frm);
    main_frame->setDevtoolsJail(blink::WebFrame::FromFrame(iframe->ContentFrame()));
  }
  args.GetReturnValue().Set(v8::Undefined(isolate));
  return;
}

}  // namespace extensions
MSVC_POP_WARNING()
