// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_SHELL_JAVASCRIPT_DIALOG_H_
#define CONTENT_SHELL_SHELL_JAVASCRIPT_DIALOG_H_

#include "content/public/browser/javascript_dialogs.h"

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
      const string16& message_text,
      const string16& default_prompt_text,
      const JavaScriptDialogCreator::DialogClosedCallback& callback);
  ~ShellJavaScriptDialog();

  // Called to cancel a dialog mid-flight.
  void Cancel();

 private:
  ShellJavaScriptDialogCreator* creator_;
  JavaScriptDialogCreator::DialogClosedCallback callback_;

#if defined(OS_MACOSX)
  ShellJavaScriptDialogHelper* helper_;  // owned
#elif defined(OS_WIN)
  JavaScriptMessageType message_type_;
  HWND dialog_win_;
  string16 message_text_;
  string16 default_prompt_text_;
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

#endif  // CONTENT_SHELL_SHELL_JAVASCRIPT_DIALOG_H_
