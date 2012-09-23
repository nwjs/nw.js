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

#include "content/nw/src/api/tray/tray.h"

#include "content/nw/src/api/menu/menu.h"

#import <Cocoa/Cocoa.h>

namespace api {

Tray::Tray(CreationOption option) {
  dispatch_sync(dispatch_get_main_queue(), ^{
    NSStatusBar *status_bar = [NSStatusBar systemStatusBar];
    status_item_ = [status_bar statusItemWithLength:NSVariableStatusItemLength];
    [status_item_ retain];
    [status_item_ setHighlightMode:YES];

    // Don't recursively call methods of MenuItem, otherwise we would
    // have deadlock
    if (!option.icon.empty()) {
      NSImage* image = [[NSImage alloc]
          initWithContentsOfFile:[NSString 
              stringWithUTF8String:option.icon.c_str()]];
      [status_item_ setImage:image];
      [image release];
    }
    if (!option.title.empty())
      [status_item_ setTitle:
          [NSString stringWithUTF8String:option.title.c_str()]];
    if (!option.tooltip.empty())
      [status_item_ setToolTip:
          [NSString stringWithUTF8String:option.tooltip.c_str()]];
  });
}

Tray::~Tray() {
  dispatch_sync(dispatch_get_main_queue(), ^{
    [status_item_ release];
  });
}

void Tray::SetTitle(const std::string& title) {
  dispatch_sync(dispatch_get_main_queue(), ^{
    [status_item_ setTitle:
        [NSString stringWithUTF8String:title.c_str()]];
  });
}

std::string Tray::GetTitle() {
  __block NSString* title = nil;
  dispatch_sync(dispatch_get_main_queue(), ^{
    title = [status_item_ title];
  });
  return [title UTF8String];
}

void Tray::SetIcon(const std::string& icon) {
  dispatch_sync(dispatch_get_main_queue(), ^{
    if (!icon.empty()) {
      NSImage* image = [[NSImage alloc]
           initWithContentsOfFile:[NSString stringWithUTF8String:icon.c_str()]];
      [status_item_ setImage:image];
      [image release];
    } else {
      [status_item_ setImage:nil];
    }
  });
}

void Tray::SetTooltip(const std::string& tooltip) {
  dispatch_sync(dispatch_get_main_queue(), ^{
    [status_item_ setToolTip:
        [NSString stringWithUTF8String:tooltip.c_str()]];
  });
}

std::string Tray::GetTooltip() {
  __block NSString* tooltip = nil;
  dispatch_sync(dispatch_get_main_queue(), ^{
    tooltip = [status_item_ toolTip];
  });
  return tooltip == nil ? "" : [tooltip UTF8String];
}

void Tray::SetMenu(Menu* menu) {
  dispatch_sync(dispatch_get_main_queue(), ^{
    [status_item_ setMenu:menu->menu_];
  });
}

void Tray::Remove() {
  dispatch_sync(dispatch_get_main_queue(), ^{
    [[NSStatusBar systemStatusBar] removeStatusItem:status_item_];
  });
}

}  // namespace api
