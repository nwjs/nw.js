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

#ifndef CONTENT_NW_SRC_BROWSER_SHELL_JAVASCRIPT_DIALOG_H_
#define CONTENT_NW_SRC_BROWSER_SHELL_JAVASCRIPT_DIALOG_H_

#include "content/public/browser/javascript_dialog_manager.h"

#if defined(TOOLKIT_GTK)
#include "ui/base/gtk/gtk_signal.h"
#endif

#if defined(OS_MACOSX)
#if __OBJC__
@class ShellJavaScriptDialogHelper;
#else
class ShellJavaScriptDialogHelper;
#endif  // __OBJC__
#endif  // defined(OS_MACOSX)

namespace content {

class ShellJavaScriptDialogCreator;

class ShellJavaScriptDialog {
 public:
  ShellJavaScriptDialog(
      ShellJavaScriptDialogCreator* creator,
      gfx::NativeWindow parent_window,
      JavaScriptMessageType message_type,
      const base::string16& message_text,
      const base::string16& default_prompt_text,
      const JavaScriptDialogManager::DialogClosedCallback& callback);
  ~ShellJavaScriptDialog();

  // Called to cancel a dialog mid-flight.
  void Cancel();

 private:
  ShellJavaScriptDialogCreator* creator_;
  JavaScriptDialogManager::DialogClosedCallback callback_;

#if defined(OS_MACOSX)
  ShellJavaScriptDialogHelper* helper_;  // owned
#elif defined(OS_WIN)
  JavaScriptMessageType message_type_;
  HWND dialog_win_;
  base::string16 message_text_;
  base::string16 default_prompt_text_;
  static INT_PTR CALLBACK DialogProc(HWND dialog, UINT message, WPARAM wparam,
                                     LPARAM lparam);
#elif defined(TOOLKIT_GTK)
  GtkWidget* gtk_dialog_;
  gfx::NativeWindow parent_window_;
  CHROMEGTK_CALLBACK_1(ShellJavaScriptDialog, void, OnResponse, int);
#endif

  DISALLOW_COPY_AND_ASSIGN(ShellJavaScriptDialog);
};

}  // namespace content

#endif  // CONTENT_NW_SRC_BROWSER_SHELL_JAVASCRIPT_DIALOG_H_
