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

#include "base/message_loop/message_loop_current.h"
#include "base/mac/scoped_sending_event.h"
#include "base/values.h"
#import <Cocoa/Cocoa.h>
#include "content/public/browser/web_contents.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/render_frame_host.h"
#include "content/nw/src/api/base/base_mac.h"
#include "content/nw/src/api/object_manager.h"
#include "content/nw/src/api/menu/menu_delegate_mac.h"
#include "content/nw/src/api/menuitem/menuitem.h"

namespace nw {

// static
Menu* Menu::GetMenuFromNative(NSMenu* menu) {
  return (Menu*)[menu associatedObject];
}

void Menu::Create(const base::DictionaryValue& option) {
  menu_ = [[NSMenu alloc] initWithTitle:@"NW Menu"];
  [menu_ setAutoenablesItems:NO];
  [menu_ setAllowsContextMenuPlugIns:NO];
  menu_delegate_ = [[NWMenuDelegate alloc] initWithMenu:this];
  [menu_ setDelegate:menu_delegate_];
  [menu_ setAssociatedObject: this];
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

void Menu::Popup(int x, int y, content::RenderFrameHost* rfh) {
  // Fake out a context menu event for our menu
  NSView* web_view = rfh->GetNativeView().GetNativeNSView();
  NSWindow* window = [web_view window];
  NSEvent* currentEvent = [NSApp currentEvent];
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
    base::MessageLoopCurrent::ScopedNestableTaskAllower allow;

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

}  // namespace nw
