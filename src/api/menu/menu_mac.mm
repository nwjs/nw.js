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

#include "content/nw/src/api/menu/menu.h"

#include "base/message_loop/message_loop.h"
#include "base/mac/scoped_sending_event.h"
#include "base/values.h"
#import <Cocoa/Cocoa.h>
#include "content/public/browser/web_contents.h"
#include "content/nw/src/api/dispatcher_host.h"
#include "content/nw/src/api/menu/menu_delegate_mac.h"
#include "content/nw/src/api/menuitem/menuitem.h"
#include "content/nw/src/browser/native_window_mac.h"
#include "content/nw/src/nw_shell.h"

namespace nwapi {

void Menu::Create(const base::DictionaryValue& option) {
  menu_ = [[NSMenu alloc] initWithTitle:@"NW Menu"];
  [menu_ setAutoenablesItems:NO];
  menu_delegate_ = [[NWMenuDelegate alloc] initWithMenu:this];
  [menu_ setDelegate:menu_delegate_];
}

void Menu::Destroy() {
  [menu_ release];
  [menu_delegate_ release];
}

void Menu::Append(MenuItem* menu_item) {
  [menu_ addItem:menu_item->menu_item_];
}

void Menu::Insert(MenuItem* menu_item, int pos) {
  [menu_ insertItem:menu_item->menu_item_ atIndex:pos];
}

void Menu::Remove(MenuItem* menu_item, int pos) {
  [menu_ removeItem:menu_item->menu_item_];
}

void Menu::Popup(int x, int y, content::Shell* shell) {
  // Fake out a context menu event for our menu
  NSWindow* window =
       static_cast<nw::NativeWindowCocoa*>(shell->window())->window();
  NSEvent* currentEvent = [NSApp currentEvent];
  NSView* web_view = shell->web_contents()->GetNativeView();
  NSPoint position = { x, web_view.bounds.size.height - y };
  NSTimeInterval eventTime = [currentEvent timestamp];
  NSEvent* clickEvent = [NSEvent mouseEventWithType:NSRightMouseDown
                                           location:position
                                      modifierFlags:NSRightMouseDownMask
                                          timestamp:eventTime
                                       windowNumber:[window windowNumber]
                                            context:nil
                                        eventNumber:0
                                         clickCount:1
                                           pressure:1.0];

  {
    // Make sure events can be pumped while the menu is up.
    base::MessageLoop::ScopedNestableTaskAllower allow(base::MessageLoop::current());

    // One of the events that could be pumped is |window.close()|.
    // User-initiated event-tracking loops protect against this by
    // setting flags in -[CrApplication sendEvent:], but since
    // web-content menus are initiated by IPC message the setup has to
    // be done manually.
    base::mac::ScopedSendingEvent sendingEventScoper;

    // Show the menu.
    [NSMenu popUpContextMenu:menu_
                   withEvent:clickEvent
                     forView:web_view];
  }
}

}  // namespace nwapi
