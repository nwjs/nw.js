// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "content/nw/src/nw_custom_bindings.h"

#define INSIDE_BLINK 1

#undef INSIDE_BLINK
#include "content/renderer/render_view_impl.h"
#include "content/public/renderer/render_thread.h"

#include <string>

#include "base/bind.h"
#include "base/files/file_util.h"
#include "extensions/renderer/script_context.h"
#include "v8/include/v8.h"

#define INSIDE_BLINK 1

using namespace blink;
#if defined(OS_WIN)
#define _USE_MATH_DEFINES
#include <math.h>
#endif

//#undef FROM_HERE
#if 0
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
//#undef TRACE_EVENT_API_GET_CATEGORY_GROUP_ENABLED
#undef TRACE_EVENT_API_ADD_TRACE_EVENT
#undef TRACE_EVENT_API_UPDATE_TRACE_EVENT_DURATION
#undef INTERNAL_TRACE_EVENT_UID2
//#undef INTERNAL_TRACE_EVENT_GET_CATEGORY_INFO
#undef INTERNAL_TRACE_EVENT_ADD
#undef INTERNAL_TRACE_EVENT_ADD_SCOPED
#undef INTERNAL_TRACE_EVENT_ADD_SCOPED_WITH_FLOW
#undef INTERNAL_TRACE_EVENT_ADD_WITH_ID
#undef INTERNAL_TRACE_EVENT_ADD_WITH_TIMESTAMP
#undef INTERNAL_TRACE_EVENT_SCOPED_CONTEXT
//#undef INTERNAL_TRACE_EVENT_CATEGORY_GROUP_ENABLED_FOR_RECORDING_MODE
#endif
//#undef LOCKS_EXCLUDED
//#undef LOCK_RETURNED
//#undef EXCLUSIVE_LOCK_FUNCTION
//#undef SHARED_LOCK_FUNCTION
//#undef UNLOCK_FUNCTION
//#undef EXCLUSIVE_TRYLOCK_FUNCTION
//#undef SHARED_TRYLOCK_FUNCTION
//#undef NO_THREAD_SAFETY_ANALYSIS

//#include "third_party/WebKit/Source/config.h"
#include "third_party/blink/renderer/core/html/html_iframe_element.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/public/web/web_frame.h"
#include "third_party/blink/public/web/web_view.h"
#include "third_party/blink/public/web/web_script_source.h"

#undef BLINK_IMPLEMENTATION
#define BLINK_IMPLEMENTATION 1
#include "base/thread_annotations.h"

#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"

#include "third_party/blink/public/web/web_document.h"
#include "third_party/blink/public/web/web_security_policy.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/core/script/modulator.h"
#include "third_party/blink/renderer/core/script/js_module_script.h"
#include "third_party/blink/renderer/core/script/module_record_resolver.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_binding_for_core.h"

//#include "third_party/WebKit/Source/core/inspector/InspectorInstrumentation.h"
//#include "third_party/WebKit/Source/core/inspector/InspectorResourceAgent.h"

//#undef CHECK
#include "v8_html_iframe_element.h"
#include "extensions/renderer/script_context_set.h"

namespace content {
#if defined(COMPONENT_BUILD) && defined(WIN32)
CONTENT_EXPORT base::FilePath g_nw_temp_dir, g_nw_old_cwd;
#else
extern base::FilePath g_nw_temp_dir, g_nw_old_cwd;
#endif
}

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

void NWCustomBindings::AddRoutes() {
  RouteHandlerFunction("crashRenderer",
                base::BindRepeating(&NWCustomBindings::CrashRenderer,
                           base::Unretained(this)));
  RouteHandlerFunction("getOldCwd",
                base::BindRepeating(&NWCustomBindings::GetOldCwd,
                           base::Unretained(this)));
  RouteHandlerFunction("evalScript",
                base::BindRepeating(&NWCustomBindings::EvalScript,
                           base::Unretained(this)));
  RouteHandlerFunction("evalNWBin",
                base::BindRepeating(&NWCustomBindings::EvalNWBin,
                           base::Unretained(this)));
  RouteHandlerFunction("getAbsolutePath",
                base::BindRepeating(&NWCustomBindings::GetAbsolutePath,
                           base::Unretained(this)));
  RouteHandlerFunction("addOriginAccessWhitelistEntry",
                base::BindRepeating(&NWCustomBindings::AddOriginAccessWhitelistEntry,
                           base::Unretained(this)));
  RouteHandlerFunction("removeOriginAccessWhitelistEntry",
                base::BindRepeating(&NWCustomBindings::RemoveOriginAccessWhitelistEntry,
                           base::Unretained(this)));
  RouteHandlerFunction("getProxyForURL",
                base::BindRepeating(&NWCustomBindings::GetProxyForURL,
                           base::Unretained(this)));
  RouteHandlerFunction("setDevToolsJail",
                base::BindRepeating(&NWCustomBindings::SetDevToolsJail,
                           base::Unretained(this)));
  RouteHandlerFunction("getWidgetRoutingID",
                base::BindRepeating(&NWCustomBindings::GetWidgetRoutingID,
                           base::Unretained(this)));
  RouteHandlerFunction("getRoutingID",
                base::BindRepeating(&NWCustomBindings::GetRoutingID,
                           base::Unretained(this)));
  RouteHandlerFunction("callInWindow",
                base::BindRepeating(&NWCustomBindings::CallInWindow,
                           base::Unretained(this)));
}

