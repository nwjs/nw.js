// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "chrome/app/breakpad_mac.h"

#import <Foundation/Foundation.h>

// Stubbed out versions of breakpad integration functions so we can compile
// without linking in Breakpad.

bool IsCrashReporterEnabled() {
  return false;
}

void InitCrashProcessInfo() {
}

void InitCrashReporter() {
}
