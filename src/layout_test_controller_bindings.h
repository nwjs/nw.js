// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_LAYOUT_TEST_CONTROLLER_BINDINGS_H_
#define CONTENT_SHELL_LAYOUT_TEST_CONTROLLER_BINDINGS_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "v8/include/v8.h"

namespace content {

class LayoutTestControllerBindings : public v8::Extension {
 public:
  LayoutTestControllerBindings();
  virtual ~LayoutTestControllerBindings();

  // v8::Extension implementation.
  virtual v8::Handle<v8::FunctionTemplate>
      GetNativeFunction(v8::Handle<v8::String> name) OVERRIDE;

 private:
  DISALLOW_COPY_AND_ASSIGN(LayoutTestControllerBindings);
};

}  // namespace content

#endif  // CONTENT_SHELL_LAYOUT_TEST_CONTROLLER_BINDINGS_H_
