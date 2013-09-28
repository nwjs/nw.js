// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/app/chrome_breakpad_client.h"

#include "base/command_line.h"
#include "chrome/common/chrome_switches.h"

#if !defined(DISABLE_NACL)
#import "breakpad/src/client/mac/Framework/Breakpad.h"
#include "components/nacl/common/nacl_switches.h"
#include "native_client/src/trusted/service_runtime/osx/crash_filter.h"
#endif

namespace chrome {

namespace {

#if !defined(DISABLE_NACL)
bool NaClBreakpadCrashFilter(int exception_type,
                             int exception_code,
                             mach_port_t crashing_thread,
                             void* context) {
  return !NaClMachThreadIsInUntrusted(crashing_thread);
}
#endif

}  // namespace

void ChromeBreakpadClient::InstallAdditionalFilters(BreakpadRef breakpad) {
#if !defined(DISABLE_NACL)
  if (CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
          switches::kProcessType) == switches::kNaClLoaderProcess) {
    BreakpadSetFilterCallback(breakpad, NaClBreakpadCrashFilter, NULL);
  }
#endif
}

}  // namespace chrome
