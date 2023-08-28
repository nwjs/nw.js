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

#include "base/values.h"
#import <Cocoa/Cocoa.h>
#include "ui/display/screen.h"
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/api/object_manager.h"


@interface MacTrayObserver : NSObject {
@private
    RAW_PTR_EXCLUSION nw::Tray* tray_;
}
- (void)setBacking:(nw::Tray*)tray_;
- (void)onClick:(id)sender;
@end

@implementation MacTrayObserver
- (void)setBacking:(nw::Tray*)newTray {
    tray_ = newTray;
}
- (void)onClick:(id)sender {
    base::Value::List args;
    base::Value::Dict data;
    // Get the position of the frame of the NSStatusItem
    NSPoint pos = ([[[NSApp currentEvent] window] frame]).origin;
    // Flip coordinates to gfx (0,0 in top-left corner) using primary screen.
    NSScreen* screen = [[NSScreen screens] objectAtIndex:0];
    pos.y = NSMaxY([screen frame]) - pos.y;
    data.Set("x", pos.x);
    data.Set("y", pos.y);
    args.Append(data.Clone());
    tray_->object_manager()->SendEvent(tray_,"TrayClick",args);
}
@end

namespace nw {
    
void Tray::Create(const base::Value::Dict& option) {
  NSStatusBar *status_bar = [NSStatusBar systemStatusBar];
  MacTrayObserver* observer = [[MacTrayObserver alloc] init];
  [observer setBacking:this];
  status_item_ = [status_bar statusItemWithLength:NSVariableStatusItemLength];
  NSButton* item_button = status_item_.button;
  NSButtonCell* item_button_cell = item_button.cell;
  item_button_cell.highlightsBy =
        NSContentsCellMask | NSChangeBackgroundCellMask;
  [status_item_ retain];
  item_button.target = observer;
  item_button.action = @selector(onClick:);
}

void Tray::ShowAfterCreate() {
}

void Tray::Destroy() {
  [status_item_ release];
}

void Tray::SetTitle(const std::string& title) {
  // note: this is kind of mad but the first time we set the title property 
  // we have to call setTitle twice or it won't get the right dimensions
  if (status_item_.button.title != nil) {
    status_item_.button.title = [NSString stringWithUTF8String:title.c_str()];
  }
  status_item_.button.title = [NSString stringWithUTF8String:title.c_str()];
}

void Tray::SetIcon(const std::string& icon) {
  if (!icon.empty()) {
    NSImage* image = [[NSImage alloc]
         initWithContentsOfFile:[NSString stringWithUTF8String:icon.c_str()]];
    [image setTemplate:iconsAreTemplates];
    status_item_.button.image = image;
    [image release];
  } else {
    status_item_.button.image = nil;
  }
}

void Tray::SetAltIcon(const std::string& alticon) {
  if (!alticon.empty()) {
    NSImage* image = [[NSImage alloc]
         initWithContentsOfFile:[NSString stringWithUTF8String:alticon.c_str()]];
    [image setTemplate:iconsAreTemplates];
    status_item_.button.alternateImage = image;
    [image release];
  } else {
    status_item_.button.alternateImage = nil;
  }
}

void Tray::SetIconsAreTemplates(bool areTemplates) {
  iconsAreTemplates = areTemplates;
  if (status_item_.button.image != nil) {
    [status_item_.button.image setTemplate:areTemplates];
  }
  if (status_item_.button.alternateImage != nil) {
    [status_item_.button.alternateImage setTemplate:areTemplates];
  }
}

void Tray::SetTooltip(const std::string& tooltip) {
  status_item_.button.toolTip = [NSString stringWithUTF8String:tooltip.c_str()];
}

void Tray::SetMenu(Menu* menu) {
  status_item_.button.target = nil;
  status_item_.button.action = nil;
  [status_item_ setMenu:menu->menu_];
}

void Tray::Remove() {
  [[NSStatusBar systemStatusBar] removeStatusItem:status_item_];
}

}  // namespace nw
