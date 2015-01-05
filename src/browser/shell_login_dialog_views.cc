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
#include "components/web_modal/web_contents_modal_dialog_host.h"
#include "components/web_modal/web_contents_modal_dialog_manager.h"
#include "components/web_modal/web_contents_modal_dialog_manager_delegate.h"
#include "content/nw/src/resource.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/resource_dispatcher_host.h"
#include "content/public/browser/resource_request_info.h"
#include "content/public/browser/web_contents.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/views/widget/widget.h"

using web_modal::WebContentsModalDialogManager;
using web_modal::WebContentsModalDialogManagerDelegate;

#if 0
namespace {

// The captive portal dialog is system-modal, but uses the web-content-modal
// dialog manager (odd) and requires this atypical dialog widget initialization.
views::Widget* CreateWindowAsFramelessChild(views::WidgetDelegate* delegate,
                                            gfx::NativeView parent) {
  views::Widget* widget = new views::Widget;

  views::Widget::InitParams params;
  params.delegate = delegate;
  params.child = true;
  params.parent = parent;
  params.remove_standard_frame = true;
  params.opacity = views::Widget::InitParams::TRANSLUCENT_WINDOW;

  widget->Init(params);
  return widget;
}

}  // namespace
#endif

namespace content {

void ShellLoginDialog::PlatformCreateDialog(const base::string16& message) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  login_view_ = new LoginView(message);

  // Scary thread safety note: This can potentially be called *after* SetAuth
  // or CancelAuth (say, if the request was cancelled before the UI thread got
  // control).  However, that's OK since any UI interaction in those functions
  // will occur via an InvokeLater on the UI thread, which is guaranteed
  // to happen after this is called (since this was InvokeLater'd first).
  content::RenderFrameHost* rfh = content::RenderFrameHost::FromID(
      render_process_id_, render_frame_id_);

  WebContents* requesting_contents = WebContents::FromRenderFrameHost(rfh);
  WebContentsModalDialogManager* web_contents_modal_dialog_manager =
    WebContentsModalDialogManager::FromWebContents(requesting_contents);
  WebContentsModalDialogManagerDelegate* modal_delegate =
    web_contents_modal_dialog_manager->delegate();
  CHECK(modal_delegate);
  dialog_ = views::DialogDelegate::CreateDialogWidget(
                                                      this, NULL, modal_delegate->GetWebContentsModalDialogHost()->GetHostView());
  web_modal::WebContentsModalDialogManager::FromWebContents(requesting_contents)->
      ShowModalDialog(dialog_->GetNativeWindow());
}

#if 0
void ShellLoginDialog::PlatformShowDialog() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
}
#endif

void ShellLoginDialog::PlatformCleanUp() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  if (dialog_)
    dialog_->Close();
}

void ShellLoginDialog::PlatformRequestCancelled() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
}


base::string16 ShellLoginDialog::GetDialogButtonLabel(
      ui::DialogButton button) const {
  if (button == ui::DIALOG_BUTTON_OK)
    return base::ASCIIToUTF16("Log In");
  return DialogDelegate::GetDialogButtonLabel(button);
}

base::string16 ShellLoginDialog::GetWindowTitle() const {
  return base::ASCIIToUTF16("Authentication Required");
}

void ShellLoginDialog::WindowClosing() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  content::RenderFrameHost* rfh = content::RenderFrameHost::FromID(
      render_process_id_, render_frame_id_);

  WebContents* tab = WebContents::FromRenderFrameHost(rfh);

  if (tab)
    tab->GetRenderViewHost()->SetIgnoreInputEvents(false);

  // Reference is no longer valid.
  dialog_ = NULL;

  // UserCancelledAuth();
}

void ShellLoginDialog::DeleteDelegate() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  // The widget is going to delete itself; clear our pointer.
  dialog_ = NULL;

  ReleaseSoon();
}

ui::ModalType ShellLoginDialog::GetModalType() const {
  return ui::MODAL_TYPE_CHILD;
}

bool ShellLoginDialog::Cancel() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  UserCancelledAuth();
  return true;
}

bool ShellLoginDialog::Accept() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  UserAcceptedAuth(login_view_->GetUsername(), login_view_->GetPassword());
  return true;
}

views::View* ShellLoginDialog::GetInitiallyFocusedView() {
  return login_view_->GetInitiallyFocusedView();
}

views::View* ShellLoginDialog::GetContentsView() {
  return login_view_;
}
views::Widget* ShellLoginDialog::GetWidget() {
  return login_view_->GetWidget();
}
const views::Widget* ShellLoginDialog::GetWidget() const {
  return login_view_->GetWidget();
}

}  // namespace content
