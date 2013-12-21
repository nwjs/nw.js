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

#include "base/bind.h"
#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/chrome_notification_types.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/notification_registrar.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/resource_dispatcher_host.h"
#include "content/public/browser/resource_request_info.h"
#include "net/base/auth.h"
#include "net/url_request/url_request.h"
#include "ui/gfx/text_elider.h"

using content::BrowserThread;
using content::ResourceDispatcherHost;

namespace {
// Helper to remove the ref from an net::URLRequest to the ShellLoginDialog.
// Should only be called from the IO thread, since it accesses an
// net::URLRequest.
void ResetShellLoginDialogForRequest(net::URLRequest* request) {
  ResourceDispatcherHost::Get()->ClearLoginDelegateForRequest(request);
}

} // namespace

namespace content {

ShellLoginDialog::ShellLoginDialogList ShellLoginDialog::dialog_queue_;

ShellLoginDialog::ShellLoginDialog(
    net::AuthChallengeInfo* auth_info,
    net::URLRequest* request)
  : render_process_id_(0),
    render_view_id_(0),
    auth_info_(auth_info),
    request_(request),
    handled_auth_(false) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));

  if (!ResourceRequestInfo::ForRequest(request_)->GetAssociatedRenderView(
          &render_process_id_,  &render_view_id_)) {
    NOTREACHED();
  }

  // prevent object destruction on wrong (IO) thread
  // paired with ReleaseSoon()
  AddRef();

  BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&ShellLoginDialog::PrepDialog, this,
                 ASCIIToUTF16(auth_info->challenger.ToString()),
                 UTF8ToUTF16(auth_info->realm)));
}

void ShellLoginDialog::OnRequestCancelled() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&ShellLoginDialog::PlatformRequestCancelled, this));
}

void ShellLoginDialog::UserAcceptedAuth(const string16& username,
                                        const string16& password) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  if (TestAndSetAuthHandled())
    return;

  // Calling NotifyAuthSupplied() directly instead of posting a task
  // allows other ShellLoginDialog instances to queue their
  // CloseContentsDeferred() before ours.  Closing dialogs in the
  // opposite order as they were created avoids races where remaining
  // dialogs in the same tab may be briefly displayed to the user
  // before they are removed.
  NotifyAuthSupplied(username, password);

  BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&ShellLoginDialog::CloseContentsDeferred, this));
  BrowserThread::PostTask(
      BrowserThread::IO, FROM_HERE,
      base::Bind(&ShellLoginDialog::SendAuthToRequester, this,
                 true, username, password));
}

void ShellLoginDialog::UserCancelledAuth() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  if (TestAndSetAuthHandled())
    return;

  // Similar to how we deal with notifications above in SetAuth()
  if (BrowserThread::CurrentlyOn(BrowserThread::UI)) {
    NotifyAuthCancelled();
  } else {
    BrowserThread::PostTask(
        BrowserThread::UI, FROM_HERE,
        base::Bind(&ShellLoginDialog::NotifyAuthCancelled, this));
  }

  BrowserThread::PostTask(
      BrowserThread::IO, FROM_HERE,
      base::Bind(&ShellLoginDialog::SendAuthToRequester, this,
                 false, string16(), string16()));
  BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&ShellLoginDialog::CloseContentsDeferred, this));
}

ShellLoginDialog::~ShellLoginDialog() {
  // Cannot post any tasks here; this object is going away and cannot be
  // referenced/dereferenced.
}

void ShellLoginDialog::PrepDialog(const string16& host,
                                  const string16& realm) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  // The realm is controlled by the remote server, so there is no reason to
  // believe it is of a reasonable length.
  string16 elided_realm;
  gfx::ElideString(realm, 120, &elided_realm);

  string16 explanation =
      ASCIIToUTF16("The server ") + host +
      ASCIIToUTF16(" requires a username and password.");

  if (!elided_realm.empty()) {
    explanation += ASCIIToUTF16(" The server says: ");
    explanation += elided_realm;
    explanation += ASCIIToUTF16(".");
  }

  AddObservers();
  dialog_queue_.push_back(this);
  PlatformCreateDialog(explanation);
  if (dialog_queue_.size() == 1)
    PlatformShowDialog();
}

void ShellLoginDialog::HandleQueueOnClose()
{
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  ShellLoginDialogList::iterator i(
      std::find(dialog_queue_.begin(), dialog_queue_.end(), this));

  // Ignore the second invocation.
  if (i == dialog_queue_.end())
    return;

  bool removed_topmost_dialog = i == dialog_queue_.begin();
  dialog_queue_.erase(i);
  if (!dialog_queue_.empty() && removed_topmost_dialog) {
    ShellLoginDialog* next_dlg = dialog_queue_.front();
    next_dlg->PlatformShowDialog();
  }
}

void ShellLoginDialog::SendAuthToRequester(bool success,
                                           const string16& username,
                                           const string16& password) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  if (success) {
    request_->SetAuth(net::AuthCredentials(username, password));
  } else
    request_->CancelAuth();
  ResourceDispatcherHost::Get()->ClearLoginDelegateForRequest(request_);
}

