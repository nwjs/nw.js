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

#include "base/files/file_path.h"
#include "base/file_util.h"
#include "base/logging.h"
#include "base/values.h"
#include "base/command_line.h"
#include "chrome/renderer/static_v8_external_string_resource.h"
#include "content/nw/src/breakpad_linux.h"
#include "content/nw/src/api/api_messages.h"
#include "content/nw/src/api/bindings_common.h"
#include "content/public/renderer/render_view.h"
#include "content/public/renderer/render_thread.h"
#include "content/public/renderer/v8_value_converter.h"
#include "grit/nw_resources.h"
#include "third_party/node/src/node.h"
#undef CHECK
#include "third_party/node/src/node_internals.h"
#include "third_party/node/src/req_wrap.h"
#include "third_party/WebKit/public/web/WebSecurityPolicy.h"
#include "url/gurl.h"

using content::RenderView;
using content::V8ValueConverter;
using base::FilePath;

namespace nwapi {

namespace {


v8::Handle<v8::String> WrapSource(v8::Handle<v8::String> source) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::EscapableHandleScope handle_scope(isolate);
  v8::Handle<v8::String> left =
    v8::String::NewFromUtf8(isolate, "(function(nw, exports, window) {");
  v8::Handle<v8::String> right = v8::String::NewFromUtf8(isolate, "\n})");
  return handle_scope.Escape(
      v8::String::Concat(left, v8::String::Concat(source, right)));
}

// Similar to node's `require` function, save functions in `exports`.
void RequireFromResource(v8::Handle<v8::Object> root,
                         v8::Handle<v8::Object> gui,
                         v8::Handle<v8::Object> window,
                         v8::Handle<v8::String> name,
                         int resource_id) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope handle_scope(isolate);

  v8::Handle<v8::String> source = v8::String::NewExternal(isolate,
      new StaticV8ExternalAsciiStringResource(
          GetStringResource(resource_id)));
  v8::Handle<v8::String> wrapped_source = WrapSource(source);

  {
    v8::TryCatch try_catch;
    v8::Handle<v8::Script> script(v8::Script::Compile(wrapped_source, name));
    v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(script->Run());
    v8::Handle<v8::Value> args[] = { root, gui, window };
    func->Call(root, 3, args);
    if (try_catch.HasCaught()) {
      v8::String::Utf8Value stack(try_catch.StackTrace());
      LOG(FATAL) << *stack;
    }
  }

}

bool MakePathAbsolute(FilePath* file_path) {
  DCHECK(file_path);

  FilePath current_directory;
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

}  // namespace

DispatcherBindings::DispatcherBindings()
    : v8::Extension("dispatcher_bindings.js",
                    GetStringResource(
                        IDR_NW_API_DISPATCHER_BINDINGS_JS).data(),
                    0,     // num dependencies.
                    NULL,  // dependencies array.
                    GetStringResource(
                        IDR_NW_API_DISPATCHER_BINDINGS_JS).size()) {
#if defined(OS_MACOSX)
  InitMsgIDMap();
#endif
}

DispatcherBindings::~DispatcherBindings() {
}

