// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_PATHS_MAC_H_
#define CONTENT_SHELL_PATHS_MAC_H_

#include "base/files/file_path.h"

// Sets up base::mac::FrameworkBundle.
void OverrideFrameworkBundlePath();

// Sets up the CHILD_PROCESS_EXE path to properly point to the helper app.
void OverrideChildProcessPath();

// Gets the path to the content shell's pak file.
base::FilePath GetResourcesPakFilePath();

#endif  // CONTENT_SHELL_PATHS_MAC_H_
