// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/app/chrome_breakpad_client.h"

#include <CoreFoundation/CoreFoundation.h>

#include "base/command_line.h"
#include "base/mac/scoped_cftyperef.h"
#include "base/strings/sys_string_conversions.h"
#include "chrome/common/chrome_switches.h"
//#include "policy/policy_constants.h"


namespace chrome {

void ChromeBreakpadClient::InstallAdditionalFilters(BreakpadRef breakpad) {
}

bool ChromeBreakpadClient::ReportingIsEnforcedByPolicy(bool* breakpad_enabled) {
  return false;
}

}  // namespace chrome