v8::Handle<v8::FunctionTemplate>
DispatcherBindings::GetNativeFunctionTemplate(
                                      v8::Isolate* isolate,
                                      v8::Handle<v8::String> name) {
  if (name->Equals(v8::String::NewFromUtf8(isolate, "RequireNwGui")))
    return v8::FunctionTemplate::New(isolate, RequireNwGui);
  else if (name->Equals(v8::String::NewFromUtf8(isolate, "GetAbsolutePath")))
    return v8::FunctionTemplate::New(isolate, GetAbsolutePath);
  else if (name->Equals(v8::String::NewFromUtf8(isolate, "GetShellIdForCurrentContext")))
    return v8::FunctionTemplate::New(isolate, GetShellIdForCurrentContext);
  else if (name->Equals(v8::String::NewFromUtf8(isolate, "GetRoutingIDForCurrentContext")))
    return v8::FunctionTemplate::New(isolate, GetRoutingIDForCurrentContext);
  else if (name->Equals(v8::String::NewFromUtf8(isolate, "CreateShell")))
    return v8::FunctionTemplate::New(isolate, CreateShell);
  else if (name->Equals(v8::String::NewFromUtf8(isolate, "AllocateObject")))
    return v8::FunctionTemplate::New(isolate, AllocateObject);
  else if (name->Equals(v8::String::NewFromUtf8(isolate, "DeallocateObject")))
    return v8::FunctionTemplate::New(isolate, DeallocateObject);
  else if (name->Equals(v8::String::NewFromUtf8(isolate, "CallObjectMethod")))
    return v8::FunctionTemplate::New(isolate, CallObjectMethod);
  else if (name->Equals(v8::String::NewFromUtf8(isolate, "CallObjectMethodSync")))
    return v8::FunctionTemplate::New(isolate, CallObjectMethodSync);
  else if (name->Equals(v8::String::NewFromUtf8(isolate, "CallStaticMethod")))
    return v8::FunctionTemplate::New(isolate, CallStaticMethod);
  else if (name->Equals(v8::String::NewFromUtf8(isolate, "CallStaticMethodSync")))
    return v8::FunctionTemplate::New(isolate, CallStaticMethodSync);
  else if (name->Equals(v8::String::NewFromUtf8(isolate, "CrashRenderer")))
    return v8::FunctionTemplate::New(isolate, CrashRenderer);
  else if (name->Equals(v8::String::NewFromUtf8(isolate, "SetCrashDumpDir")))
    return v8::FunctionTemplate::New(isolate, SetCrashDumpDir);
  else if (name->Equals(v8::String::NewFromUtf8(isolate, "AllocateId")))
    return v8::FunctionTemplate::New(isolate, AllocateId);
#if defined(OS_MACOSX)
  else if (name->Equals(v8::String::NewFromUtf8(isolate, "GetNSStringWithFixup")))
    return v8::FunctionTemplate::New(isolate, GetNSStringWithFixup);
  else if (name->Equals(v8::String::NewFromUtf8(isolate, "GetNSStringFWithFixup")))
    return v8::FunctionTemplate::New(isolate, GetNSStringFWithFixup);
#else
  else if (name->Equals(v8::String::NewFromUtf8(isolate, "GetNSStringWithFixup")))
    return v8::FunctionTemplate::New(isolate);
  else if (name->Equals(v8::String::NewFromUtf8(isolate, "GetNSStringFWithFixup")))
    return v8::FunctionTemplate::New(isolate);
#endif
  NOTREACHED() << "Trying to get an non-exist function in DispatcherBindings:"
               << *v8::String::Utf8Value(name);
  return v8::FunctionTemplate::New(isolate);
}

// static
void
DispatcherBindings::RequireNwGui(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::EscapableHandleScope handle_scope(isolate);

  // Initialize lazily
  v8::Local<v8::String> NwGuiSymbol = v8::String::NewFromUtf8(isolate, "nwGui", v8::String::kInternalizedString);
  v8::Local<v8::Value> NwGuiHidden = args.This()->Get(NwGuiSymbol);
  if (NwGuiHidden->IsObject()) {
    args.GetReturnValue().Set(handle_scope.Escape(NwGuiHidden));
    return;
  }

  v8::Local<v8::Context> context = isolate->GetEnteredContext();
  v8::Local<v8::Object> global = context->Global();
  ASSERT(!global->IsUndefined());
  v8::Local<v8::Context> g_context =
    v8::Local<v8::Context>::New(isolate, node::g_context);

  g_context->Enter();
  v8::Local<v8::Object> NwGui = v8::Object::New(isolate);
  args.This()->Set(NwGuiSymbol, NwGui);
  RequireFromResource(args.This(),
                      NwGui, global, v8::String::NewFromUtf8(isolate, "base.js"), IDR_NW_API_BASE_JS);
  RequireFromResource(args.This(),
                      NwGui, global, v8::String::NewFromUtf8(isolate, "menuitem.js"), IDR_NW_API_MENUITEM_JS);
  RequireFromResource(args.This(),
                      NwGui, global, v8::String::NewFromUtf8(isolate, "menu.js"), IDR_NW_API_MENU_JS);
  RequireFromResource(args.This(),
                      NwGui, global, v8::String::NewFromUtf8(isolate, "tray.js"), IDR_NW_API_TRAY_JS);
  RequireFromResource(args.This(),
                      NwGui, global, v8::String::NewFromUtf8(isolate, "clipboard.js"), IDR_NW_API_CLIPBOARD_JS);
  RequireFromResource(args.This(),
                      NwGui, global, v8::String::NewFromUtf8(isolate, "window.js"), IDR_NW_API_WINDOW_JS);
  RequireFromResource(args.This(),
                      NwGui, global, v8::String::NewFromUtf8(isolate, "shell.js"), IDR_NW_API_SHELL_JS);
  RequireFromResource(args.This(),
                      NwGui, global, v8::String::NewFromUtf8(isolate, "app.js"), IDR_NW_API_APP_JS);
  RequireFromResource(args.This(),
                      NwGui, global, v8::String::NewFromUtf8("shortcut.js"), IDR_NW_API_SHORTCUT_JS);

  g_context->Exit();
  args.GetReturnValue().Set(handle_scope.Escape(NwGui));
}

