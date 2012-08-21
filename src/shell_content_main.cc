// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "shell_content_main.h"

#include "content/public/app/content_main.h"
#include "shell_main_delegate.h"
#include "third_party/node/src/node.h"

#if defined(OS_MACOSX)
int ContentMain(int argc,
                const char** argv) {
  node::SetupUv(1, (char**)argv);
  content::ShellMainDelegate delegate;
  return content::ContentMain(argc, argv, &delegate);
}
#endif  // OS_MACOSX
