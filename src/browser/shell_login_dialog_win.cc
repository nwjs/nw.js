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

#include "content/nw/src/browser/shell_login_dialog.h"

#include "base/logging.h"
#include "base/strings/string16.h"
#include "base/strings/utf_string_conversions.h"
#include "content/nw/src/resource.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/resource_dispatcher_host.h"
#include "content/public/browser/resource_request_info.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_view.h"

namespace content {

INT_PTR CALLBACK ShellLoginDialog::DialogProc(HWND dialog,
                                              UINT message,
                                              WPARAM wparam,
                                              LPARAM lparam) {
  switch (message) {
    case WM_INITDIALOG: {
      SetWindowLongPtr(dialog, DWL_USER, static_cast<LONG_PTR>(lparam));
      ShellLoginDialog* owner = reinterpret_cast<ShellLoginDialog*>(lparam);
      owner->dialog_win_ = dialog;
      SetDlgItemText(dialog,
                     IDC_DIALOG_MESSAGETEXT,
                     owner->message_text_.c_str());
      break;
    }
    case WM_CLOSE: {
      ShellLoginDialog* owner = reinterpret_cast<ShellLoginDialog*>(
          GetWindowLongPtr(dialog, DWL_USER));
      owner->UserCancelledAuth();
      DestroyWindow(owner->dialog_win_);
      owner->dialog_win_ = NULL;
      ReleaseSoon();
      break;
    }
    case WM_COMMAND: {
      ShellLoginDialog* owner = reinterpret_cast<ShellLoginDialog*>(
          GetWindowLongPtr(dialog, DWL_USER));
      if (LOWORD(wparam) == IDOK) {
        string16 username;
        string16 password;
        size_t length =
            GetWindowTextLength(GetDlgItem(dialog, IDC_USERNAMEEDIT)) + 1;
        GetDlgItemText(dialog, IDC_USERNAMEEDIT,
                       WriteInto(&username, length), length);
        length = GetWindowTextLength(GetDlgItem(dialog, IDC_PASSWORDEDIT)) + 1;
        GetDlgItemText(dialog, IDC_PASSWORDEDIT,
                       WriteInto(&password, length), length);
        owner->UserAcceptedAuth(username, password);
      } else if (LOWORD(wparam) == IDCANCEL) {
        owner->UserCancelledAuth();
      } else {
        NOTREACHED();
      }

      break;
    }
    default:
      return DefWindowProc(dialog, message, wparam, lparam);
  }

  return 0;
}

void ShellLoginDialog::PlatformCreateDialog(const string16& message) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  int render_process_id;
  int render_view_id;
  if (!ResourceRequestInfo::ForRequest(request_)->GetAssociatedRenderView(
          &render_process_id,  &render_view_id)) {
    NOTREACHED();
  }

  WebContents* web_contents = NULL;
  RenderViewHost* render_view_host =
      RenderViewHost::FromID(render_process_id, render_view_id);
  if (render_view_host)
    web_contents = WebContents::FromRenderViewHost(render_view_host);
  DCHECK(web_contents);

  gfx::NativeWindow parent_window =
      web_contents->GetView()->GetTopLevelNativeWindow();
  message_text_ = message;
  dialog_win_ = CreateDialogParam(GetModuleHandle(0),
                                  MAKEINTRESOURCE(IDD_LOGIN), parent_window,
                                  DialogProc, reinterpret_cast<LPARAM>(this));
}

void ShellLoginDialog::PlatformShowDialog() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  ShowWindow(dialog_win_, SW_SHOWNORMAL);
}

void ShellLoginDialog::PlatformCleanUp() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  if (dialog_win_) {
    DestroyWindow(dialog_win_);
    dialog_win_ = NULL;
  }
}

void ShellLoginDialog::PlatformRequestCancelled() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
}

}  // namespace content
