// Copyright (c) 2012 Intel Corp
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

// Defines all of the command line switches used by nw.

#ifndef CEF_TESTS_CEFCLIENT_CEFCLIENT_SWITCHES_H_
#define CEF_TESTS_CEFCLIENT_CEFCLIENT_SWITCHES_H_
#pragma once

namespace nw {

// Manifest settings
extern const char kmMain[];
extern const char kmWebkit[];
extern const char kmRoot[];
extern const char kmWindow[];
extern const char kmToolbar[];
extern const char kmIcon[];
extern const char kmWidth[];
extern const char kmHeight[];
extern const char kmResizable[];
extern const char kmPosition[];
extern const char kmMinWidth[];
extern const char kmMinHeight[];
extern const char kmMaxWidth[];
extern const char kmMaxHeight[];

extern const char kVersion[];
extern const char kUrl[];
extern const char kNoToolbar[];

// CefSettings attributes.
extern const char kMultiThreadedMessageLoop[];
extern const char kCachePath[];

// CefBrowserSettings attributes.
extern const char kRemoteFontsDisabled[];
extern const char kDefaultEncoding[];
extern const char kEncodingDetectorEnabled[];
extern const char kJavascriptDisabled[];
extern const char kJavascriptOpenWindowsDisallowed[];
extern const char kJavascriptCloseWindowsDisallowed[];
extern const char kJavascriptAccessClipboardDisallowed[];
extern const char kDomPasteDisabled[];
extern const char kCaretBrowsingEnabled[];
extern const char kJavaDisabled[];
extern const char kPluginsDisabled[];
extern const char kUniversalAccessFromFileUrlsAllowed[];
extern const char kFileAccessFromFileUrlsAllowed[];
extern const char kWebSecurityDisabled[];
extern const char kXssAuditorEnabled[];
extern const char kImageLoadingDisabled[];
extern const char kShrinkStandaloneImagesToFit[];
extern const char kSiteSpecificQuirksDisabled[];
extern const char kTextAreaResizeDisabled[];
extern const char kPageCacheDisabled[];
extern const char kTabToLinksDisabled[];
extern const char kHyperlinkAuditingDisabled[];
extern const char kUserStyleSheetEnabled[];
extern const char kUserStyleSheetLocation[];
extern const char kAuthorAndUserStylesDisabled[];
extern const char kLocalStorageDisabled[];
extern const char kDatabasesDisabled[];
extern const char kApplicationCacheDisabled[];
extern const char kWebglDisabled[];
extern const char kAcceleratedCompositingDisabled[];
extern const char kAcceleratedLayersDisabled[];
extern const char kAcceleratedVideoDisabled[];
extern const char kAcceledated2dCanvasDisabled[];
extern const char kAcceleratedPaintingEnabled[];
extern const char kAcceleratedFiltersEnabled[];
extern const char kAcceleratedPluginsDisabled[];
extern const char kDeveloperToolsDisabled[];
extern const char kFullscreenEnabled[];

// Other attributes.
extern const char kProxyType[];
extern const char kProxyType_Direct[];
extern const char kProxyType_Named[];
extern const char kProxyType_Pac[];
extern const char kProxyConfig[];

}  // namespace nw

#endif  // CEF_TESTS_CEFCLIENT_CEFCLIENT_SWITCHES_H_
