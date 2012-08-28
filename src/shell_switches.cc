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

#include "content/nw/src/shell_switches.h"

namespace switches {

// Check whether all system dependencies for running layout tests are met.
const char kCheckLayoutTestSysDeps[] = "check-layout-test-sys-deps";

// Tells Content Shell that it's running as a content_browsertest.
const char kContentBrowserTest[] = "browser-test";

// Makes Content Shell use the given path for its data directory.
const char kContentShellDataPath[] = "data-path";

// Enable developer tools
const char kDeveloper[] = "developer";

// Request pages to be dumped as text once they finished loading.
const char kDumpRenderTree[] = "dump-render-tree";

// Disables the timeout for layout tests.
const char kNoTimeout[] = "no-timeout";

// Display no toolbar
const char kNoToolbar[] = "no-toolbar";

// Show version and quit
const char kVersion[] = "version";

// Open specified url
const char kUrl[] = "url";

const char kmMain[]      = "main";
const char kmName[]      = "name";
const char kmWebkit[]    = "webkit";
const char kmNodejs[]    = "nodejs";
const char kmRoot[]      = "root";
const char kmWindow[]    = "window";
const char kmToolbar[]   = "toolbar";
const char kmIcon[]      = "icon";
const char kmWidth[]     = "width";
const char kmHeight[]    = "height";
const char kmX[]         = "x";
const char kmY[]         = "y";
const char kmResizable[] = "resizable";
const char kmPosition[]  = "position";
const char kmMinWidth[]  = "min_width";
const char kmMinHeight[] = "min_height";
const char kmMaxWidth[]  = "max_width";
const char kmMaxHeight[] = "max_height";

}  // namespace switches
