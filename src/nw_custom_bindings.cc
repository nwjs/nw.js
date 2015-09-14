// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/nw_custom_bindings.h"

#include "content/renderer/render_view_impl.h"

#include <string>

#include "base/bind.h"
#include "base/files/file_util.h"
#include "extensions/renderer/script_context.h"
#include "v8/include/v8.h"

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
//#include "third_party/WebKit/Source/core/inspector/InspectorInstrumentation.h"
//#include "third_party/WebKit/Source/core/inspector/InspectorResourceAgent.h"

#undef CHECK
#include "V8HTMLIFrameElement.h"

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
                base::Bind(&NWCustomBindings::EvalScript,
                           base::Unretained(this)));
  RouteFunction("getAbsolutePath",
                base::Bind(&NWCustomBindings::GetAbsolutePath,
                           base::Unretained(this)));
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
    web_frame = blink::WebFrame::fromFrame(iframe->contentFrame());
  }
#if defined(OS_WIN)
  base::string16 jscript((WCHAR*)*v8::String::Value(args[1]));
#else
  base::string16 jscript = *v8::String::Value(args[1]);
#endif
  if (web_frame) {
    result = web_frame->executeScriptAndReturnValue(WebScriptSource(jscript));
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
#if defined(OS_WIN)
  base::FilePath path((WCHAR*)*v8::String::Value(args[1]));
#else
  base::FilePath path(*v8::String::Utf8Value(args[1]));
#endif
  base::File file(path, base::File::FLAG_OPEN | base::File::FLAG_READ);
  if (file.IsValid()) {
    int64 length = file.GetLength();
    if (length > 0 && length < INT_MAX) {
      int size = static_cast<int>(length);
      std::vector<unsigned char> raw_data;
      raw_data.resize(size);
      uint8_t* data = reinterpret_cast<uint8_t*>(&(raw_data.front()));
      if (file.ReadAtCurrentPos((char*)data, size) == length) {
        WebFrame* main_frame = render_frame->GetWebFrame();
        v8::Handle<v8::String> source_string = v8::String::NewFromUtf8(isolate, "");
        v8::ScriptCompiler::CachedData* cache;
        cache = new v8::ScriptCompiler::CachedData(
                                                   data, length, v8::ScriptCompiler::CachedData::BufferNotOwned);
        v8::ScriptCompiler::Source source(source_string, cache);
        v8::Local<v8::UnboundScript> script;
        script = v8::ScriptCompiler::CompileUnbound(
                                                    isolate, &source, v8::ScriptCompiler::kConsumeCodeCache);
        ASSERT(!cache->rejected);
        v8::Handle<v8::Value> result;
        v8::Handle<v8::Object> frm = v8::Handle<v8::Object>::Cast(args[0]);
        WebFrame* web_frame = NULL;
        if (frm->IsNull()) {
          web_frame = main_frame;
        }else{
          blink::HTMLIFrameElement* iframe = blink::V8HTMLIFrameElement::toImpl(frm);
          web_frame = blink::WebFrame::fromFrame(iframe->contentFrame());
        }
        v8::Context::Scope cscope (web_frame->mainWorldScriptContext());
        v8::FixSourceNWBin(isolate, script);
        result = script->BindToCurrentContext()->Run();
        args.GetReturnValue().Set(result);
      }
    }
  }
  return;
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

}  // namespace extensions