NWCustomBindings::NWCustomBindings(ScriptContext* context)
    : ObjectBackedNativeHandler(context) {
}

void NWCustomBindings::CallInWindow(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Local<v8::Context> target_context = v8::Local<v8::Object>::Cast(args[0])->CreationContext();
  v8::Local<v8::String> method   = v8::Local<v8::String>::Cast(args[1]);
  ScriptContext* context = ScriptContextSet::GetContextByV8Context(target_context);
  v8::Context::Scope cscope(target_context);
  v8::MaybeLocal<v8::Value> val = v8::Local<v8::Object>::Cast(args[0])->Get(target_context, method);
  v8::Local<v8::Function> func = val.ToLocalChecked().As<v8::Function>();
  v8::Local<v8::Value>* argv = new v8::Local<v8::Value>[args.Length() - 2];
  for (int i = 0; i < args.Length() - 2; i++)
    argv[i] = args[i + 2];
  context->SafeCallFunction(func, args.Length() - 2, argv);
  delete[] argv;
}

void NWCustomBindings::GetRoutingID(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  int routing_id = context()->GetRenderFrame()->GetMainRenderFrame()->GetRoutingID();
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
  v8::Isolate* isolate = args.GetIsolate();
  WebFrame* main_frame = render_frame->GetWebFrame();
  if (args.Length() > 2) {
    int frame_id = args[2].As<v8::Int32>()->Value();
    content::RenderFrame* frame =
      content::RenderFrame::FromRoutingID(frame_id);
    if (frame && frame->GetWebFrame())
      main_frame = frame->GetWebFrame();
    else {
      args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
           "Invalid frame id. Window could be in remote process", v8::NewStringType::kNormal).ToLocalChecked())));
      return;
    }
  }
  v8::Handle<v8::Value> result;
  WebFrame* web_frame = NULL;
  if (args[0]->IsNull()) {
    web_frame = main_frame;
  }else{
    v8::Handle<v8::Object> frm = v8::Handle<v8::Object>::Cast(args[0]);
    blink::HTMLIFrameElement* iframe = blink::V8HTMLIFrameElement::ToImpl(frm);
    web_frame = blink::WebFrame::FromCoreFrame(iframe->ContentFrame());
  }
#if defined(OS_WIN)
  std::u16string jscript((WCHAR*)*v8::String::Value(isolate, args[1]));
#else
  std::u16string jscript((char16_t*)*v8::String::Value(isolate, args[1]));
#endif
  if (web_frame) {
    blink::WebLocalFrame* local_frame = web_frame->ToWebLocalFrame();
    result = local_frame->ExecuteScriptAndReturnValue(blink::WebScriptSource(blink::WebString::FromUTF16(jscript)));
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
  int64_t length = ab->GetBackingStore()->ByteLength();
  uint8_t *data = reinterpret_cast<uint8_t*>(ab->GetBackingStore()->Data());

  WebFrame* main_frame = render_frame->GetWebFrame();
  if (args.Length() > 3) {
    int frame_id = args[3].As<v8::Int32>()->Value();
    content::RenderFrame* frame =
      content::RenderFrame::FromRoutingID(frame_id);
    if (frame && frame->GetWebFrame())
      main_frame = frame->GetWebFrame();
    else {
      args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
           "Invalid frame id. Window could be in remote process", v8::NewStringType::kNormal).ToLocalChecked())));
      return;
    }
  }
  v8::Handle<v8::String> source_string = v8::String::NewFromUtf8(isolate, "", v8::NewStringType::kNormal).ToLocalChecked();
  v8::ScriptCompiler::CachedData* cache;
  cache = new v8::ScriptCompiler::CachedData(
                                             data, length, v8::ScriptCompiler::CachedData::BufferNotOwned);

  WebFrame* web_frame = NULL;
  if (args[0]->IsNull()) {
    web_frame = main_frame;
  }else{
    v8::Handle<v8::Object> frm = v8::Handle<v8::Object>::Cast(args[0]);
    blink::HTMLIFrameElement* iframe = blink::V8HTMLIFrameElement::ToImpl(frm);
    web_frame = blink::WebFrame::FromCoreFrame(iframe->ContentFrame());
  }
  blink::WebLocalFrame* local_frame = web_frame->ToWebLocalFrame();
  if (args.Length() == 2 || args[2]->IsNull()) {
    v8::Local<v8::UnboundScript> script;
    v8::ScriptCompiler::Source source(source_string, cache);
    script = v8::ScriptCompiler::CompileUnboundScript(
                                                      isolate, &source, v8::ScriptCompiler::kConsumeCodeCache).ToLocalChecked();
    CHECK(!cache->rejected);
    v8::Local<v8::Value> result;
    v8::Context::Scope cscope (local_frame->MainWorldScriptContext());
    v8::FixSourceNWBin(isolate, script);
    ignore_result(script->BindToCurrentContext()->Run(local_frame->MainWorldScriptContext()).ToLocal(&result));
    args.GetReturnValue().Set(result);
  } else {
    v8::ScriptOrigin origin(
                            args[2].As<v8::String>(),
                            v8::Integer::New(isolate, 0),
                            v8::Integer::New(isolate, 0),
                            v8::Boolean::New(isolate, true),
                            v8::Local<v8::Integer>(),    // script id
                            v8::String::Empty(isolate),  // source_map_url
                            v8::Boolean::New(isolate, true),
                            v8::False(isolate),  // is_wasm
                            v8::True(isolate));
    v8::Local<v8::Module> module;
    v8::ScriptCompiler::Source source(source_string, origin, cache);
    module = v8::ScriptCompiler::CompileModuleWithCache(isolate, &source).ToLocalChecked();
                                                                                 //fix
                                                                                 //v8
                                                                                 //here
    v8::FixSourceNWBin(isolate, module);
    blink::Modulator* modulator = blink::Modulator::From(ToScriptStateForMainWorld(static_cast<blink::WebLocalFrameImpl*>(local_frame)->GetFrame()));
    GURL url = render_frame->GetWebFrame()->GetDocument().Url();
    v8::String::Utf8Value file(args.GetIsolate(), args[2]);
    url = url.Resolve(*file);
    // LOG(WARNING) << "registering module as: " << url;
    KURL kurl(WTF::String(url.spec().c_str()));
    //blink::ModuleRecord script_module(isolate, module, kurl);
    blink::JSModuleScript* module_script =
      blink::JSModuleScript::CreateForTest(modulator, module, kurl);
    modulator->AddToMap(kurl, module_script);
  }
}

