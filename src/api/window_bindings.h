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

#ifndef CONTENT_NW_SRC_API_WINDOW_BINDINGS_H_
#define CONTENT_NW_SRC_API_WINDOW_BINDINGS_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "v8/include/v8.h"

namespace api {

class WindowBindings : public v8::Extension {
 public:
  WindowBindings();
  virtual ~WindowBindings();

  // v8::Extension implementation.
  virtual v8::Handle<v8::FunctionTemplate>
      GetNativeFunction(v8::Handle<v8::String> name) OVERRIDE;
  
 private:
  // Tell browser to bind a js object to Shell.
  static v8::Handle<v8::Value> BindToShell(const v8::Arguments& args);

  // Call method of an object in browser.
  static v8::Handle<v8::Value> CallObjectMethod(const v8::Arguments& args);

  // Call method of an object in browser synchrounously.
  static v8::Handle<v8::Value> CallObjectMethodSync(const v8::Arguments& args);

  // Get the window object of current render view.
  static v8::Handle<v8::Value> GetWindowObject(const v8::Arguments& args);

  DISALLOW_COPY_AND_ASSIGN(WindowBindings);
};

}  // namespace api

#endif  // CONTENT_NW_SRC_API_WINDOW_BINDINGS_H_
