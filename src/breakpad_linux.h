// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Public interface for enabling Breakpad on Linux systems.

#ifndef CHROME_APP_BREAKPAD_LINUX_H_
#define CHROME_APP_BREAKPAD_LINUX_H_

#include "build/build_config.h"

// Turns on the crash reporter in any process.
extern void InitCrashReporter();

// Enables the crash reporter in child processes.
#if defined(OS_ANDROID)
extern void InitNonBrowserCrashReporterForAndroid();
#endif

// Checks if crash reporting is enabled. Note that this is not the same as
// being opted into metrics reporting (and crash reporting), which controls
// whether InitCrashReporter() is called.
bool IsCrashReporterEnabled();

#endif  // CHROME_APP_BREAKPAD_LINUX_H_
