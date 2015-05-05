// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/nw_custom_bindings.h"

#include <string>

#include "base/bind.h"
#include "extensions/renderer/script_context.h"
#include "v8/include/v8.h"

namespace extensions {

NWCustomBindings::NWCustomBindings(ScriptContext* context)
    : ObjectBackedNativeHandler(context) {
  RouteFunction("crashRenderer",
                base::Bind(&NWCustomBindings::CrashRenderer,
                           base::Unretained(this)));
}

// Attach an event name to an object.
void NWCustomBindings::CrashRenderer(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  LOG(ERROR) << "crashing renderer";
  int* ptr = nullptr;
  *ptr = 1;
}

}  // namespace extensions
