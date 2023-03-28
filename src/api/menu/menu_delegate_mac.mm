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

#include "base/run_loop.h"
#include "base/values.h"
#include "content/nw/src/api/object_manager.h"
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/api/menu/menu_delegate_mac.h"

@implementation NWMenuDelegate

- (id)initWithMenu:(nw::Menu*) menu {
  if ((self = [super init])) {
    nwmenu_ = menu;
  }
  return self;
}

- (BOOL)menuHasKeyEquivalent:(NSMenu *)menu forEvent:(NSEvent *)event target:(id *)target action:(SEL *)action {
  return NO;
}

- (void)menuNeedsUpdate:(NSMenu*)menu {

  if (!nwmenu_->enable_show_event() || nwmenu_->object_manager()->run_loop())
    return;

  // NSEvent* event = [NSApp currentEvent];
  // NSLog (@"%@\n", event);
  // Cocoa will try to populate menu on every keystoke of the key equivlants,
  // which is slow. The following bypassed it

  // if ([event type] != NSSystemDefined || [event subtype] == 8)
  //  return;

  if (!nwmenu_->enable_show_event())
    return;

  base::Value::List args;
  base::RunLoop run_loop;
  nwmenu_->object_manager()->set_run_loop(&run_loop);
  nwmenu_->object_manager()->SendEvent(nwmenu_, "show", args);
  run_loop.Run();
}

@end
