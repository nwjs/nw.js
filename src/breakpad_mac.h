// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_APP_BREAKPAD_MAC_H_
#define CHROME_APP_BREAKPAD_MAC_H_

// This header defines the Chrome entry points for Breakpad integration.

// Initializes Breakpad.
void InitCrashReporter();

// Give Breakpad a chance to store information about the current process.
// Extra information requires a parsed command line, so call this after
// CommandLine::Init has been called.
void InitCrashProcessInfo();

// Is Breakpad enabled?
bool IsCrashReporterEnabled();

#endif  // CHROME_APP_BREAKPAD_MAC_H_
