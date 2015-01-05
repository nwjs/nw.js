// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/shell_login_dialog.h"

#include "base/bind.h"
#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/resource_dispatcher_host.h"
#include "content/public/browser/resource_request_info.h"
#include "net/base/auth.h"
#include "net/url_request/url_request.h"
#include "ui/gfx/text_elider.h"

namespace content {

ShellLoginDialog::ShellLoginDialog(
    net::AuthChallengeInfo* auth_info,
    net::URLRequest* request) : auth_info_(auth_info),
                                request_(request) {
#if !defined(OS_MACOSX)
  AddRef();
#endif

#if defined(OS_WIN)
  dialog_ = NULL;
#endif

  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  if (!ResourceRequestInfo::ForRequest(request_)->GetAssociatedRenderFrame(
          &render_process_id_,  &render_frame_id_)) {
    NOTREACHED();
  }
  BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&ShellLoginDialog::PrepDialog, this,
                 base::ASCIIToUTF16(auth_info->challenger.ToString()),
                 base::UTF8ToUTF16(auth_info->realm)));
}

void ShellLoginDialog::OnRequestCancelled() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&ShellLoginDialog::PlatformRequestCancelled, this));
}

void ShellLoginDialog::UserAcceptedAuth(const base::string16& username,
                                        const base::string16& password) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  BrowserThread::PostTask(
      BrowserThread::IO, FROM_HERE,
      base::Bind(&ShellLoginDialog::SendAuthToRequester, this,
                 true, username, password));
}

void ShellLoginDialog::UserCancelledAuth() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  BrowserThread::PostTask(
      BrowserThread::IO, FROM_HERE,
      base::Bind(&ShellLoginDialog::SendAuthToRequester, this,
                 false, base::string16(), base::string16()));
  BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&ShellLoginDialog::PlatformCleanUp, this));
}

ShellLoginDialog::~ShellLoginDialog() {
  // Cannot post any tasks here; this object is going away and cannot be
  // referenced/dereferenced.
}

void ShellLoginDialog::PrepDialog(const base::string16& host,
                                  const base::string16& realm) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  // The realm is controlled by the remote server, so there is no reason to
  // believe it is of a reasonable length.
  base::string16 elided_realm;
  gfx::ElideString(realm, 120, &elided_realm);

  base::string16 explanation =
      base::ASCIIToUTF16("The server ") + host +
      base::ASCIIToUTF16(" requires a username and password.");

  if (!elided_realm.empty()) {
    explanation += base::ASCIIToUTF16(" The server says: ");
    explanation += elided_realm;
    explanation += base::ASCIIToUTF16(".");
  }

  PlatformCreateDialog(explanation);
}

void ShellLoginDialog::SendAuthToRequester(bool success,
                                           const base::string16& username,
                                           const base::string16& password) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  if (success)
    request_->SetAuth(net::AuthCredentials(username, password));
  else
    request_->CancelAuth();
  ResourceDispatcherHost::Get()->ClearLoginDelegateForRequest(request_);

  BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&ShellLoginDialog::PlatformCleanUp, this));
}

void ShellLoginDialog::ReleaseSoon() {
  BrowserThread::ReleaseSoon(BrowserThread::IO, FROM_HERE, this);
}

}  // namespace content
