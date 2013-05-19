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
#include "content/nw/src/api/api_messages.h"
#include "content/nw/src/api/bindings_common.h"
#include "content/public/renderer/render_view.h"
#include "content/public/renderer/render_thread.h"
#include "content/public/renderer/v8_value_converter.h"
#include "grit/nw_resources.h"

using content::RenderView;
using content::V8ValueConverter;
using base::FilePath;

namespace api {

namespace {

v8::Handle<v8::String> WrapSource(v8::Handle<v8::String> source) {
  v8::HandleScope handle_scope;
  v8::Handle<v8::String> left =
      v8::String::New("(function(nw, exports) {");
  v8::Handle<v8::String> right = v8::String::New("\n})");
  return handle_scope.Close(
      v8::String::Concat(left, v8::String::Concat(source, right)));
}

// Similar to node's `require` function, save functions in `exports`.
void RequireFromResource(v8::Handle<v8::Object> root,
                         v8::Handle<v8::Object> gui,
                         v8::Handle<v8::String> name,
                         int resource_id) {
  v8::HandleScope scope;

  v8::Handle<v8::String> source = v8::String::NewExternal(
      new StaticV8ExternalAsciiStringResource(
          GetStringResource(resource_id)));
  v8::Handle<v8::String> wrapped_source = WrapSource(source);

  v8::Handle<v8::Script> script(v8::Script::New(wrapped_source, name));
  v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(script->Run());
  v8::Handle<v8::Value> args[] = { root, gui };
  func->Call(root, 2, args);
}

bool MakePathAbsolute(FilePath* file_path) {
  DCHECK(file_path);

  FilePath current_directory;
  if (!file_util::GetCurrentDirectory(&current_directory))
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
}

DispatcherBindings::~DispatcherBindings() {
}

v8::Handle<v8::FunctionTemplate>
DispatcherBindings::GetNativeFunction(v8::Handle<v8::String> name) {
  if (name->Equals(v8::String::New("RequireNwGui")))
    return v8::FunctionTemplate::New(RequireNwGui);
  else if (name->Equals(v8::String::New("GetAbsolutePath")))
    return v8::FunctionTemplate::New(GetAbsolutePath);
  else if (name->Equals(v8::String::New("GetShellIdForCurrentContext")))
    return v8::FunctionTemplate::New(GetShellIdForCurrentContext);
  else if (name->Equals(v8::String::New("GetRoutingIDForCurrentContext")))
    return v8::FunctionTemplate::New(GetRoutingIDForCurrentContext);
  else if (name->Equals(v8::String::New("CreateShell")))
    return v8::FunctionTemplate::New(CreateShell);
  else if (name->Equals(v8::String::New("AllocateObject")))
    return v8::FunctionTemplate::New(AllocateObject);
  else if (name->Equals(v8::String::New("DeallocateObject")))
    return v8::FunctionTemplate::New(DeallocateObject);
  else if (name->Equals(v8::String::New("CallObjectMethod")))
    return v8::FunctionTemplate::New(CallObjectMethod);
  else if (name->Equals(v8::String::New("CallObjectMethodSync")))
    return v8::FunctionTemplate::New(CallObjectMethodSync);
  else if (name->Equals(v8::String::New("CallStaticMethod")))
    return v8::FunctionTemplate::New(CallStaticMethod);
  else if (name->Equals(v8::String::New("CallStaticMethodSync")))
    return v8::FunctionTemplate::New(CallStaticMethodSync);

  NOTREACHED() << "Trying to get an non-exist function in DispatcherBindings:"
               << *v8::String::Utf8Value(name);
  return v8::FunctionTemplate::New();
}

// static
v8::Handle<v8::Value>
DispatcherBindings::RequireNwGui(const v8::Arguments& args) {
  v8::HandleScope scope;

  // Initialize lazily
  v8::Local<v8::String> NwGuiSymbol = v8::String::NewSymbol("nwGui");
  v8::Local<v8::Value> NwGuiHidden = args.This()->Get(NwGuiSymbol);
  if (NwGuiHidden->IsObject())
    return scope.Close(NwGuiHidden);

  v8::Local<v8::Object> NwGui = v8::Object::New();
  args.This()->Set(NwGuiSymbol, NwGui);
  RequireFromResource(args.This(),
      NwGui, v8::String::New("base.js"), IDR_NW_API_BASE_JS);
  RequireFromResource(args.This(),
      NwGui, v8::String::New("menuitem.js"), IDR_NW_API_MENUITEM_JS);
  RequireFromResource(args.This(),
      NwGui, v8::String::New("menu.js"), IDR_NW_API_MENU_JS);
  RequireFromResource(args.This(),
      NwGui, v8::String::New("tray.js"), IDR_NW_API_TRAY_JS);
  RequireFromResource(args.This(),
      NwGui, v8::String::New("clipboard.js"), IDR_NW_API_CLIPBOARD_JS);
  RequireFromResource(args.This(),
      NwGui, v8::String::New("window.js"), IDR_NW_API_WINDOW_JS);
  RequireFromResource(args.This(),
      NwGui, v8::String::New("shell.js"), IDR_NW_API_SHELL_JS);
  RequireFromResource(args.This(),
      NwGui, v8::String::New("app.js"), IDR_NW_API_APP_JS);

  return scope.Close(NwGui);
}

// static
v8::Handle<v8::Value>
DispatcherBindings::GetAbsolutePath(const v8::Arguments& args) {
  FilePath path = FilePath::FromUTF8Unsafe(*v8::String::Utf8Value(args[0]));
  MakePathAbsolute(&path);
#if defined(OS_POSIX)
  return v8::String::New(path.value().c_str());
#else
  return v8::String::New(path.AsUTF8Unsafe().c_str());
#endif
}

// static
v8::Handle<v8::Value>
DispatcherBindings::GetShellIdForCurrentContext(const v8::Arguments& args) {
  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "Unable to get render view in CallStaticMethodSync")));
  }

  int id = -1;
  render_view->Send(new ShellViewHostMsg_GetShellId(MSG_ROUTING_NONE, &id));
  return v8::Integer::New(id);
}

