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

#include "content/nw/src/api/id_weak_map.h"

namespace api {

v8::Persistent<v8::Function> IDWeakMap::constructor_;

// static
void IDWeakMap::Init() {
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(New);
  tpl->SetClassName(v8::String::NewSymbol("IDWeakMap"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("set"),
      v8::FunctionTemplate::New(Set)->GetFunction());
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("get"),
      v8::FunctionTemplate::New(Get)->GetFunction());
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("has"),
      v8::FunctionTemplate::New(Has)->GetFunction());
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("delete"),
      v8::FunctionTemplate::New(Delete)->GetFunction());

  constructor_ = v8::Persistent<v8::Function>::New(tpl->GetFunction());
}

// static
v8::Handle<v8::Function> IDWeakMap::GetContructor() {
  if (constructor_.IsEmpty())
    Init();

  return constructor_;
}

// static
v8::Handle<v8::Value> IDWeakMap::New(const v8::Arguments& args) {
  IDWeakMap* obj = new IDWeakMap();
  obj->Wrap(args.This());
  return args.This();
}

// static
v8::Handle<v8::Value> IDWeakMap::Set(const v8::Arguments& args) {
  if (args.Length() < 2 || !args[0]->IsNumber() || !args[1]->IsObject()) {
    return v8::ThrowException(v8::Exception::Error(
          v8::String::New("Invalid arguments")));
  }

  IDWeakMap* obj = ObjectWrap::Unwrap<IDWeakMap>(args.This());

  int key = args[0]->IntegerValue();
  if (!obj->map_[key].IsEmpty())
    return v8::ThrowException(v8::Exception::Error(
          v8::String::New("Element already exists")));

  v8::Persistent<v8::Value> value = v8::Persistent<v8::Value>::New(args[1]);
  value->ToObject()->SetHiddenValue(
      v8::String::New("IDWeakMapKey"), v8::Integer::New(key));
  value.MakeWeak(obj, WeakCallback);
  obj->map_[key] = value;

  return v8::Undefined();
}

// static
v8::Handle<v8::Value> IDWeakMap::Get(const v8::Arguments& args) {
  if (args.Length() < 1 || !args[0]->IsNumber()) {
    return v8::ThrowException(v8::Exception::Error(
          v8::String::New("Invalid arguments")));
  }

  IDWeakMap* obj = ObjectWrap::Unwrap<IDWeakMap>(args.This());

  int key = args[0]->IntegerValue();
  v8::Persistent<v8::Value> value = obj->map_[key];
  if (value.IsEmpty())
    return v8::Null();

  return value;
}

// static
v8::Handle<v8::Value> IDWeakMap::Has(const v8::Arguments& args) {
  if (args.Length() < 1 || !args[0]->IsNumber()) {
    return v8::ThrowException(v8::Exception::Error(
          v8::String::New("Invalid arguments")));
  }

  IDWeakMap* obj = ObjectWrap::Unwrap<IDWeakMap>(args.This());

  int key = args[0]->IntegerValue();
  return v8::Boolean::New(!obj->map_[key].IsEmpty());
}

// static
v8::Handle<v8::Value> IDWeakMap::Delete(const v8::Arguments& args) {
  if (args.Length() < 1 || !args[0]->IsNumber()) {
    return v8::ThrowException(v8::Exception::Error(
          v8::String::New("Invalid arguments")));
  }

  IDWeakMap* obj = ObjectWrap::Unwrap<IDWeakMap>(args.This());

  int key = args[0]->IntegerValue();
  obj->Erase(key);
  return v8::Undefined();
}

IDWeakMap::IDWeakMap() {
}

IDWeakMap::~IDWeakMap() {
}

void IDWeakMap::Erase(int key) {
  v8::Persistent<v8::Value> value = map_[key];
  value.ClearWeak();
  value.Dispose();
  value.Clear();
  map_.erase(key);
}

// static
void IDWeakMap::WeakCallback(v8::Persistent<v8::Value> value, void *data) {
  v8::HandleScope scope;

  IDWeakMap* obj = static_cast<IDWeakMap*>(data);
  int key = value->ToObject()->GetHiddenValue(
      v8::String::New("IDWeakMapKey"))->IntegerValue();
  obj->Erase(key);
}

}  // namespace api
