// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_SHELL_APPLICATION_MAC_H_
#define CONTENT_SHELL_SHELL_APPLICATION_MAC_H_

#include "base/message_pump_mac.h"
#include "base/mac/scoped_sending_event.h"

@interface ShellCrApplication : NSApplication<CrAppProtocol,
                                              CrAppControlProtocol> {
 @private
  BOOL handlingSendEvent_;
}

// CrAppProtocol:
- (BOOL)isHandlingSendEvent;

// CrAppControlProtocol:
- (void)setHandlingSendEvent:(BOOL)handlingSendEvent;

- (IBAction)newDocument:(id)sender;

@end

#endif  // CONTENT_SHELL_SHELL_APPLICATION_MAC_H_
