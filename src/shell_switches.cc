// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "shell_switches.h"

namespace switches {

// Check whether all system dependencies for running layout tests are met.
const char kCheckLayoutTestSysDeps[] = "check-layout-test-sys-deps";

// Tells Content Shell that it's running as a content_browsertest.
const char kContentBrowserTest[] = "browser-test";

// Request pages to be dumped as text once they finished loading.
const char kDumpRenderTree[] = "dump-render-tree";

// Display no toolbar
const char kNoToolbar[] = "no-toolbar";

// Show version and quit
const char kVersion[] = "version";

// Open specified url
const char kUrl[] = "url";

const char kmMain[]      = "main";
const char kmName[]      = "name";
const char kmWebkit[]    = "webkit";
const char kmRoot[]      = "root";
const char kmWindow[]    = "window";
const char kmToolbar[]   = "toolbar";
const char kmIcon[]      = "icon";
const char kmWidth[]     = "width";
const char kmHeight[]    = "height";
const char kmResizable[] = "resizable";
const char kmPosition[]  = "position";
const char kmMinWidth[]  = "min_width";
const char kmMinHeight[] = "min_height";
const char kmMaxWidth[]  = "max_width";
const char kmMaxHeight[] = "max_height";

}  // namespace switches
