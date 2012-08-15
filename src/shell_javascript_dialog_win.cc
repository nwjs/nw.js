// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/shell_javascript_dialog.h"

#include "base/string_util.h"
#include "content/shell/resource.h"
#include "content/shell/shell.h"
#include "content/shell/shell_javascript_dialog_creator.h"

namespace content {

class ShellJavaScriptDialog;

INT_PTR CALLBACK ShellJavaScriptDialog::DialogProc(HWND dialog,
                                                   UINT message,
                                                   WPARAM wparam,
                                                   LPARAM lparam) {
  switch (message) {
    case WM_INITDIALOG: {
      SetWindowLongPtr(dialog, DWL_USER, static_cast<LONG_PTR>(lparam));
      ShellJavaScriptDialog* owner =
          reinterpret_cast<ShellJavaScriptDialog*>(lparam);
      owner->dialog_win_ = dialog;
      SetDlgItemText(dialog, IDC_DIALOGTEXT, owner->message_text_.c_str());
      if (owner->message_type_ == JAVASCRIPT_MESSAGE_TYPE_PROMPT)
        SetDlgItemText(dialog, IDC_PROMPTEDIT,
                       owner->default_prompt_text_.c_str());
      break;
    }
    case WM_DESTROY: {
      ShellJavaScriptDialog* owner = reinterpret_cast<ShellJavaScriptDialog*>(
          GetWindowLongPtr(dialog, DWL_USER));
      if (owner->dialog_win_) {
        owner->dialog_win_ = 0;
        owner->callback_.Run(false, string16());
        owner->creator_->DialogClosed(owner);
      }
      break;
    }
    case WM_COMMAND: {
      ShellJavaScriptDialog* owner = reinterpret_cast<ShellJavaScriptDialog*>(
          GetWindowLongPtr(dialog, DWL_USER));
      string16 user_input;
      bool finish = false;
      bool result;
      switch (LOWORD(wparam)) {
        case IDOK:
          finish = true;
          result = true;
          if (owner->message_type_ == JAVASCRIPT_MESSAGE_TYPE_PROMPT) {
            size_t length =
                GetWindowTextLength(GetDlgItem(dialog, IDC_PROMPTEDIT)) + 1;
            GetDlgItemText(dialog, IDC_PROMPTEDIT,
                           WriteInto(&user_input, length), length);
          }
          break;
        case IDCANCEL:
          finish = true;
          result = false;
          break;
      }
      if (finish) {
        owner->dialog_win_ = 0;
        owner->callback_.Run(result, user_input);
        DestroyWindow(dialog);
        owner->creator_->DialogClosed(owner);
      }
      break;
    }
    default:
      return DefWindowProc(dialog, message, wparam, lparam);
  }
  return 0;
}

ShellJavaScriptDialog::ShellJavaScriptDialog(
    ShellJavaScriptDialogCreator* creator,
    gfx::NativeWindow parent_window,
    JavaScriptMessageType message_type,
    const string16& message_text,
    const string16& default_prompt_text,
    const JavaScriptDialogCreator::DialogClosedCallback& callback)
    : creator_(creator),
      callback_(callback),
      message_text_(message_text),
      default_prompt_text_(default_prompt_text),
      message_type_(message_type) {
  int dialog_type;
  if (message_type == JAVASCRIPT_MESSAGE_TYPE_ALERT)
    dialog_type = IDD_ALERT;
  else if (message_type == JAVASCRIPT_MESSAGE_TYPE_CONFIRM)
    dialog_type = IDD_CONFIRM;
  else // JAVASCRIPT_MESSAGE_TYPE_PROMPT
    dialog_type = IDD_PROMPT;

  dialog_win_ = CreateDialogParam(GetModuleHandle(0),
                                  MAKEINTRESOURCE(dialog_type), 0, DialogProc,
                                  reinterpret_cast<LPARAM>(this));
  ShowWindow(dialog_win_, SW_SHOWNORMAL);
}

ShellJavaScriptDialog::~ShellJavaScriptDialog() {
  Cancel();
}

void ShellJavaScriptDialog::Cancel() {
  if (dialog_win_)
    DestroyWindow(dialog_win_);
}

}  // namespace content
