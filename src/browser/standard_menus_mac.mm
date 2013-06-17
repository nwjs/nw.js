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

#include "content/nw/src/browser/standard_menus_mac.h"

#include "base/strings/sys_string_conversions.h"
#import <Cocoa/Cocoa.h>

// For some reaon, Apple removed setAppleMenu from the headers in 10.4,
// but the method still is there and works. To avoid warnings, we declare
// it ourselves here.
@interface NSApplication(SDL_Missing_Methods)
- (void)setAppleMenu:(NSMenu *)menu;
@end

namespace nw {

StandardMenusMac::StandardMenusMac(const std::string& app_name)
    : app_name_(app_name) {
}

StandardMenusMac::~StandardMenusMac() {
}

void StandardMenusMac::BuildAppleMenu() {
  NSMenu* appleMenu = [[NSMenu alloc] initWithTitle:@""];

  NSString* name = base::SysUTF8ToNSString(app_name_);
  [appleMenu addItemWithTitle:[@"About " stringByAppendingString:name]
                       action:@selector(orderFrontStandardAboutPanel:)
                keyEquivalent:@""];

  [appleMenu addItem:[NSMenuItem separatorItem]];

  [appleMenu addItemWithTitle:[@"Hide " stringByAppendingString:name]
                       action:@selector(hide:)
                keyEquivalent:@"h"];

  NSMenuItem* menuItem = (NSMenuItem *)[appleMenu
      addItemWithTitle:@"Hide Others"
                action:@selector(hideOtherApplications:)
         keyEquivalent:@"h"];
  [menuItem setKeyEquivalentModifierMask:(NSAlternateKeyMask|NSCommandKeyMask)];

  [appleMenu addItemWithTitle:@"Show All"
                       action:@selector(unhideAllApplications:)
                keyEquivalent:@""];

  [appleMenu addItem:[NSMenuItem separatorItem]];

  [appleMenu addItemWithTitle:[@"Quit " stringByAppendingString:name]
                       action:@selector(closeAllWindows:)
                keyEquivalent:@"q"];

  // Add to menubar.
  [[[NSApp mainMenu] itemAtIndex:0] setSubmenu:appleMenu];

  // Finally give up our references to the objects.
  [appleMenu release];
}

void StandardMenusMac::BuildEditMenu() {
  NSMenu* editMenu = [[NSMenu alloc] initWithTitle:@"Edit"];

  [editMenu addItemWithTitle:@"Undo"
                      action:@selector(undo:)
               keyEquivalent:@"z"];
  NSMenuItem* menuItem = (NSMenuItem *)[editMenu
      addItemWithTitle:@"Redo"
                action:@selector(redo:)
         keyEquivalent:@"z"];
  [menuItem setKeyEquivalentModifierMask:(NSShiftKeyMask|NSCommandKeyMask)];

  [editMenu addItem:[NSMenuItem separatorItem]];

  [editMenu addItemWithTitle:@"Cut"
                      action:@selector(cut:)
               keyEquivalent:@"x"];
  [editMenu addItemWithTitle:@"Copy"
                      action:@selector(copy:)
               keyEquivalent:@"c"];
  [editMenu addItemWithTitle:@"Paste"
                      action:@selector(paste:)
               keyEquivalent:@"v"];
  [editMenu addItemWithTitle:@"Delete"
                      action:@selector(delete:)
               keyEquivalent:@""];
  [editMenu addItemWithTitle:@"Select All"
                      action:@selector(selectAll:)
               keyEquivalent:@"a"];
    
  menuItem = [[NSMenuItem alloc] initWithTitle:@"Edit"
                                        action:nil
                                 keyEquivalent:@""];
  [menuItem setSubmenu:editMenu];
  [[NSApp mainMenu] insertItem:menuItem atIndex:1];

  [editMenu release];
  [menuItem release];
}

void StandardMenusMac::BuildWindowMenu() {
  NSMenu* windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];

  [windowMenu addItemWithTitle:@"Minimize"
                        action:@selector(performMiniaturize:)
                 keyEquivalent:@"m"];
  [windowMenu addItemWithTitle:@"Close"
                        action:@selector(performClose:)
                 keyEquivalent:@"w"];

  [windowMenu addItem:[NSMenuItem separatorItem]];

  [windowMenu addItemWithTitle:@"Bring All to Front"
                        action:@selector(arrangeInFront:)
                 keyEquivalent:@""];
    
  NSMenuItem* windowMenuItem = [[NSMenuItem alloc]
      initWithTitle:@"Window"
             action:nil
      keyEquivalent:@""];
  [windowMenuItem setSubmenu:windowMenu];
  [[NSApp mainMenu] addItem:windowMenuItem];
    
  [NSApp setWindowsMenu:windowMenu];

  [windowMenu release];
  [windowMenuItem release];
}

}  // namespace nw
