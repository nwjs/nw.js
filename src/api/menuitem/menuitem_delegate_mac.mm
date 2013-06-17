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

#import "content/nw/src/api/menuitem/menuitem_delegate_mac.h"

#include "base/values.h"
#import <Cocoa/Cocoa.h>
#include "content/nw/src/api/app/app.h"
#include "content/nw/src/api/dispatcher_host.h"
#include "content/nw/src/api/menuitem/menuitem.h"
#include "content/nw/src/browser/native_window_mac.h"
#include "content/nw/src/nw_shell.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/render_view_host.h"

@implementation MenuItemDelegate

-(id)initWithMenuItem: (api::MenuItem*)item {
  if ([super init]) {
    menu_item_ = item;
  }

  return self;
}

-(void)invoke: (id)sender {
  NSString *value = [NSString stringWithUTF8String:menu_item_->selector_.c_str()];
  
  content::WebContents *contents = content::WebContents::FromRenderViewHost(menu_item_->dispatcher_host()->render_view_host());
  //content::RenderWidgetHostView *view = contents->GetRenderWidgetHostView();
  content::RenderViewHost *viewhost = contents->GetRenderViewHost();
  content::Shell *shell = content::Shell::FromRenderViewHost(menu_item_->dispatcher_host()->render_view_host());
  
  NSWindow *window = static_cast<nw::NativeWindowCocoa *>(shell->window())->window();
  
  if([value isEqualToString:@"closeAllWindows"]) {
    api::App::CloseAllWindows();
  } else if([value isEqualToString:@"orderFrontStandardAboutPanel"]) {
    [NSApp performSelector:@selector(orderFrontStandardAboutPanel:)];
  } else if([value isEqualToString:@"hide"]) {
    [NSApp performSelector:@selector(hide:)];
  } else if([value isEqualToString:@"hideOtherApplications"]) {
    [NSApp performSelector:@selector(hideOtherApplications:)];
  } else if([value isEqualToString:@"unhideAllApplications"]) {
    [NSApp performSelector:@selector(unhideAllApplications:)];
  } else if([value isEqualToString:@"undo"]) {
    viewhost->Undo();
  } else if([value isEqualToString:@"cut"]) {
    viewhost->Cut();
  } else if([value isEqualToString:@"copy"]) {
    viewhost->Copy();
  } else if([value isEqualToString:@"paste"]) {
    viewhost->Paste();
  } else if([value isEqualToString:@"delete"]) {
    viewhost->Delete();
  } else if([value isEqualToString:@"selectAll"]) {
    viewhost->SelectAll();
  } else if([value isEqualToString:@"performMiniaturize"]) {
    [window performSelector:@selector(performMiniaturize:)];
  } else if([value isEqualToString:@"performClose"]) {
    [window performSelector:@selector(performClose:)];
  } else if([value isEqualToString:@"arrangeInFront"]) {
    [window performSelector:@selector(arrangeInFront:)];
  } else {
    menu_item_->OnClick();

    // Send event.
    base::ListValue args;
    menu_item_->dispatcher_host()->SendEvent(menu_item_, "click", args);
  }
}

@end

