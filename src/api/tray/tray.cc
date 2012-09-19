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

#include "content/nw/src/api/tray/tray.h"

#include "content/nw/src/api/menu/menu.h"

namespace api {

using namespace v8;

// static
void Tray::Init(Handle<Object> target) {
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  tpl->SetClassName(String::NewSymbol("Tray"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->InstanceTemplate()->SetAccessor(
      String::New("title"), PropertyGetter, PropertySetter);
  tpl->InstanceTemplate()->SetAccessor(
      String::New("icon"), PropertyGetter, PropertySetter);
  tpl->InstanceTemplate()->SetAccessor(
      String::New("menu"), PropertyGetter, PropertySetter);
  tpl->PrototypeTemplate()->Set(String::NewSymbol("remove"),
      FunctionTemplate::New(Remove)->GetFunction());

  Persistent<Function> constructor = 
      Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Tray"), constructor);
}

// static
Handle<Value> Tray::New(const Arguments& args) {
  HandleScope scope;

  CreationOption option;
  if (args.Length() < 1 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(
          String::New("Require first argument as option in contructor.")));
  }

  Handle<Object> v8op = args[0]->ToObject();
  if (!v8op->Has(String::New("title")) &&
      !v8op->Has(String::New("icon"))) {
    return ThrowException(Exception::Error(
          String::New("Must set 'icon' or 'title' in option.")));
  }

  if (v8op->Has(String::New("icon")))
    option.icon = *String::Utf8Value(v8op->Get(String::New("icon")));
  if (v8op->Has(String::New("title")))
    option.title = *String::Utf8Value(v8op->Get(String::New("title")));

  Tray* obj = new Tray(option);
  obj->Wrap(args.This());

  return args.This();
}

// static
v8::Handle<v8::Value> Tray::Remove(const v8::Arguments& args) {
  Tray* obj = ObjectWrap::Unwrap<Tray>(args.This());
  obj->Remove();

  return Undefined();
}

// static
Handle<Value> Tray::PropertyGetter(Local<String> property,
                                   const AccessorInfo& info) {
  Tray* obj = ObjectWrap::Unwrap<Tray>(info.This());

  String::Utf8Value key(property);
  if (!strcmp(*key, "title")) {
    return String::New(obj->GetTitle().c_str());
  } else if (!strcmp(*key, "icon")) {
    return String::New(obj->GetIcon().c_str());
  } else if (!strcmp(*key, "menu")) {
    return info.This()->GetHiddenValue(String::New("realmenu"));
  }

  return Undefined();
}

// static
void Tray::PropertySetter(Local<String> property,
                          Local<Value> value,
                          const AccessorInfo& info) {
  HandleScope scope;
  Tray* obj = ObjectWrap::Unwrap<Tray>(info.This());

  String::Utf8Value key(property);
  if (!strcmp(*key, "title")) {
    obj->SetTitle(*String::Utf8Value(value));
  } else if (!strcmp(*key, "icon")) {
    obj->SetIcon(*String::Utf8Value(value));
  } else if (!strcmp(*key, "menu")) {
    info.This()->SetHiddenValue(String::New("realmenu"), value);
    obj->SetMenu(ObjectWrap::Unwrap<Menu>(value->ToObject()));
  }
}

}  // namespace api
