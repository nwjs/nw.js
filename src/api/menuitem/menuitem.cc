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

#include "content/nw/src/api/menuitem/menuitem.h"

#include <base/logging.h>
#include <string.h>

using namespace v8;

namespace {

api::MenuItem::MenuItemType TypeStringToType(Handle<Value> type_str) {
  String::Utf8Value utf8_value(type_str);
  if (!strcmp("checkbox", *utf8_value))
    return api::MenuItem::CHECKBOX;
  else if (!strcmp("separator", *utf8_value))
    return api::MenuItem::SEPARATOR;
  else
    return api::MenuItem::NORMAL;
}

Handle<Value> TypeToTypeString(api::MenuItem::MenuItemType type) {
  switch (type) {
    case api::MenuItem::CHECKBOX:
      return String::New("checkbox");
    case api::MenuItem::SEPARATOR:
      return String::New("separator");
    case api::MenuItem::NORMAL:
      return String::New("normal");
  }

  NOTREACHED() << "Invalid MenuItem type.";
  return Undefined();
}

void on_click_callback(uv_async_t* handle, int status) {
  ((api::MenuItem*)(handle->data))->OnClick();
}

}  // namespace

namespace api {

Persistent<Function> MenuItem::constructor_;

MenuItem::CreationOption::CreationOption()
    : type(MenuItem::NORMAL),
      enabled(true),
      checked(false) {
}

// static
void MenuItem::Init(Handle<Object> target) {
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  tpl->SetClassName(String::NewSymbol("MenuItem"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->InstanceTemplate()->SetAccessor(
      String::New("type"), PropertyGetter);
  tpl->InstanceTemplate()->SetAccessor(
      String::New("label"), PropertyGetter, PropertySetter);
  tpl->InstanceTemplate()->SetAccessor(
      String::New("icon"), PropertyGetter, PropertySetter);
  tpl->InstanceTemplate()->SetAccessor(
      String::New("tooltip"), PropertyGetter, PropertySetter);
  tpl->InstanceTemplate()->SetAccessor(
      String::New("enabled"), PropertyGetter, PropertySetter);
  tpl->InstanceTemplate()->SetAccessor(
      String::New("checked"), PropertyGetter, PropertySetter);
  tpl->InstanceTemplate()->SetAccessor(
      String::New("submenu"), PropertyGetter, PropertySetter);

  constructor_ = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("MenuItem"), constructor_);
}

void MenuItem::OnClickFromUI() {
  uv_async_send(&click_event_);
}

void MenuItem::OnClick() {
  HandleScope scope;

  // Call the 'click' function of MenuItem object
  Handle<Value> click = handle_->Get(String::New("click"));
  if (click->IsFunction()) {
    Handle<Value> argv[1];
    Handle<Function>::Cast(click)->Call(handle_, 0, argv);
  }
}

// static
Handle<Value> MenuItem::New(const Arguments& args) {
  HandleScope scope;

  // Check if called from C++
  if (args.Length() == 1 && args[0]->IsExternal()) {
    return args.This();
  }

  CreationOption option;
  if (args.Length() < 1 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(
          String::New("Require first argument as option in contructor.")));
  }

  Handle<Object> v8op = args[0]->ToObject();
  if (v8op->Has(String::New("label")))
    option.label = *String::Utf8Value(v8op->Get(String::New("label")));
  if (v8op->Has(String::New("icon")))
    option.icon = *String::Utf8Value(v8op->Get(String::New("icon")));
  if (v8op->Has(String::New("tooltip")))
    option.tooltip = *String::Utf8Value(v8op->Get(String::New("tooltip")));
  if (v8op->Has(String::New("type")))
    option.type = TypeStringToType(v8op->Get(String::New("type")));
  if (v8op->Has(String::New("enabled")))
    option.enabled = v8op->Get(String::New("enabled"))->BooleanValue();
  if (v8op->Has(String::New("checked")))
    option.checked = v8op->Get(String::New("checked"))->BooleanValue();

  // Wrap C++ object in V8 object
  MenuItem* obj = new MenuItem(option);
  obj->option_ = option;
  obj->Wrap(args.This());

  // Async events
  obj->click_event_.data = obj;
  uv_async_init(uv_default_loop(), &obj->click_event_, on_click_callback);

  // this.click = option.click
  Handle<Value> callback = v8op->Get(String::New("click"));
  if (callback->IsFunction())
    args.This()->Set(String::New("click"), callback);

  // this.submenu = option.submenu
  Handle<Value> submenu = v8op->Get(String::New("submenu"));
  if (submenu->IsObject())
    args.This()->Set(String::New("submenu"), submenu);

  return args.This();
}

// static
Handle<Value> MenuItem::PropertyGetter(Local<String> property,
                                       const AccessorInfo& info) {
  MenuItem* obj = ObjectWrap::Unwrap<MenuItem>(info.This());

  String::Utf8Value key(property);
  if (!strcmp(*key, "label"))
    return String::New(obj->GetLabel().c_str());
  else if (!strcmp(*key, "icon"))
    return String::New(obj->option_.icon.c_str());
  else if (!strcmp(*key, "tooltip"))
    return String::New(obj->GetTooltip().c_str());
  else if (!strcmp(*key, "enabled"))
    return Boolean::New(obj->GetEnabled());
  else if (!strcmp(*key, "checked"))
    return Boolean::New(obj->GetChecked());
  else if (!strcmp(*key, "type"))
    return TypeToTypeString(obj->option_.type);
  else if (!strcmp(*key, "submenu"))
    return info.This()->GetHiddenValue(String::New("realsubmenu"));

  return Undefined();
}

// static
void MenuItem::PropertySetter(Local<String> property,
                                       Local<Value> value,
                                       const AccessorInfo& info) {
  HandleScope scope;
  MenuItem* obj = ObjectWrap::Unwrap<MenuItem>(info.This());

  String::Utf8Value key(property);
  if (!strcmp(*key, "label")) {
    obj->SetLabel(*String::Utf8Value(value));
  } else if (!strcmp(*key, "icon")) {
    if (obj->option_.type == SEPARATOR)
      return;

    obj->option_.icon = *String::Utf8Value(value);
    obj->SetIcon(*String::Utf8Value(value));
  } else if (!strcmp(*key, "tooltip")) {
    obj->SetTooltip(*String::Utf8Value(value));
  } else if (!strcmp(*key, "enabled")) {
    obj->SetEnabled(value->BooleanValue());
  } else if (!strcmp(*key, "checked")) {
    obj->SetChecked(value->BooleanValue());
  } else if (!strcmp(*key, "submenu")) {
    info.This()->SetHiddenValue(String::New("realsubmenu"), value);
    obj->SetSubmenu(ObjectWrap::Unwrap<Menu>(value->ToObject()));
  }
}

}  // namespace api
