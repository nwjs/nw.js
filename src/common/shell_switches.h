// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Defines all the "content_shell" command-line switches.

#ifndef CONTENT_NW_SRC_SHELL_SWITCHES_H_
#define CONTENT_NW_SRC_SHELL_SWITCHES_H_

namespace nw {
  const int kMenuHeight = 25;
  const int kToolbarHeight = 34;
}

namespace switches {

extern const char kContentShellDataPath[];
extern const char kDeveloper[];
extern const char kNoToolbar[];
extern const char kUrl[];
extern const char kWorkingDirectory[];
extern const char kNodeMain[];
extern const char kSnapshot[];
extern const char kDomStorageQuota[];
extern const char kNodejs[];
extern const char kChromeExtension[];
// Manifest settings
extern const char kmMain[];
extern const char kmName[];
extern const char kmWebkit[];
extern const char kmWindow[];
extern const char kmChromiumArgs[];
extern const char kmJsFlags[];

extern const char kmSingleInstance[];

extern const char kmTitle[];
extern const char kmToolbar[];
extern const char kmIcon[];
extern const char kmFrame[];
extern const char kmShow[];
extern const char kmPosition[];
extern const char kmX[];
extern const char kmY[];
extern const char kmWidth[];
extern const char kmHeight[];
extern const char kmMinWidth[];
extern const char kmMinHeight[];
extern const char kmMaxWidth[];
extern const char kmMaxHeight[];
extern const char kmResizable[];
extern const char kmAsDesktop[];
extern const char kmFullscreen[];
extern const char kmShowInTaskbar[];
extern const char kmKiosk[];
extern const char kmAlwaysOnTop[];
extern const char kmVisibleOnAllWorkspaces[];
extern const char kmInitialFocus[];
extern const char kmTransparent[];
extern const char kmDisableTransparency[];

extern const char kmWebgl[];
extern const char kmJava[];
extern const char kmPlugin[];
extern const char kmPageCache[];
extern const char kmUserAgent[];
extern const char kmRemotePages[];
extern const char kmNewInstance[];
extern const char kmInjectJSDocStart[];
extern const char kmInjectJSDocEnd[];
extern const char kmInjectCSS[];

#if defined(OS_WIN)
extern const char kPrintRaster[];
#endif

extern const char kCrashDumpsDir[];

}  // namespace switches

#endif  // CONTENT_NW_SRC_SHELL_SWITCHES_H_
