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

#include "content/nw/src/api/dispatcher.h"

#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/api/menuitem/menuitem.h"
#include "content/nw/src/api/tray/tray.h"

namespace api {

Dispatcher::Dispatcher() {
}

Dispatcher::~Dispatcher() {
  gui_.Dispose();
}

void Dispatcher::DidCreateScriptContext(
    WebKit::WebFrame* frame,
    v8::Handle<v8::Context> context,
    int extension_group,
    int world_id) {
  v8::HandleScope scope;

  // Create our gui bindings
  if (gui_.IsEmpty()) {
    gui_ = v8::Persistent<v8::Object>::New(v8::Object::New());
    MenuItem::Init(gui_);
    Menu::Init(gui_);
    Tray::Init(gui_);
  }

  // And attach it to the global
  v8::Handle<v8::Value> process = context->Global()->Get(
      v8::String::New("process"));
  if (process->IsObject()) {
    process->ToObject()->Set(v8::String::New("gui"), gui_);
  }
}

void Dispatcher::WillReleaseScriptContext(
    WebKit::WebFrame* frame,
    v8::Handle<v8::Context> context,
    int world_id) {
}

}  // namespace api
