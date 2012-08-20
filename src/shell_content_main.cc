// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/shell_content_main.h"

#include "content/public/app/content_main.h"
#include "content/shell/shell_main_delegate.h"

#if defined(OS_MACOSX)
int ContentMain(int argc,
                const char** argv) {
  content::ShellMainDelegate delegate;
  return content::ContentMain(argc, argv, &delegate);
}
#endif  // OS_MACOSX
