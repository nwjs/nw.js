// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Defines all the "content_shell" command-line switches.

#ifndef CONTENT_NW_SRC_SHELL_SWITCHES_H_
#define CONTENT_NW_SRC_SHELL_SWITCHES_H_

#include "content/nw/src/nw_export.h"

namespace nw {
  const int kMenuHeight = 25;
  const int kToolbarHeight = 34;
}

namespace switches {

extern NW_EXPORT const char kContentShellDataPath[];
extern NW_EXPORT const char kDeveloper[];
extern NW_EXPORT const char kNoToolbar[];
extern NW_EXPORT const char kUrl[];
extern NW_EXPORT const char kWorkingDirectory[];
extern NW_EXPORT const char kNodeMain[];
extern NW_EXPORT const char kSnapshot[];
extern NW_EXPORT const char kDomStorageQuota[];
extern NW_EXPORT const char kNodejs[];
extern NW_EXPORT const char kChromeExtension[];
extern NW_EXPORT const char kVerifyContent[];

// Manifest settings
extern NW_EXPORT const char kmMain[];
extern NW_EXPORT const char kmName[];
extern NW_EXPORT const char kmWebkit[];
extern NW_EXPORT const char kmWindow[];
extern NW_EXPORT const char kmChromiumArgs[];
extern NW_EXPORT const char kmJsFlags[];

extern NW_EXPORT const char kmSingleInstance[];

extern NW_EXPORT const char kmTitle[];
extern NW_EXPORT const char kmToolbar[];
extern NW_EXPORT const char kmIcon[];
extern NW_EXPORT const char kmFrame[];
extern NW_EXPORT const char kmShow[];
extern NW_EXPORT const char kmPosition[];
extern NW_EXPORT const char kmX[];
extern NW_EXPORT const char kmY[];
extern NW_EXPORT const char kmWidth[];
extern NW_EXPORT const char kmHeight[];
extern NW_EXPORT const char kmMinWidth[];
extern NW_EXPORT const char kmMinHeight[];
extern NW_EXPORT const char kmMaxWidth[];
extern NW_EXPORT const char kmMaxHeight[];
extern NW_EXPORT const char kmResizable[];
extern NW_EXPORT const char kmAsDesktop[];
extern NW_EXPORT const char kmFullscreen[];
extern NW_EXPORT const char kmShowInTaskbar[];
extern NW_EXPORT const char kmKiosk[];
extern NW_EXPORT const char kmAlwaysOnTop[];
extern NW_EXPORT const char kmVisibleOnAllWorkspaces[];
extern NW_EXPORT const char kmInitialFocus[];
extern NW_EXPORT const char kmTransparent[];
extern NW_EXPORT const char kmDisableTransparency[];

extern NW_EXPORT const char kmWebgl[];
extern NW_EXPORT const char kmJava[];
extern NW_EXPORT const char kmPlugin[];
extern NW_EXPORT const char kmPageCache[];
extern NW_EXPORT const char kmUserAgent[];
extern NW_EXPORT const char kmRemotePages[];
extern NW_EXPORT const char kmNewInstance[];
extern NW_EXPORT const char kmInjectJSDocStart[];
extern NW_EXPORT const char kmInjectJSDocEnd[];
extern NW_EXPORT const char kmInjectCSS[];

#if defined(OS_WIN)
extern const char kPrintRaster[];
#endif

extern const char kCrashDumpsDir[];
extern NW_EXPORT const char kEnableSpellChecking[];
extern NW_EXPORT const char kDisableDevTools[];

extern NW_EXPORT const char kNodeOptions[];
}  // namespace switches

#endif  // CONTENT_NW_SRC_SHELL_SWITCHES_H_
