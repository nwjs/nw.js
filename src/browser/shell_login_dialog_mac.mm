// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/browser/shell_login_dialog.h"

#import <Cocoa/Cocoa.h>

#include "base/logging.h"
#include "base/mac/bundle_locations.h"
#import "base/mac/scoped_nsobject.h"
#include "base/strings/sys_string_conversions.h"
#include "content/public/browser/browser_thread.h"
#import "ui/base/cocoa/nib_loading.h"

namespace {

const int kUsernameFieldTag = 1;
const int kPasswordFieldTag = 2;

}  // namespace

// Helper object that receives the notification that the dialog/sheet is
// going away.
@interface ShellLoginDialogHelper : NSObject<NSAlertDelegate> {
 @private
  base::scoped_nsobject<NSAlert> alert_;
  NSTextField* usernameField_;  // WEAK; owned by alert_
  NSSecureTextField* passwordField_;  // WEAK; owned by alert_
}

- (NSAlert*)alert;
- (NSView*)accessoryView;
- (void)focus;
- (void)alertDidEnd:(NSAlert*)alert
         returnCode:(int)returnCode
        contextInfo:(void*)contextInfo;
- (void)cancel;

@end

@implementation ShellLoginDialogHelper

- (NSAlert*)alert {
  alert_.reset([[NSAlert alloc] init]);
  [alert_ setAccessoryView:[self accessoryView]];
  return alert_;
}

- (NSView*)accessoryView {
  NSView* accessory_view = ui::GetViewFromNib(@"HttpAuth");
  if (!accessory_view)
    return nil;

  usernameField_ = [accessory_view viewWithTag:kUsernameFieldTag];
  passwordField_ = [accessory_view viewWithTag:kPasswordFieldTag];
  return accessory_view;
}

- (void)focus {
  [[alert_ window] makeFirstResponder:usernameField_];
}

- (void)alertDidEnd:(NSAlert*)alert
         returnCode:(int)returnCode
        contextInfo:(void*)contextInfo {
  if (returnCode == NSRunStoppedResponse)
    return;

  content::ShellLoginDialog* this_dialog =
      reinterpret_cast<content::ShellLoginDialog*>(contextInfo);
  if (returnCode == NSAlertFirstButtonReturn) {
    this_dialog->UserAcceptedAuth(
        base::SysNSStringToUTF16([usernameField_ stringValue]),
        base::SysNSStringToUTF16([passwordField_ stringValue]));
  } else {
    this_dialog->UserCancelledAuth();
  }
}

- (void)cancel {
  [NSApp endSheet:[alert_ window]];
  alert_.reset();
}

@end

namespace content {

void ShellLoginDialog::PlatformCreateDialog(const base::string16& message) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  helper_ = [[ShellLoginDialogHelper alloc] init];

  // Show the modal dialog.
  NSAlert* alert = [helper_ alert];
  [alert setDelegate:helper_];
  [alert setInformativeText:base::SysUTF16ToNSString(message)];
  [alert setMessageText:@"Please log in."];
  [alert addButtonWithTitle:@"OK"];
  NSButton* other = [alert addButtonWithTitle:@"Cancel"];
  [other setKeyEquivalent:@"\e"];
  [alert
      beginSheetModalForWindow:nil  // nil here makes it app-modal
                 modalDelegate:helper_
                didEndSelector:@selector(alertDidEnd:returnCode:contextInfo:)
                   contextInfo:this];

  [helper_ focus];
}

void ShellLoginDialog::PlatformCleanUp() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  [helper_ release];
  helper_ = nil;
}

void ShellLoginDialog::PlatformRequestCancelled() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  [helper_ cancel];
}

}  // namespace content
