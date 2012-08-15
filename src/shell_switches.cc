// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/shell_switches.h"

namespace switches {

// Check whether all system dependencies for running layout tests are met.
const char kCheckLayoutTestSysDeps[] = "check-layout-test-sys-deps";

// Tells Content Shell that it's running as a content_browsertest.
const char kContentBrowserTest[] = "browser-test";

// Request pages to be dumped as text once they finished loading.
const char kDumpRenderTree[] = "dump-render-tree";

}  // namespace switches