// static
void
DispatcherBindings::GetAbsolutePath(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  FilePath path = FilePath::FromUTF8Unsafe(*v8::String::Utf8Value(args[0]));
  MakePathAbsolute(&path);
#if defined(OS_POSIX)
  args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, path.value().c_str()));
#else
  args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, path.AsUTF8Unsafe().c_str()));
#endif
}

// static
void
DispatcherBindings::GetShellIdForCurrentContext(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
                                     "Unable to get render view in CallStaticMethodSync"))));
    return;
  }

  int id = -1;
  render_view->Send(new ShellViewHostMsg_GetShellId(MSG_ROUTING_NONE, &id));
  args.GetReturnValue().Set(v8::Integer::New(isolate, id));
}

// static
void
DispatcherBindings::GetRoutingIDForCurrentContext(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
                                     "Unable to get render view in GetRoutingIDForCurrentContext"))));
    return;
  }

  args.GetReturnValue().Set(v8::Integer::New(isolate, render_view->GetRoutingID()));
}

// static
void
DispatcherBindings::CreateShell(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  if (args.Length() < 2) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
                                     "CreateShell requries 2 arguments"))));
    return;
  }

  std::string url = *v8::String::Utf8Value(args[0]);

  scoped_ptr<V8ValueConverter> converter(V8ValueConverter::create());

  scoped_ptr<base::Value> value_manifest(
                                         converter->FromV8Value(args[1], isolate->GetCurrentContext()));
  if (!value_manifest.get() ||
      !value_manifest->IsType(base::Value::TYPE_DICTIONARY)) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
                                     "Unable to convert 'options' passed to CreateShell"))));
    return;
  }

  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
                                     "Unable to get render view in CreateShell"))));
    return;
  }

  int routing_id = -1;
  render_view->Send(new ShellViewHostMsg_CreateShell(
      render_view->GetRoutingID(),
      url,
      *static_cast<base::DictionaryValue*>(value_manifest.get()),
      &routing_id));

  args.GetReturnValue().Set(v8::Integer::New(isolate, routing_id));
}

// static
void
DispatcherBindings::AllocateId(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
                                     "Unable to get render view in AllocateId"))));
    return;
  }

  args.GetReturnValue().Set(remote::AllocateId(render_view->GetRoutingID()));
}

void
DispatcherBindings::AllocateObject(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  if (args.Length() < 3) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
                                     "AllocateObject requries 3 arguments"))));
    return;
  }

  int object_id = args[0]->Int32Value();
  std::string name = *v8::String::Utf8Value(args[1]);

  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
                                     "Unable to get render view in AllocateObject"))));
    return;
  }

  args.GetReturnValue().Set(remote::AllocateObject(render_view->GetRoutingID(), object_id, name, args[2]));
}

// static
void
DispatcherBindings::DeallocateObject(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
                                     "Unable to get render view in DeallocateObject"))));
    return;
  }

  if (args.Length() < 1) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
                                     "DeallocateObject requries 1 arguments"))));
    return;
  }
  args.GetReturnValue().Set(remote::DeallocateObject(render_view->GetRoutingID(), args[0]->Int32Value()));
}

// static
void
DispatcherBindings::CallObjectMethod(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  if (args.Length() < 4) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
                                     "CallObjectMethod requries 4 arguments"))));
    return;
  }

  int object_id = args[0]->Int32Value();
  std::string type = *v8::String::Utf8Value(args[1]);
  std::string method = *v8::String::Utf8Value(args[2]);

  RenderView* render_view = GetCurrentRenderView();
  if (!render_view)
    render_view = GetEnteredRenderView();
  if (!render_view) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
                                     "Unable to get render view in CallObjectMethod"))));
    return;
  }

  args.GetReturnValue().Set(remote::CallObjectMethod(render_view->GetRoutingID(), object_id, type, method, args[3]));
}

// static
void DispatcherBindings::CallObjectMethodSync(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  if (args.Length() < 4) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
                                     "CallObjectMethodSync requries 4 arguments"))));
    return;
  }

  int object_id = args[0]->Int32Value();
  std::string type = *v8::String::Utf8Value(args[1]);
  std::string method = *v8::String::Utf8Value(args[2]);

  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
                                     "Unable to get render view in CallObjectMethod"))));
    return;
  }

  args.GetReturnValue().Set(remote::CallObjectMethodSync(render_view->GetRoutingID(), object_id, type, method, args[3]));
}

