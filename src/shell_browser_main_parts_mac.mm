// Copyright (c) 2012 Intel Corp
// Copyright (c) 2012 The Chromium Authors
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell co
// pies of the Software, and to permit persons to whom the Software is furnished
//  to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in al
// l copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IM
// PLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNES
// S FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
//  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WH
// ETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "shell_browser_main_parts.h"

#import <Cocoa/Cocoa.h>

#include "base/mac/bundle_locations.h"
#include "base/memory/scoped_nsobject.h"
#include "base/command_line.h"
#include "shell_application_mac.h"
#include "shell_switches.h"

namespace content {

void ShellBrowserMainParts::PreMainMessageLoopStart() {
  // Force the NSApplication subclass to be used.
  [ShellCrApplication sharedApplication];

  if (CommandLine::ForCurrentProcess()->HasSwitch(switches::kDeveloper)) {
    scoped_nsobject<NSNib>
        nib([[NSNib alloc] initWithNibNamed:@"MainMenu"
                                     bundle:base::mac::FrameworkBundle()]);
    [nib instantiateNibWithOwner:NSApp topLevelObjects:nil];
  }
}

}  // namespace content