// static
v8::Handle<v8::Value>
DispatcherBindings::GetRoutingIDForCurrentContext(const v8::Arguments& args) {
  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "Unable to get render view in GetRoutingIDForCurrentContext")));
  }

  return v8::Integer::New(render_view->GetRoutingID());
}

// static
v8::Handle<v8::Value>
DispatcherBindings::CreateShell(const v8::Arguments& args) {
  if (args.Length() < 2)
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
        "CreateShell requries 2 arguments")));

  std::string url = *v8::String::Utf8Value(args[0]);

  scoped_ptr<V8ValueConverter> converter(V8ValueConverter::create());

  scoped_ptr<base::Value> value_manifest(
      converter->FromV8Value(args[1], v8::Context::GetCurrent()));
  if (!value_manifest.get() ||
      !value_manifest->IsType(base::Value::TYPE_DICTIONARY)) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "Unable to convert 'options' passed to CreateShell")));
  }

  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "Unable to get render view in CallStaticMethod")));
  }

  int routing_id = -1;
  render_view->Send(new ShellViewHostMsg_CreateShell(
      render_view->GetRoutingID(),
      url,
      *static_cast<base::DictionaryValue*>(value_manifest.get()),
      &routing_id));

  return v8::Integer::New(routing_id);
}

// static
v8::Handle<v8::Value>
DispatcherBindings::AllocateObject(const v8::Arguments& args) {
  if (args.Length() < 3)
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
        "AllocateObject requries 3 arguments")));

  int object_id = args[0]->Int32Value();
  std::string name = *v8::String::Utf8Value(args[1]);

  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "Unable to get render view in AllocateObject")));
  }

  return remote::AllocateObject(
      render_view->GetRoutingID(), object_id, name, args[2]);
}

// static
v8::Handle<v8::Value>
DispatcherBindings::DeallocateObject(const v8::Arguments& args) {
  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "Unable to get render view in DeallocateObject")));
  }

  if (args.Length() < 1)
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
        "DeallocateObject requries 1 arguments")));

  return remote::DeallocateObject(render_view->GetRoutingID(),
                                  args[0]->Int32Value());
}

// static
v8::Handle<v8::Value>
DispatcherBindings::CallObjectMethod(const v8::Arguments& args) {
  if (args.Length() < 4)
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
        "CallObjectMethod requries 4 arguments")));

  int object_id = args[0]->Int32Value();
  std::string type = *v8::String::Utf8Value(args[1]);
  std::string method = *v8::String::Utf8Value(args[2]);

  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "Unable to get render view in CallObjectMethod")));
  }

  return remote::CallObjectMethod(
      render_view->GetRoutingID(), object_id, type, method, args[3]);
}

// static
v8::Handle<v8::Value> DispatcherBindings::CallObjectMethodSync(
    const v8::Arguments& args) {
  if (args.Length() < 4)
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
        "CallObjectMethodSync requries 4 arguments")));

  int object_id = args[0]->Int32Value();
  std::string type = *v8::String::Utf8Value(args[1]);
  std::string method = *v8::String::Utf8Value(args[2]);

  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "Unable to get render view in CallObjectMethod")));
  }

  return remote::CallObjectMethodSync(
      render_view->GetRoutingID(), object_id, type, method, args[3]);
}

// static
v8::Handle<v8::Value> DispatcherBindings::CallStaticMethod(
    const v8::Arguments& args) {
  if (args.Length() < 3) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "CallStaticMethod requries 3 arguments")));
  }

  std::string type = *v8::String::Utf8Value(args[0]);
  std::string method = *v8::String::Utf8Value(args[1]);

  scoped_ptr<V8ValueConverter> converter(V8ValueConverter::create());

  scoped_ptr<base::Value> value_args(
      converter->FromV8Value(args[2], v8::Context::GetCurrent()));
  if (!value_args.get() ||
      !value_args->IsType(base::Value::TYPE_LIST)) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "Unable to convert 'args' passed to CallStaticMethod")));
  }

  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "Unable to get render view in CallStaticMethod")));
  }

  render_view->Send(new ShellViewHostMsg_Call_Static_Method(
        render_view->GetRoutingID(),
        type,
        method,
        *static_cast<base::ListValue*>(value_args.get())));
  return v8::Undefined();
}

// static
v8::Handle<v8::Value> DispatcherBindings::CallStaticMethodSync(
    const v8::Arguments& args) {
  if (args.Length() < 3) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "CallStaticMethodSync requries 3 arguments")));
  }

  std::string type = *v8::String::Utf8Value(args[0]);
  std::string method = *v8::String::Utf8Value(args[1]);

  scoped_ptr<V8ValueConverter> converter(V8ValueConverter::create());

  scoped_ptr<base::Value> value_args(
      converter->FromV8Value(args[2], v8::Context::GetCurrent()));
  if (!value_args.get() ||
      !value_args->IsType(base::Value::TYPE_LIST)) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "Unable to convert 'args' passed to CallStaticMethodSync")));
  }

  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "Unable to get render view in CallStaticMethodSync")));
  }

  base::ListValue result;
  render_view->Send(new ShellViewHostMsg_Call_Static_Method_Sync(
        MSG_ROUTING_NONE,
        type,
        method,
        *static_cast<base::ListValue*>(value_args.get()),
        &result));
  return converter->ToV8Value(&result, v8::Context::GetCurrent());
}

}  // namespace api
