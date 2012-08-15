// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/shell_browser_main_parts.h"

#import <Cocoa/Cocoa.h>

#include "base/mac/bundle_locations.h"
#include "base/memory/scoped_nsobject.h"
#include "content/shell/shell_application_mac.h"

namespace content {

void ShellBrowserMainParts::PreMainMessageLoopStart() {
  // Force the NSApplication subclass to be used.
  [ShellCrApplication sharedApplication];

  scoped_nsobject<NSNib>
      nib([[NSNib alloc] initWithNibNamed:@"MainMenu"
                                   bundle:base::mac::FrameworkBundle()]);
  [nib instantiateNibWithOwner:NSApp topLevelObjects:nil];
}

}  // namespace content
