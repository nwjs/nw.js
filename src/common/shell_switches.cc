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

#include "content/nw/src/common/shell_switches.h"

namespace switches {

// Makes Content Shell use the given path for its data directory.
const char kContentShellDataPath[] = "data-path";

// Enable developer tools
const char kDeveloper[] = "developer";

// Display no toolbar
const char kNoToolbar[] = "no-toolbar";

// Open specified url
const char kUrl[] = "url";

// Set current working directory.
const char kWorkingDirectory[] = "working-directory";

// Pass the main script to node.
const char kNodeMain[] = "node-main";

// snapshot file path
const char kSnapshot[] = "snapshot";
const char kDomStorageQuota[] = "ds-quota";
const char kNodejs[] = "nodejs";
const char kChromeExtension[] = "extension";
const char kVerifyContent[] = "verify-content";

const char kmMain[]   = "main";
const char kmName[]   = "name";
const char kmWebkit[] = "webkit";
const char kmWindow[] = "window";
const char kmChromiumArgs[] = "chromium-args";
const char kmJsFlags[] = "js-flags";

// Allows only one instance of the app.
const char kmSingleInstance[] = "single-instance";

const char kmTitle[]      = "title";
const char kmToolbar[]    = "toolbar";
const char kmIcon[]       = "icon";
const char kmFrame[]      = "frame";
const char kmShow[]       = "show";
const char kmPosition[]   = "position";
const char kmX[]          = "x";
const char kmY[]          = "y";
const char kmWidth[]      = "width";
const char kmHeight[]     = "height";
const char kmMinWidth[]   = "min_width";
const char kmMinHeight[]  = "min_height";
const char kmMaxWidth[]   = "max_width";
const char kmMaxHeight[]  = "max_height";
const char kmResizable[]  = "resizable";
const char kmAsDesktop[]  = "as_desktop";
const char kmFullscreen[] = "fullscreen";
const char kmInitialFocus[] = "focus";
const char kmTransparent[] = "transparent";
const char kmDisableTransparency[] = "disable-transparency";

// Make windows icon hide show or hide in taskbar.
const char kmShowInTaskbar[] = "show_in_taskbar";

// Start with the kiosk mode, see Opera's page for description:
// http://www.opera.com/support/mastering/kiosk/
const char kmKiosk[] = "kiosk";

// Make windows stays on the top of all other windows.
const char kmAlwaysOnTop[] = "always_on_top";

// Make window visible on all workspaces.
const char kmVisibleOnAllWorkspaces[] = "visible_on_all_workspaces";

// Whether we should support WebGL.
const char kmWebgl[] = "webgl";

// Whether to enable Java applets in web page.
const char kmJava[] = "java";

// Whether to enable third party NPAPI plugins.
const char kmPlugin[] = "plugin";

// Whether to enable page caches.
const char kmPageCache[] = "page-cache";

const char kmUserAgent[] = "user-agent";

// rules to turn on Node for remote pages
const char kmRemotePages[] = "node-remote";

const char kmNewInstance[] = "new-instance";
const char kmInjectJSDocStart[]  = "inject_js_start";
const char kmInjectJSDocEnd[]  = "inject_js_end";
const char kmInjectCSS[] = "inject-css";
const char kmAppUserModelID[] = "app_user_model_id";

#if defined(OS_WIN)
// Enable conversion from vector to raster for any page.
const char kPrintRaster[] = "print-raster";
#endif

const char kCrashDumpsDir[] = "crash-dumps-dir";
const char kEnableSpellChecking[] = "enable-spell-checking";
const char kDisableDevTools[] = "disable-devtools";
}  // namespace switches
