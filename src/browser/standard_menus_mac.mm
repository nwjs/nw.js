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
#include "base/strings/utf_string_conversions.h"
#include "grit/nw_strings.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/l10n/l10n_util_mac.h"

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

  base::string16 name = base::UTF8ToUTF16(app_name_);
  [appleMenu addItemWithTitle:l10n_util::GetNSStringFWithFixup(IDS_ABOUT_MAC, name)
                       action:@selector(orderFrontStandardAboutPanel:)
                keyEquivalent:@""];

  [appleMenu addItem:[NSMenuItem separatorItem]];

  [appleMenu addItemWithTitle:l10n_util::GetNSStringFWithFixup(IDS_HIDE_APP_MAC, name)
                       action:@selector(hide:)
                keyEquivalent:@"h"];

  NSMenuItem* menuItem = (NSMenuItem *)[appleMenu
      addItemWithTitle:l10n_util::GetNSStringWithFixup(IDS_HIDE_OTHERS_MAC)
                action:@selector(hideOtherApplications:)
         keyEquivalent:@"h"];
  [menuItem setKeyEquivalentModifierMask:(NSAlternateKeyMask|NSCommandKeyMask)];

  [appleMenu addItemWithTitle:l10n_util::GetNSStringWithFixup(IDS_SHOW_ALL_MAC)
                       action:@selector(unhideAllApplications:)
                keyEquivalent:@""];

  [appleMenu addItem:[NSMenuItem separatorItem]];

  [appleMenu addItemWithTitle:l10n_util::GetNSStringFWithFixup(IDS_EXIT_MAC, name)
                       action:@selector(closeAllWindowsQuit:)
                keyEquivalent:@"q"];

  // Add to menubar.
  [[[NSApp mainMenu] itemAtIndex:0] setSubmenu:appleMenu];

  // Finally give up our references to the objects.
  [appleMenu release];
}

void StandardMenusMac::BuildEditMenu() {
  NSMenu* editMenu = [[NSMenu alloc] initWithTitle:l10n_util::GetNSStringWithFixup(IDS_EDIT_MENU_MAC)];

  [editMenu addItemWithTitle:l10n_util::GetNSStringWithFixup(IDS_EDIT_UNDO_MAC)
                      action:@selector(undo:)
               keyEquivalent:@"z"];
  NSMenuItem* menuItem = (NSMenuItem *)[editMenu
      addItemWithTitle:l10n_util::GetNSStringWithFixup(IDS_EDIT_REDO_MAC)
                action:@selector(redo:)
         keyEquivalent:@"z"];
  [menuItem setKeyEquivalentModifierMask:(NSShiftKeyMask|NSCommandKeyMask)];

  [editMenu addItem:[NSMenuItem separatorItem]];

  [editMenu addItemWithTitle:l10n_util::GetNSStringWithFixup(IDS_CUT_MAC)
                      action:@selector(cut:)
               keyEquivalent:@"x"];
  [editMenu addItemWithTitle:l10n_util::GetNSStringWithFixup(IDS_COPY_MAC)
                      action:@selector(copy:)
               keyEquivalent:@"c"];
  [editMenu addItemWithTitle:l10n_util::GetNSStringWithFixup(IDS_PASTE_MAC)
                      action:@selector(paste:)
               keyEquivalent:@"v"];
  [editMenu addItemWithTitle:l10n_util::GetNSStringWithFixup(IDS_EDIT_DELETE_MAC)
                      action:@selector(delete:)
               keyEquivalent:@""];
  [editMenu addItemWithTitle:l10n_util::GetNSStringWithFixup(IDS_EDIT_SELECT_ALL_MAC)
                      action:@selector(selectAll:)
               keyEquivalent:@"a"];
    
  menuItem = [[NSMenuItem alloc] initWithTitle:l10n_util::GetNSStringWithFixup(IDS_EDIT_MENU_MAC)
                                        action:nil
                                 keyEquivalent:@""];
  [menuItem setSubmenu:editMenu];
  [[NSApp mainMenu] insertItem:menuItem atIndex:1];

  [editMenu release];
  [menuItem release];
}

void StandardMenusMac::BuildWindowMenu() {
  NSMenu* windowMenu = [[NSMenu alloc] initWithTitle:l10n_util::GetNSStringWithFixup(IDS_WINDOW_MENU_MAC)];

  [windowMenu addItemWithTitle:l10n_util::GetNSStringWithFixup(IDS_MINIMIZE_WINDOW_MAC)
                        action:@selector(performMiniaturize:)
                 keyEquivalent:@"m"];

  [windowMenu addItemWithTitle:l10n_util::GetNSStringWithFixup(IDS_CLOSE_WINDOW_MAC)
                        action:@selector(performClose:)
                 keyEquivalent:@"w"];

  [windowMenu addItem:[NSMenuItem separatorItem]];

  [windowMenu addItemWithTitle:l10n_util::GetNSStringWithFixup(IDS_ALL_WINDOWS_FRONT_MAC)
                        action:@selector(arrangeInFront:)
                 keyEquivalent:@""];
    
  NSMenuItem* windowMenuItem = [[NSMenuItem alloc]
      initWithTitle:l10n_util::GetNSStringWithFixup(IDS_WINDOW_MENU_MAC)
             action:nil
      keyEquivalent:@""];
  [windowMenuItem setSubmenu:windowMenu];
  [[NSApp mainMenu] addItem:windowMenuItem];
    
  [NSApp setWindowsMenu:windowMenu];

  [windowMenu release];
  [windowMenuItem release];
}

}  // namespace nw
