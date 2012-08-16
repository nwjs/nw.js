// Copyright (c) 2012 Intel Corp
// Copyright (c) 2012 The Chromium Embedded Framework Authors
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

// This file is shared by nw and cef_unittests so don't include using
// a qualified path.
#include "client_switches.h"  // NOLINT(build/include)

namespace nw {

const char kmMain[]      = "main";
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

const char kVersion[] = "version";
const char kUrl[] = "url";
const char kNoToolbar[] = "no-toolbar";

// CefSettings attributes.
const char kMultiThreadedMessageLoop[] = "multi-threaded-message-loop";
const char kCachePath[] = "cache-path";

// CefBrowserSettings attributes.
const char kRemoteFontsDisabled[] = "remote-fonts-disabled";
const char kDefaultEncoding[] = "default-encoding";
const char kEncodingDetectorEnabled[] = "encoding-detector-enabled";
const char kJavascriptDisabled[] = "javascript-disabled";
const char kJavascriptOpenWindowsDisallowed[] =
    "javascript-open-windows-disallowed";
const char kJavascriptCloseWindowsDisallowed[] =
    "javascript-close-windows-disallowed";
const char kJavascriptAccessClipboardDisallowed[] =
    "javascript-access-clipboard-disallowed";
const char kDomPasteDisabled[] = "dom-paste-disabled";
const char kCaretBrowsingEnabled[] = "caret-browsing-enabled";
const char kJavaDisabled[] = "java-disabled";
const char kPluginsDisabled[] = "plugins-disabled";
const char kUniversalAccessFromFileUrlsAllowed[] =
    "universal-access-from-file-urls-allowed";
const char kFileAccessFromFileUrlsAllowed[] =
    "file-access-from-file-urls-allowed";
const char kWebSecurityDisabled[] = "web-security-disabled";
const char kXssAuditorEnabled[] = "xss-auditor-enabled";
const char kImageLoadingDisabled[] = "image-load-disabled";
const char kShrinkStandaloneImagesToFit[] = "shrink-standalone-images-to-fit";
const char kSiteSpecificQuirksDisabled[] = "site-specific-quirks-disabled";
const char kTextAreaResizeDisabled[] = "text-area-resize-disabled";
const char kPageCacheDisabled[] = "page-cache-disabled";
const char kTabToLinksDisabled[] = "tab-to-links-disabled";
const char kHyperlinkAuditingDisabled[] = "hyperlink-auditing-disabled";
const char kUserStyleSheetEnabled[] = "user-style-sheet-enabled";
const char kUserStyleSheetLocation[] = "user-style-sheet-location";
const char kAuthorAndUserStylesDisabled[] = "author-and-user-styles-disabled";
const char kLocalStorageDisabled[] = "local-storage-disabled";
const char kDatabasesDisabled[] = "databases-disabled";
const char kApplicationCacheDisabled[] = "application-cache-disabled";
const char kWebglDisabled[] = "webgl-disabled";
const char kAcceleratedCompositingDisabled[] =
    "accelerated-compositing-disabled";
const char kAcceleratedLayersDisabled[] = "accelerated-layers-disabled";
const char kAcceleratedVideoDisabled[] = "accelerated-video-disabled";
const char kAcceledated2dCanvasDisabled[] = "accelerated-2d-canvas-disabled";
const char kAcceleratedPaintingEnabled[] = "accelerated-painting-enabled";
const char kAcceleratedFiltersEnabled[] = "accelerated-filters-enabled";
const char kAcceleratedPluginsDisabled[] = "accelerated-plugins-disabled";
const char kDeveloperToolsDisabled[] = "developer-tools-disabled";
const char kFullscreenEnabled[] = "fullscreen-enabled";

// Other attributes.
const char kProxyType[] = "proxy-type";
const char kProxyType_Direct[] = "direct";
const char kProxyType_Named[] = "named";
const char kProxyType_Pac[] = "pac";
const char kProxyConfig[] = "proxy-config";

}  // namespace nw
