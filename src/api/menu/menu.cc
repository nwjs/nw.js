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

#include "content/nw/src/api/menu/menu.h"

#include "base/bind.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/renderer/render_view.h"
#include "content/nw/src/api/menuitem/menuitem.h"
#include "content/nw/src/api/utils.h"

using namespace v8;

namespace api {

Persistent<Function> Menu::constructor_;

// static
void Menu::Init(Handle<Object> target) {
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  tpl->SetClassName(String::NewSymbol("Menu"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->InstanceTemplate()->SetAccessor(
      String::New("title"), PropertyGetter, PropertySetter);
  tpl->InstanceTemplate()->SetAccessor(
      String::New("length"), PropertyGetter);
  tpl->InstanceTemplate()->SetIndexedPropertyHandler(
      MenuItemGetter);
  tpl->PrototypeTemplate()->Set(String::NewSymbol("append"),
      FunctionTemplate::New(Append)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("insert"),
      FunctionTemplate::New(Insert)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("remove"),
      FunctionTemplate::New(Remove)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("removeAt"),
      FunctionTemplate::New(RemoveAt)->GetFunction());
  tpl->PrototypeTemplate()->Set(String::NewSymbol("popup"),
      FunctionTemplate::New(Popup)->GetFunction());

  constructor_ = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Menu"), constructor_);
}

// static
Handle<Value> Menu::New(const Arguments& args) {
  HandleScope scope;

  CreationOption option;
  if (args.Length() < 1 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(
          String::New("Require first argument as option in contructor.")));
  }

  Handle<Object> v8op = args[0]->ToObject();
  if (!v8op->Has(String::New("title")))
    return ThrowException(Exception::Error(
          String::New("Option 'title' is required for Menu.")));

  option.title = *String::Utf8Value(v8op->Get(String::New("title")));

  Menu* obj = new Menu(option);
  obj->Wrap(args.This());

  // Save menu items in 'items'
  args.This()->SetHiddenValue(String::New("items"), Array::New());

  return args.This();
}

// static
Handle<Value> Menu::Append(const Arguments& args) {
  HandleScope scope;
  Menu* obj = ObjectWrap::Unwrap<Menu>(args.This());

  if (args.Length() < 1)
    return ThrowException(Exception::Error(
          String::New("Menu.Append requires one argument.")));

  obj->Append(ObjectWrap::Unwrap<MenuItem>(args[0]->ToObject()));

  // menu.items.push(args[0]);
  Handle<Value> argv[] = { args[0] };
  CallMethod(args.This()->GetHiddenValue(String::New("items")),
             "push", 1, argv);

  return args.This();
}

// static
Handle<Value> Menu::Insert(const Arguments& args) {
  HandleScope scope;
  Menu* obj = ObjectWrap::Unwrap<Menu>(args.This());

  if (args.Length() < 2)
    return ThrowException(Exception::Error(
          String::New("Menu.Insert requires two arguments.")));

  if (!args[0]->IsObject() || !args[1]->IsNumber())
    return ThrowException(Exception::Error(
          String::New("Invalid arguments for Menu.Insert")));

  obj->Insert(ObjectWrap::Unwrap<MenuItem>(args[0]->ToObject()),
              args[1]->IntegerValue());

  // menu.items.splice(args[1], 0, args[0]);
  Handle<Value> argv[] = { args[1], Integer::New(0), args[0] };
  CallMethod(args.This()->GetHiddenValue(String::New("items")),
             "splice", 3, argv);

  return args.This();
}

// static
Handle<Value> Menu::Remove(const Arguments& args) {
  HandleScope scope;
  Menu* obj = ObjectWrap::Unwrap<Menu>(args.This());

  if (args.Length() < 1)
    return ThrowException(Exception::Error(
          String::New("Menu.Remove requires one argument.")));

  obj->Remove(ObjectWrap::Unwrap<MenuItem>(args[0]->ToObject()));

  // index = menu.items.indexOf(args[0])
  Handle<Value> argv[] = { args[0] };
  Handle<Value> index = CallMethod(
      args.This()->GetHiddenValue(String::New("items")),
      "indexOf", 1, argv);

  // menu.items.splice(index, 1);
  Handle<Value> argv2[] = { index, Integer::New(1) };
  CallMethod(args.This()->GetHiddenValue(String::New("items")),
             "splice", 2, argv2);

  return args.This();
}

// static
Handle<Value> Menu::RemoveAt(const Arguments& args) {
  HandleScope scope;
  Menu* obj = ObjectWrap::Unwrap<Menu>(args.This());

  if (args.Length() < 1)
    return ThrowException(Exception::Error(
          String::New("Menu.RemoveAt requires one argument.")));

  obj->Remove(args[0]->IntegerValue());

  // menu.items.splice(args[0], 1);
  Handle<Value> argv[] = { args[0], Integer::New(1) };
  CallMethod(args.This()->GetHiddenValue(String::New("items")),
             "splice", 2, argv);

  return args.This();
}

// static
Handle<Value> Menu::Popup(const Arguments& args) {
  HandleScope scope;
  Menu* obj = ObjectWrap::Unwrap<Menu>(args.This());

  if (args.Length() < 2 || !args[0]->IsNumber() || !args[1]->IsNumber())
    return ThrowException(Exception::Error(
          String::New("Wrong arguments for Menu.Popup(x, y)")));

  int x = args[0]->IntegerValue();
  int y = args[1]->IntegerValue();
  int render_view_id = GetCurrentRenderViewInRenderer();
  content::BrowserThread::PostTask(
      content::BrowserThread::UI,
      FROM_HERE,
      base::Bind(&Menu::PopupFromUI,
                 base::Unretained(obj),
                 x, y,
                 render_view_id));

  return args.This();
}

void Menu::PopupFromUI(int x, int y, int render_view_id) {
  content::Shell* shell = GetShellFromRenderViewInUI(render_view_id);
  Popup(x, y, shell);
}

// static
Handle<Value> Menu::PropertyGetter(Local<String> property,
                                   const AccessorInfo& info) {
  Menu* obj = ObjectWrap::Unwrap<Menu>(info.This());

  String::Utf8Value key(property);
  if (!strcmp(*key, "title"))
    return String::New(obj->GetTitle().c_str());
  else if (!strcmp(*key, "length"))
    return Integer::New(Handle<Array>::Cast(
        info.This()->GetHiddenValue(String::New("items")))->Length());

  return Undefined();
}

// static
void Menu::PropertySetter(Local<String> property,
                          Local<Value> value,
                          const AccessorInfo& info) {
  HandleScope scope;
  Menu* obj = ObjectWrap::Unwrap<Menu>(info.This());

  String::Utf8Value key(property);
  if (!strcmp(*key, "title"))
    obj->SetTitle(*String::Utf8Value(value));
}

// static
Handle<Value> Menu::MenuItemGetter(uint32_t index,
                                   const v8::AccessorInfo& info) {
  return info.This()->GetHiddenValue(String::New("items"))->
      ToObject()->Get(index);
}

}  // namespace api
