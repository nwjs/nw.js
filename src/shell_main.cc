// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/public/app/content_main.h"

#include "content/shell/shell_main_delegate.h"
#include "sandbox/win/src/sandbox_types.h"

#if defined(OS_WIN)
#include "content/public/app/startup_helper_win.h"
#endif

#if defined(OS_MACOSX)
#include "content/shell/shell_content_main.h"
#endif

#if defined(OS_WIN)

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, wchar_t*, int) {
  sandbox::SandboxInterfaceInfo sandbox_info = {0};
  content::InitializeSandboxInfo(&sandbox_info);
  content::ShellMainDelegate delegate;
  return content::ContentMain(instance, &sandbox_info, &delegate);
}

#else

int main(int argc, const char** argv) {
#if defined(OS_MACOSX)
  // Do the delegate work in shell_content_main to avoid having to export the
  // delegate types.
  return ::ContentMain(argc, argv);
#else
  content::ShellMainDelegate delegate;
  return content::ContentMain(argc, argv, &delegate);
#endif  // OS_MACOSX
}

#endif  // OS_POSIX