// static
void DispatcherBindings::CallStaticMethod(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  if (args.Length() < 3) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
                                     "CallStaticMethod requries 3 arguments"))));
    return;
  }

  std::string type = *v8::String::Utf8Value(args[0]);
  std::string method = *v8::String::Utf8Value(args[1]);

  scoped_ptr<V8ValueConverter> converter(V8ValueConverter::create());

  scoped_ptr<base::Value> value_args(
      converter->FromV8Value(args[2], isolate->GetCurrentContext()));
  if (!value_args.get() ||
      !value_args->IsType(base::Value::TYPE_LIST)) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
                                     "Unable to convert 'args' passed to CallStaticMethod"))));
    return;
  }

  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
                                     "Unable to get render view in CallStaticMethod"))));
    return;
  }

  render_view->Send(new ShellViewHostMsg_Call_Static_Method(
        render_view->GetRoutingID(),
        type,
        method,
        *static_cast<base::ListValue*>(value_args.get())));
  args.GetReturnValue().Set(v8::Undefined(isolate));
}

// static
void DispatcherBindings::CrashRenderer(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  int* ptr = NULL;
  *ptr = 1;
}

// static
void DispatcherBindings::SetCrashDumpDir(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
#if defined(OS_WIN) || defined(OS_MACOSX)
  std::string path = *v8::String::Utf8Value(args[0]);
  SetCrashDumpPath(path.c_str());
#endif
}

// static
void DispatcherBindings::CallStaticMethodSync(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  if (args.Length() < 3) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
                                     "CallStaticMethodSync requries 3 arguments"))));
    return;
  }

  std::string type = *v8::String::Utf8Value(args[0]);
  std::string method = *v8::String::Utf8Value(args[1]);

  scoped_ptr<V8ValueConverter> converter(V8ValueConverter::create());

  RenderView* render_view = GetEnteredRenderView();
  if (!render_view) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
                                     "Unable to get render view in CallStaticMethodSync"))));
    return;
  }

  if (type == "App" && method == "getProxyForURL") {
    std::string url = *v8::String::Utf8Value(args[2]);
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

  if (type == "App" && method == "AddOriginAccessWhitelistEntry") {
    std::string sourceOrigin        = *v8::String::Utf8Value(args[2]);
    std::string destinationProtocol = *v8::String::Utf8Value(args[3]);
    std::string destinationHost     = *v8::String::Utf8Value(args[4]);
    bool allowDestinationSubdomains = args[5]->ToBoolean()->Value();

    blink::WebSecurityPolicy::addOriginAccessWhitelistEntry(GURL(sourceOrigin),
                                                             blink::WebString::fromUTF8(destinationProtocol),
                                                             blink::WebString::fromUTF8(destinationHost),
                                                             allowDestinationSubdomains);
    args.GetReturnValue().Set(v8::Undefined(isolate));
    return;
  }
  if (type == "App" && method == "RemoveOriginAccessWhitelistEntry") {
    std::string sourceOrigin        = *v8::String::Utf8Value(args[2]);
    std::string destinationProtocol = *v8::String::Utf8Value(args[3]);
    std::string destinationHost     = *v8::String::Utf8Value(args[4]);
    bool allowDestinationSubdomains = args[5]->ToBoolean()->Value();

    blink::WebSecurityPolicy::removeOriginAccessWhitelistEntry(GURL(sourceOrigin),
                                                             blink::WebString::fromUTF8(destinationProtocol),
                                                             blink::WebString::fromUTF8(destinationHost),
                                                             allowDestinationSubdomains);
    args.GetReturnValue().Set(v8::Undefined(isolate));
    return;
  }
  scoped_ptr<base::Value> value_args(
                                     converter->FromV8Value(args[2], isolate->GetCurrentContext()));
  if (!value_args.get() ||
      !value_args->IsType(base::Value::TYPE_LIST)) {
    args.GetReturnValue().Set(isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate,
                                     "Unable to convert 'args' passed to CallStaticMethodSync"))));
    return;
  }

  base::ListValue result;
  render_view->Send(new ShellViewHostMsg_Call_Static_Method_Sync(
        MSG_ROUTING_NONE,
        type,
        method,
        *static_cast<base::ListValue*>(value_args.get()),
        &result));
  args.GetReturnValue().Set(converter->ToV8Value(&result, isolate->GetCurrentContext()));
}

}  // namespace nwapi
