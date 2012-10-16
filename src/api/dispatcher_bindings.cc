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

#include "base/file_path.h"
#include "base/file_util.h"
#include "base/logging.h"
#include "base/values.h"
#include "base/command_line.h"
#include "chrome/renderer/static_v8_external_string_resource.h"
#include "content/public/renderer/render_view.h"
#include "content/public/renderer/render_thread.h"
#include "content/public/renderer/v8_value_converter.h"
#include "content/nw/src/api/api_messages.h"
#include "content/nw/src/api/id_weak_map.h"
#include "content/nw/src/api/object_life_monitor.h"
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

  if (current_directory.empty())
    return file_util::AbsolutePath(file_path);

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
  // Since we can only return FunctionTemplate here, we have no choice but to
  // return a helper function to return the real constructor.
  else if (name->Equals(v8::String::New("GetIDWeakMapConstructor")))
    return v8::FunctionTemplate::New(GetIDWeakMapConstructor);
  else if (name->Equals(v8::String::New("GetConstructorName")))
    return v8::FunctionTemplate::New(GetConstructorName);
  else if (name->Equals(v8::String::New("GetHiddenValue")))
    return v8::FunctionTemplate::New(GetHiddenValue);
  else if (name->Equals(v8::String::New("SetHiddenValue")))
    return v8::FunctionTemplate::New(SetHiddenValue);
  else if (name->Equals(v8::String::New("SetDestructor")))
    return v8::FunctionTemplate::New(SetDestructor);
  else if (name->Equals(v8::String::New("GetNextObjectId")))
    return v8::FunctionTemplate::New(GetNextObjectId);
  else if (name->Equals(v8::String::New("GetAbsolutePath")))
    return v8::FunctionTemplate::New(GetAbsolutePath);
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

  return scope.Close(NwGui);
}

// static
v8::Handle<v8::Value>
DispatcherBindings::GetIDWeakMapConstructor(const v8::Arguments& args) {
  return IDWeakMap::GetContructor();
}

// static
v8::Handle<v8::Value>
DispatcherBindings::GetNextObjectId(const v8::Arguments& args) {
  static int next_object_id = 1;
  return v8::Integer::New(next_object_id++);
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
DispatcherBindings::GetHiddenValue(const v8::Arguments& args) {
  return args.This()->GetHiddenValue(args[0]->ToString());
}

// static
v8::Handle<v8::Value>
DispatcherBindings::SetHiddenValue(const v8::Arguments& args) {
  args.This()->SetHiddenValue(args[0]->ToString(), args[1]);
  return v8::Undefined();
}

// static
v8::Handle<v8::Value>
DispatcherBindings::GetConstructorName(const v8::Arguments& args) {
  return args.This()->GetConstructorName();
}

// static
v8::Handle<v8::Value>
DispatcherBindings::SetDestructor(const v8::Arguments& args) {
  ObjectLifeMonitor::BindTo(args.This(), args[0]);
  return v8::Undefined();
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

// static
v8::Handle<v8::Value> DispatcherBindings::CallObjectMethodSync(
    const v8::Arguments& args) {
  if (args.Length() < 4) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "CallObjectMethodSync requries 4 arguments")));
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
            "Unable to convert 'args' passed to CallObjectMethodSync")));
  }

  RenderView* render_view = GetCurrentRenderView();
  if (!render_view) {
    return v8::ThrowException(v8::Exception::Error(v8::String::New(
            "Unable to get render view in CallObjectMethodSync")));
  }

  base::ListValue result;
  render_view->Send(new ShellViewHostMsg_Call_Object_Method_Sync(
        render_view->GetRoutingID(),
        object_id,
        type,
        method,
        *static_cast<base::ListValue*>(value_args.get()),
        &result));
  return converter->ToV8Value(&result, v8::Context::GetCurrent());
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

}  // namespace api
