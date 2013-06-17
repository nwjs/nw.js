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

#include "content/nw/src/browser/shell_javascript_dialog.h"

#import <Cocoa/Cocoa.h>

#import "base/memory/scoped_nsobject.h"
#include "base/strings/sys_string_conversions.h"
#include "content/nw/src/browser/shell_javascript_dialog_creator.h"

// Helper object that receives the notification that the dialog/sheet is
// going away. Is responsible for cleaning itself up.
@interface ShellJavaScriptDialogHelper : NSObject<NSAlertDelegate> {
 @private
  scoped_nsobject<NSAlert> alert_;
  NSTextField* textField_;  // WEAK; owned by alert_

  // Copies of the fields in ShellJavaScriptDialog because they're private.
  content::ShellJavaScriptDialogCreator* creator_;
  content::JavaScriptDialogManager::DialogClosedCallback callback_;
}

- (id)initHelperWithCreator:(content::ShellJavaScriptDialogCreator*)creator
   andCallback:(content::JavaScriptDialogManager::DialogClosedCallback)callback;
- (NSAlert*)alert;
- (NSTextField*)textField;
- (void)alertDidEnd:(NSAlert*)alert
         returnCode:(int)returnCode
        contextInfo:(void*)contextInfo;
- (void)cancel;

@end

@implementation ShellJavaScriptDialogHelper

- (id)initHelperWithCreator:(content::ShellJavaScriptDialogCreator*)creator
  andCallback:(content::JavaScriptDialogManager::DialogClosedCallback)callback {
  if (self = [super init]) {
    creator_ = creator;
    callback_ = callback;
  }

  return self;
}

- (NSAlert*)alert {
  alert_.reset([[NSAlert alloc] init]);
  return alert_;
}

- (NSTextField*)textField {
  textField_ = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 300, 22)];
  [[textField_ cell] setLineBreakMode:NSLineBreakByTruncatingTail];
  [alert_ setAccessoryView:textField_];
  [textField_ release];

  return textField_;
}

- (void)alertDidEnd:(NSAlert*)alert
         returnCode:(int)returnCode
        contextInfo:(void*)contextInfo {
  if (returnCode == NSRunStoppedResponse)
    return;

  bool success = returnCode == NSAlertFirstButtonReturn;
  string16 input;
  if (textField_)
    input = base::SysNSStringToUTF16([textField_ stringValue]);

  content::ShellJavaScriptDialog* native_dialog =
      reinterpret_cast<content::ShellJavaScriptDialog*>(contextInfo);
  callback_.Run(success, input);
  creator_->DialogClosed(native_dialog);
}

- (void)cancel {
  [NSApp endSheet:[alert_ window]];
  alert_.reset();
}

@end

namespace content {

ShellJavaScriptDialog::ShellJavaScriptDialog(
    ShellJavaScriptDialogCreator* creator,
    gfx::NativeWindow parent_window,
    JavaScriptMessageType message_type,
    const string16& message_text,
    const string16& default_prompt_text,
    const JavaScriptDialogManager::DialogClosedCallback& callback)
    : creator_(creator),
      callback_(callback) {
  bool text_field = message_type == JAVASCRIPT_MESSAGE_TYPE_PROMPT;
  bool one_button = message_type == JAVASCRIPT_MESSAGE_TYPE_ALERT;

  helper_ =
      [[ShellJavaScriptDialogHelper alloc] initHelperWithCreator:creator
                                                     andCallback:callback];

  // Show the modal dialog.
  NSAlert* alert = [helper_ alert];
  NSTextField* field = nil;
  if (text_field) {
    field = [helper_ textField];
    [field setStringValue:base::SysUTF16ToNSString(default_prompt_text)];
  }
  [alert setDelegate:helper_];
  [alert setInformativeText:base::SysUTF16ToNSString(message_text)];
  [alert setMessageText:@"Javascript alert"];
  [alert addButtonWithTitle:@"OK"];
  if (!one_button) {
    NSButton* other = [alert addButtonWithTitle:@"Cancel"];
    [other setKeyEquivalent:@"\e"];
  }

  [alert
      beginSheetModalForWindow:nil  // nil here makes it app-modal
                 modalDelegate:helper_
                didEndSelector:@selector(alertDidEnd:returnCode:contextInfo:)
                   contextInfo:this];

  if ([alert accessoryView])
    [[alert window] makeFirstResponder:[alert accessoryView]];
}

ShellJavaScriptDialog::~ShellJavaScriptDialog() {
  [helper_ release];
}

void ShellJavaScriptDialog::Cancel() {
  [helper_ cancel];
}

}  // namespace content
