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

#ifndef CONTENT_NW_SRC_API_DISPATCHER_BINDINGS_H_
#define CONTENT_NW_SRC_API_DISPATCHER_BINDINGS_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "v8/include/v8.h"

namespace nwapi {

class DispatcherBindings : public v8::Extension {
 public:
  DispatcherBindings();
  virtual ~DispatcherBindings();

  // v8::Extension implementation.
  virtual v8::Handle<v8::FunctionTemplate>
      GetNativeFunctionTemplate(
                        v8::Isolate* isolate,
                        v8::Handle<v8::String> name) OVERRIDE;

 private:
  // Helper functions for bindings.
  static void RequireNwGui(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void GetAbsolutePath(const v8::FunctionCallbackInfo<v8::Value>& args);

  // Get Shell's corresponding js object's id.
  static void GetShellIdForCurrentContext(
      const v8::FunctionCallbackInfo<v8::Value>& args);

  // Get current routing id.
  static void GetRoutingIDForCurrentContext(
      const v8::FunctionCallbackInfo<v8::Value>& args);

  // Create new shell and returns its routing id.
  static void CreateShell(const v8::FunctionCallbackInfo<v8::Value>& args);

  // Remote objects.
  static void AllocateId(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void AllocateObject(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void DeallocateObject(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void CallObjectMethod(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void CallObjectMethodSync(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void CallStaticMethod(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void CallStaticMethodSync(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void CrashRenderer(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SetCrashDumpDir(const v8::FunctionCallbackInfo<v8::Value>& args);
#if defined(OS_MACOSX)
  static void InitMsgIDMap();
  static void GetNSStringWithFixup(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void GetNSStringFWithFixup(const v8::FunctionCallbackInfo<v8::Value>& args);
#endif

  DISALLOW_COPY_AND_ASSIGN(DispatcherBindings);
};

}  // namespace nwapi

#endif  // CONTENT_NW_SRC_API_DISPATCHER_BINDINGS_H_
