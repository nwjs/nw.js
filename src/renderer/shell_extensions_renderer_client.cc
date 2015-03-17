// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "extensions/shell/renderer/shell_extensions_renderer_client.h"

namespace extensions {

ShellExtensionsRendererClient::ShellExtensionsRendererClient() {
}

ShellExtensionsRendererClient::~ShellExtensionsRendererClient() {
}

bool ShellExtensionsRendererClient::IsIncognitoProcess() const {
  // app_shell doesn't support off-the-record contexts.
  return false;
}

int ShellExtensionsRendererClient::GetLowestIsolatedWorldId() const {
  // app_shell doesn't need to reserve world IDs for anything other than
  // extensions, so we always return 1. Note that 0 is reserved for the global
  // world.
  return 1;
}

}  // namespace extensions