void NWCustomBindings::GetAbsolutePath(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  base::FilePath path = base::FilePath::FromUTF8Unsafe(*v8::String::Utf8Value(isolate, args[0]));
  MakePathAbsolute(&path);
#if defined(OS_POSIX)
  args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, path.value().c_str(), v8::NewStringType::kNormal).ToLocalChecked());
#else
  args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, path.AsUTF8Unsafe().c_str(), v8::NewStringType::kNormal).ToLocalChecked());
#endif
}

void NWCustomBindings::GetOldCwd(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();
  base::FilePath path = content::g_nw_old_cwd;
#if defined(OS_POSIX)
  args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, path.value().c_str(), v8::NewStringType::kNormal).ToLocalChecked());
#else
  args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, path.AsUTF8Unsafe().c_str(), v8::NewStringType::kNormal).ToLocalChecked());
#endif
}

void NWCustomBindings::AddOriginAccessWhitelistEntry(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();

  std::string sourceOrigin        = *v8::String::Utf8Value(isolate, args[0]);
  std::string destinationProtocol = *v8::String::Utf8Value(isolate, args[1]);
  std::string destinationHost     = *v8::String::Utf8Value(isolate, args[2]);
  bool allowDestinationSubdomains = args[3].As<v8::Boolean>()->Value();

  blink::WebSecurityPolicy::AddOriginAccessAllowListEntry(GURL(sourceOrigin),
                                                          blink::WebString::FromUTF8(destinationProtocol),
                                                          blink::WebString::FromUTF8(destinationHost), 0,
                                                          allowDestinationSubdomains ?
                                                          network::mojom::CorsDomainMatchMode::kAllowSubdomains : network::mojom::CorsDomainMatchMode::kDisallowSubdomains,
                                                          network::mojom::CorsPortMatchMode::kAllowAnyPort,
                                                          network::mojom::CorsOriginAccessMatchPriority::kDefaultPriority);
  args.GetReturnValue().Set(v8::Undefined(isolate));
  return;
}

void NWCustomBindings::RemoveOriginAccessWhitelistEntry(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();

  std::string sourceOrigin        = *v8::String::Utf8Value(isolate, args[0]);

  //FIXME blink::WebSecurityPolicy::RemoveAllOriginAccessWhitelistEntriesForOrigin(GURL(sourceOrigin));

  args.GetReturnValue().Set(v8::Undefined(isolate));
  return;
}

void NWCustomBindings::GetProxyForURL(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();

  std::string url = *v8::String::Utf8Value(isolate, args[0]);
  GURL gurl(url);
  if (!gurl.is_valid()) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
                                                                                                   "Invalid URL passed to App.getProxyForURL()", v8::NewStringType::kNormal).ToLocalChecked())));
    return;
  }
  std::string proxy;
  bool result = content::RenderThread::Get()->ResolveProxy(gurl, &proxy);
  if (!result) {
    args.GetReturnValue().Set(v8::Undefined(isolate));
    return;
  }
  args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, proxy.c_str(), v8::NewStringType::kNormal).ToLocalChecked());
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
	  blink::HTMLIFrameElement* iframe = blink::V8HTMLIFrameElement::ToImpl(frm);
    main_frame->setDevtoolsJail(blink::WebFrame::FromCoreFrame(iframe->ContentFrame()));
  }
  args.GetReturnValue().Set(v8::Undefined(isolate));
  return;
}

}  // namespace extensions

