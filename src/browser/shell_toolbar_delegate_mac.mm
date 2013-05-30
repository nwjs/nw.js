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

#include "content/nw/src/browser/shell_toolbar_delegate_mac.h"

#include <string>

#import "base/memory/scoped_nsobject.h"
#include "base/strings/sys_string_conversions.h"
#include "content/nw/src/browser/native_window.h"
#include "content/nw/src/nw_shell.h"
#include "googleurl/src/gurl.h"

static NSString *BackToolbarItemIdentifier = @"Back";
static NSString *ForwardToolbarItemIdentifier = @"Forward";
static NSString *RefreshToolbarItemIdentifier = @"Refresh";
static NSString *DevtoolsToolbarItemIdentifier = @"Devtools";
static NSString *EntryToolbarItemIdentifier = @"Entry";
static NSString *ReloadDevToolbarItemIdentifier = @"RefreshDev";

@implementation ShellToolbarDelegate

- (id)initWithShell:(content::Shell*)shell {
  if ((self = [super init])) {
    shell_ = shell;
  }
  return self;
}

- (void)buttonPressed:(id)sender {
  switch ([sender tag]) {
    case nw::NativeWindow::BUTTON_BACK:
      shell_->GoBackOrForward(-1);
      break;
    case nw::NativeWindow::BUTTON_FORWARD:
      shell_->GoBackOrForward(1);
      break;
    case nw::NativeWindow::BUTTON_REFRESH_OR_STOP:
      shell_->ReloadOrStop();
      break;
    case nw::NativeWindow::BUTTON_DEVTOOLS:
      shell_->ShowDevTools();
      break;
    case nw::NativeWindow::BUTTON_REFRESH_DEV:
      shell_->Reload(content::Shell::RELOAD_DEV);
      break;
  }
}

- (void)urlEntered:(id)sender {
  std::string url_string = base::SysNSStringToUTF8([sender stringValue]);
  if (!url_string.empty()) {
    GURL url(url_string);
    if (!url.has_scheme())
      url = GURL("http://" + url_string);
    shell_->LoadURL(url);
  }
}

- (void)setEnabled:(BOOL)enabled
         forButton:(nw::NativeWindow::TOOLBAR_BUTTON)button {
  switch (button) {
    case nw::NativeWindow::BUTTON_BACK:
      [back_button_ setEnabled:enabled];
      break;
    case nw::NativeWindow::BUTTON_FORWARD:
      [forward_button_ setEnabled:enabled];
      break;
    case nw::NativeWindow::BUTTON_REFRESH_OR_STOP:
      [refresh_stop_button_ setEnabled:enabled];
      break;
    case nw::NativeWindow::BUTTON_DEVTOOLS:
      [devtools_button_ setEnabled:enabled];
      break;
    case nw::NativeWindow::BUTTON_REFRESH_DEV:
      [refresh_dev_button_ setEnabled:enabled];
      break;
  }
}

- (void)setUrl:(NSString*)url {
  [entry_ setStringValue:url];
}

- (void)setIsLoading:(BOOL)loading {
  if (loading)
    [refresh_stop_button_
        setImage:[NSImage imageNamed:NSImageNameStopProgressTemplate]];
  else
    [refresh_stop_button_
        setImage:[NSImage imageNamed:NSImageNameRefreshTemplate]];
}

- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar
     itemForItemIdentifier:(NSString *)identifier
 willBeInsertedIntoToolbar:(BOOL)flag {
  NSToolbarItem* item = 
     [[NSToolbarItem alloc] initWithItemIdentifier:identifier];

  if ([identifier isEqualTo:EntryToolbarItemIdentifier]) {
    scoped_nsobject<NSTextField> entry(
        [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 100, 24)]);
    entry_ = entry;
    [entry setAutoresizingMask:(NSViewWidthSizable | NSViewMinYMargin)];
    [entry setBezelStyle:NSTextFieldSquareBezel];
    [entry setTarget:self];
    [entry setAction:@selector(urlEntered:)];
    [[entry cell] setWraps:NO];
    [[entry cell] setScrollable:YES];
    [item setView:entry];
    [item setMinSize:NSMakeSize(100, 20)];
    [item setMaxSize:NSMakeSize(2000, 20)];
  } else {
    scoped_nsobject<NSButton> button([[NSButton alloc] init]);
    [button setBezelStyle:NSTexturedRoundedBezelStyle];
    [button setTarget:self];
    [button setAction:@selector(buttonPressed:)];
    item = [[NSToolbarItem alloc] initWithItemIdentifier:identifier];
    [item setView:button];
    [item setMinSize:NSMakeSize(28, 24)];
    [item setMaxSize:NSMakeSize(28, 24)];
    [item setToolTip:identifier];
  }

  if ([identifier isEqualTo:BackToolbarItemIdentifier]) {
    back_button_ = item;
    [item setImage:[NSImage imageNamed:NSImageNameGoLeftTemplate]];
    [item setTag:nw::NativeWindow::BUTTON_BACK];
  } else if ([identifier isEqualTo:ForwardToolbarItemIdentifier]) {
    forward_button_ = item;
    [item setImage:[NSImage imageNamed:NSImageNameGoRightTemplate]];
    [item setTag:nw::NativeWindow::BUTTON_FORWARD];
  } else if ([identifier isEqualTo:RefreshToolbarItemIdentifier]) {
    refresh_stop_button_ = item;
    [item setImage:[NSImage imageNamed:NSImageNameStopProgressTemplate]];
    [item setTag:nw::NativeWindow::BUTTON_REFRESH_OR_STOP];
  } else if ([identifier isEqualTo:DevtoolsToolbarItemIdentifier]) {
    devtools_button_ = item;
    [item setMinSize:NSMakeSize(28, 24)];
    [item setMaxSize:NSMakeSize(28, 24)];
    [item setImage:[NSImage imageNamed:NSImageNameActionTemplate]];
    [item setTag:nw::NativeWindow::BUTTON_DEVTOOLS];
  } else if ([identifier isEqualTo:ReloadDevToolbarItemIdentifier]) {
    refresh_dev_button_ = item;
    [item setImage:[NSImage imageNamed:NSImageNameRefreshTemplate]];
    [item setTag:nw::NativeWindow::BUTTON_REFRESH_DEV];
  }

  return [item autorelease];
}

- (NSArray *) toolbarAllowedItemIdentifiers: (NSToolbar *) toolbar {
  return [NSArray arrayWithObjects:BackToolbarItemIdentifier, 
      ForwardToolbarItemIdentifier,
      RefreshToolbarItemIdentifier, 
      EntryToolbarItemIdentifier,
      DevtoolsToolbarItemIdentifier,
      ReloadDevToolbarItemIdentifier,
      nil];
}

- (NSArray *) toolbarDefaultItemIdentifiers: (NSToolbar *)toolbar {
  return [NSArray arrayWithObjects:BackToolbarItemIdentifier, 
      ForwardToolbarItemIdentifier,
      RefreshToolbarItemIdentifier, 
      EntryToolbarItemIdentifier,
      DevtoolsToolbarItemIdentifier,
      ReloadDevToolbarItemIdentifier,
      nil];
}

@end
