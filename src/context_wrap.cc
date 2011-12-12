// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <assert.h>

#include "context_wrap.h"

using v8::Context;
using v8::Script;
using v8::Value;
using v8::Handle;
using v8::HandleScope;
using v8::Object;
using v8::Arguments;
using v8::ThrowException;
using v8::TryCatch;
using v8::String;
using v8::Exception;
using v8::Local;
using v8::Array;
using v8::Persistent;
using v8::Integer;
using v8::Function;
using v8::FunctionTemplate;

namespace nwebkit {

Persistent<FunctionTemplate> ContextWrap::constructor_template;

void ContextWrap::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(ContextWrap::New);
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(2);
  constructor_template->SetClassName(String::NewSymbol("Context"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template,
                            "onContextCreationFromWebKit",
                            ContextWrap::OnContextCreationFromWebKit);

  NODE_SET_METHOD(constructor_template,
		  "onContextCreationFromWebKit",
		  ContextWrap::OnContextCreationFromWebKit);

  target->Set(String::NewSymbol("Context"),
              constructor_template->GetFunction());
}


bool ContextWrap::InstanceOf(Handle<Value> value) {
  return !value.IsEmpty() && constructor_template->HasInstance(value);
}


Handle<Value> ContextWrap::New(const Arguments& args) {
  HandleScope scope;

  ContextWrap *t = new ContextWrap();
  t->Wrap(args.This());

  return args.This();
}


ContextWrap::ContextWrap() : ObjectWrap() {
  context_valid = false;
}


ContextWrap::~ContextWrap() {
}


Local<Object> ContextWrap::NewInstance() {
  Local<Object> wrapper = constructor_template->GetFunction()->NewInstance();
  return wrapper;
}


Persistent<Context> ContextWrap::GetV8Context() {
  return context_;
}

Handle<Value> ContextWrap::OnContextCreationFromWebKit(const Arguments& args) {
  HandleScope scope;

  if (args.Length() > 0) {
    Local<Object> sandbox = args[0]->ToObject();
  }

  ContextWrap* context_wrap = ObjectWrap::Unwrap<ContextWrap> (args.Holder());
  v8::Context* p = static_cast<v8::Context*>(args.Holder()->GetPointerFromInternalField (1));
  context_wrap->context_ = p;
  context_wrap->context_valid = true;

  return v8::Undefined();
}

}
