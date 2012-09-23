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

#include "content/nw/src/api/menuitem/menuitem.h"

#import <Cocoa/Cocoa.h>
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/api/menuitem/menuitem_delegate_mac.h"
#include <dispatch/dispatch.h>

namespace api {

using namespace v8;

MenuItem::MenuItem(CreationOption option) {
  dispatch_sync(dispatch_get_main_queue(), ^{
    if (option.type == SEPARATOR) {
      menu_item_ = [NSMenuItem separatorItem];

      // Balance release
      delegate_ = [MenuItemDelegate alloc];
      [menu_item_ retain];
    } else {
      menu_item_ = [[NSMenuItem alloc]
         initWithTitle:[NSString stringWithUTF8String:option.label.c_str()]
         action: @selector(invoke:)
         keyEquivalent: @""];

      delegate_ = [[MenuItemDelegate alloc] initWithMenuItem:this];
      [menu_item_ setTarget:delegate_];

      // Don't recursively call methods of MenuItem, otherwise we would
      // have deadlock
      if (!option.enabled)
        [menu_item_ setEnabled:NO];
      if (!option.icon.empty()) {
        NSImage* image = [[NSImage alloc]
            initWithContentsOfFile:[NSString
                stringWithUTF8String:option.icon.c_str()]];
        [menu_item_ setImage:image];
        [image release];
      }
      if (option.checked)
        [menu_item_ setState:NSOnState];
      if (!option.tooltip.empty())
        [menu_item_ setToolTip:
            [NSString stringWithUTF8String:option.tooltip.c_str()]];
    }
  });
}

MenuItem::~MenuItem() {
  dispatch_sync(dispatch_get_main_queue(), ^{
    [menu_item_ release];
    [delegate_ release];
  });
}

void MenuItem::SetLabel(const std::string& label) {
  dispatch_sync(dispatch_get_main_queue(), ^{
    [menu_item_ setTitle:[NSString stringWithUTF8String:label.c_str()]];
  });
}

std::string MenuItem::GetLabel() {
  __block NSString* title = nil;
  dispatch_sync(dispatch_get_main_queue(), ^{
    title = [menu_item_ title];
  });
  return [title UTF8String];
}

void MenuItem::SetIcon(const std::string& icon) {
  dispatch_sync(dispatch_get_main_queue(), ^{
    if (!icon.empty()) {
      NSImage* image = [[NSImage alloc]
           initWithContentsOfFile:[NSString stringWithUTF8String:icon.c_str()]];
      [menu_item_ setImage:image];
      [image release];
    } else {
      [menu_item_ setImage:nil];
    }
  });
}

void MenuItem::SetTooltip(const std::string& tooltip) {
  dispatch_sync(dispatch_get_main_queue(), ^{
    [menu_item_ setToolTip:
        [NSString stringWithUTF8String:tooltip.c_str()]];
  });
}

std::string MenuItem::GetTooltip() {
  __block NSString* tooltip = nil;
  dispatch_sync(dispatch_get_main_queue(), ^{
    tooltip = [menu_item_ toolTip];
  });
  return tooltip == nil ? "" : [tooltip UTF8String];
}

void MenuItem::SetEnabled(bool enabled) {
  dispatch_sync(dispatch_get_main_queue(), ^{
    [menu_item_ setEnabled:enabled];
  });
}

bool MenuItem::GetEnabled() {
  __block bool enabled;
  dispatch_sync(dispatch_get_main_queue(), ^{
    enabled = [menu_item_ isEnabled];
  });
  return enabled;
}

void MenuItem::SetChecked(bool checked) {
  dispatch_sync(dispatch_get_main_queue(), ^{
    if (checked)
      [menu_item_ setState:NSOnState];
    else
      [menu_item_ setState:NSOffState];
  });
}

bool MenuItem::GetChecked() {
  __block bool checked;
  dispatch_sync(dispatch_get_main_queue(), ^{
    checked = [menu_item_ state];
  });
  return checked;
}

void MenuItem::SetSubmenu(Menu* sub_menu) {
  dispatch_sync(dispatch_get_main_queue(), ^{
    [menu_item_ setSubmenu:sub_menu->menu_];
  });
}

}  // namespace api
