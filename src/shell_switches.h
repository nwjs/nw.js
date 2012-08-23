// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Defines all the "content_shell" command-line switches.

#ifndef CONTENT_SHELL_SHELL_SWITCHES_H_
#define CONTENT_SHELL_SHELL_SWITCHES_H_

namespace switches {

extern const char kCheckLayoutTestSysDeps[];
extern const char kContentBrowserTest[];
extern const char kContentShellDataPath[];
extern const char kDeveloper[];
extern const char kDumpRenderTree[];
extern const char kNoTimeout[];
extern const char kNoToolbar[];
extern const char kVersion[];
extern const char kUrl[];

// Manifest settings
extern const char kmMain[];
extern const char kmName[];
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

}  // namespace switches

#endif  // CONTENT_SHELL_SHELL_SWITCHES_H_