void ShellLoginDialog::NotifyAuthSupplied(const string16& username,
                                          const string16& password) {

  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  content::NotificationService* service =
      content::NotificationService::current();

  AuthSuppliedLoginNotificationDetails details(this, username, password);

  service->Notify(chrome::NOTIFICATION_AUTH_SUPPLIED,
                  content::Source<ShellLoginDialog>(this),
                  content::Details<AuthSuppliedLoginNotificationDetails>(&details));
}

void ShellLoginDialog::NotifyAuthCancelled() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(WasAuthHandled());

  content::NotificationService* service =
      content::NotificationService::current();

  AuthSuppliedLoginNotificationDetails details(this, string16(), string16());

  service->Notify(chrome::NOTIFICATION_AUTH_CANCELLED,
                  content::Source<ShellLoginDialog>(this),
                  content::Details<LoginNotificationDetails>(&details));
}


void ShellLoginDialog::Observe(int type,
                               const content::NotificationSource& source,
                               const content::NotificationDetails& details) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(type == chrome::NOTIFICATION_AUTH_SUPPLIED ||
         type == chrome::NOTIFICATION_AUTH_CANCELLED);

  // Break out early if we aren't interested in the notification.
  if (WasAuthHandled())
    return;

  LoginNotificationDetails* login_details =
      content::Details<LoginNotificationDetails>(details).ptr();

  // WasAuthHandled() should always test positive before we publish
  // AUTH_SUPPLIED or AUTH_CANCELLED notifications.
  DCHECK(login_details->handler() != this);

  // Only handle notification for the identical auth info.
  if (!login_details->handler()->auth_info()->Equals(*auth_info()))
    return;

  // Set or cancel the auth in this handler.
  if (type == chrome::NOTIFICATION_AUTH_SUPPLIED) {
    AuthSuppliedLoginNotificationDetails* supplied_details =
        content::Details<AuthSuppliedLoginNotificationDetails>(details).ptr();
    UserAcceptedAuth(supplied_details->username(), supplied_details->password());
  } else {
    DCHECK(type == chrome::NOTIFICATION_AUTH_CANCELLED);
    UserCancelledAuth();
  }
}

// Returns whether authentication had been handled (SetAuth or CancelAuth).
bool ShellLoginDialog::WasAuthHandled() const {
  base::AutoLock lock(handled_auth_lock_);
  bool was_handled = handled_auth_;
  return was_handled;
}

// Marks authentication as handled and returns the previous handled state.
bool ShellLoginDialog::TestAndSetAuthHandled() {
  base::AutoLock lock(handled_auth_lock_);
  bool was_handled = handled_auth_;
  handled_auth_ = true;
  return was_handled;
}

// Closes the view_contents from the UI loop.
void ShellLoginDialog::CloseContentsDeferred() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  HandleQueueOnClose();
  PlatformCleanUp();
}

void ShellLoginDialog::AddObservers() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  // This is probably OK; we need to listen to everything and we break out of
  // the Observe() if we aren't handling the same auth_info().
  registrar_.reset(new content::NotificationRegistrar);
  registrar_->Add(this, chrome::NOTIFICATION_AUTH_SUPPLIED,
                  content::NotificationService::AllBrowserContextsAndSources());
  registrar_->Add(this, chrome::NOTIFICATION_AUTH_CANCELLED,
                  content::NotificationService::AllBrowserContextsAndSources());
}

void ShellLoginDialog::RemoveObservers() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  registrar_.reset();
}

void ShellLoginDialog::ReleaseSoon() {
  if (!TestAndSetAuthHandled()) {
    BrowserThread::PostTask(
        BrowserThread::IO, FROM_HERE,
        base::Bind(&ShellLoginDialog::CancelAuthDeferred, this));
    BrowserThread::PostTask(
        BrowserThread::UI, FROM_HERE,
        base::Bind(&ShellLoginDialog::NotifyAuthCancelled, this));
  }

  BrowserThread::PostTask(
    BrowserThread::UI, FROM_HERE,
    base::Bind(&ShellLoginDialog::RemoveObservers, this));

  // Delete this object once all InvokeLaters have been called.
  BrowserThread::ReleaseSoon(BrowserThread::IO, FROM_HERE, this);
}

// Calls SetAuth from the IO loop.
void ShellLoginDialog::SetAuthDeferred(const string16& username,
                                       const string16& password) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));

  if (request_) {
    request_->SetAuth(net::AuthCredentials(username, password));
    ResetShellLoginDialogForRequest(request_);
  }
}

// Calls CancelAuth from the IO loop.
void ShellLoginDialog::CancelAuthDeferred() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));

  if (request_) {
    request_->CancelAuth();
    // Verify that CancelAuth doesn't destroy the request via our delegate.
    DCHECK(request_ != NULL);
    ResetShellLoginDialogForRequest(request_);
  }
}

}  // namespace content
