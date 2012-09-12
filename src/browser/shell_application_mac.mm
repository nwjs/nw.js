// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/shell_application_mac.h"

#include "base/auto_reset.h"
#include "googleurl/src/gurl.h"
#include "content/nw/src/shell.h"
#include "content/nw/src/shell_browser_context.h"
#include "content/nw/src/shell_content_browser_client.h"
#include "content/nw/src/shell_switches.h"

@implementation ShellCrApplication

- (BOOL)isHandlingSendEvent {
  return handlingSendEvent_;
}

- (void)sendEvent:(NSEvent*)event {
  AutoReset<BOOL> scoper(&handlingSendEvent_, YES);
  [super sendEvent:event];
}

- (void)setHandlingSendEvent:(BOOL)handlingSendEvent {
  handlingSendEvent_ = handlingSendEvent;
}

- (IBAction)newDocument:(id)sender {
  content::ShellBrowserContext* browserContext =
      static_cast<content::ShellContentBrowserClient*>(
        content::GetContentClient()->browser())->browser_context();
  content::Shell::CreateNewWindow(browserContext,
                                  GURL("nw:blank"),
                                  NULL,
                                  MSG_ROUTING_NONE,
                                  NULL);
}

@end
