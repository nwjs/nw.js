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

#include "base/message_loop.h"
#include "base/mac/scoped_sending_event.h"
#import <Cocoa/Cocoa.h>
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_view.h"
#include "content/nw/src/api/menuitem/menuitem.h"
#include "content/nw/src/shell.h"
#include <dispatch/dispatch.h>

namespace api {

using namespace v8;

Menu::Menu(CreationOption option) {
  dispatch_sync(dispatch_get_main_queue(), ^{
    menu_ = [[NSMenu alloc]
        initWithTitle:[NSString stringWithUTF8String:option.title.c_str()]];
    [menu_ setAutoenablesItems:NO];
  });
}

Menu::~Menu() {
  dispatch_sync(dispatch_get_main_queue(), ^{
    [menu_ release];
  });
}

void Menu::SetTitle(const std::string& title) {
  dispatch_sync(dispatch_get_main_queue(), ^{
    [menu_ setTitle:[NSString stringWithUTF8String:title.c_str()]];
  });
}

std::string Menu::GetTitle() {
  __block NSString* title = nil;
  dispatch_sync(dispatch_get_main_queue(), ^{
    title = [menu_ title];
  });
  return [title UTF8String];
}

void Menu::Append(MenuItem* menu_item) {
  dispatch_sync(dispatch_get_main_queue(), ^{
    [menu_ addItem:menu_item->menu_item_];
  });
}

void Menu::Insert(MenuItem* menu_item, int pos) {
  dispatch_sync(dispatch_get_main_queue(), ^{
    [menu_ insertItem:menu_item->menu_item_ atIndex:pos];
  });
}

void Menu::Remove(MenuItem* menu_item) {
  dispatch_sync(dispatch_get_main_queue(), ^{
    [menu_ removeItem:menu_item->menu_item_];
  });
}

void Menu::Remove(int pos) {
  dispatch_sync(dispatch_get_main_queue(), ^{
    [menu_ removeItemAtIndex:pos];
  });
}

void Menu::PopupInUI(int x, int y, content::Shell* shell) {
  // Fake out a context menu event for our menu
  NSEvent* currentEvent = [NSApp currentEvent];
  NSWindow* window = shell->window();
  NSView* web_view = shell->web_contents()->GetView()->GetNativeView();
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
    MessageLoop::ScopedNestableTaskAllower allow(MessageLoop::current());

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

}  // namespace api
