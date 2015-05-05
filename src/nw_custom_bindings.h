// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EXTENSIONS_RENDERER_NW_CUSTOM_BINDINGS_H_
#define EXTENSIONS_RENDERER_NW_CUSTOM_BINDINGS_H_

#include "extensions/renderer/object_backed_native_handler.h"

namespace extensions {
class ScriptContext;

// Implements custom bindings for document-level operations.
class NWCustomBindings : public ObjectBackedNativeHandler {
 public:
  NWCustomBindings(ScriptContext* context);

 private:
  void CrashRenderer(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}  // namespace extensions

#endif  // EXTENSIONS_RENDERER_DOCUMENT_CUSTOM_BINDINGS_H_
