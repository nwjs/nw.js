// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/shell_login_dialog.h"

#include "base/bind.h"
#include "base/logging.h"
#include "base/utf_string_conversions.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/resource_dispatcher_host.h"
#include "net/base/auth.h"
#include "net/url_request/url_request.h"
#include "ui/base/text/text_elider.h"

namespace content {

ShellLoginDialog::ShellLoginDialog(
    net::AuthChallengeInfo* auth_info,
    net::URLRequest* request) : auth_info_(auth_info),
                                request_(request) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
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
                 false, string16(), string16()));
  BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&ShellLoginDialog::PlatformCleanUp, this));
}

ShellLoginDialog::~ShellLoginDialog() {
  // Cannot post any tasks here; this object is going away and cannot be
  // referenced/dereferenced.
}

#if !defined(OS_MACOSX) && !defined(TOOLKIT_GTK)
// Bogus implementations for linking. They are never called because
// ResourceDispatcherHostDelegate::CreateLoginDelegate returns NULL.
// TODO: implement ShellLoginDialog for other platforms, drop this #if
void ShellLoginDialog::PlatformCreateDialog(const string16& message) {}
void ShellLoginDialog::PlatformCleanUp() {}
void ShellLoginDialog::PlatformRequestCancelled() {}
#endif

void ShellLoginDialog::PrepDialog(const string16& host,
                                  const string16& realm) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  // The realm is controlled by the remote server, so there is no reason to
  // believe it is of a reasonable length.
  string16 elided_realm;
  ui::ElideString(realm, 120, &elided_realm);

  string16 explanation =
      ASCIIToUTF16("The server ") + host +
      ASCIIToUTF16(" requires a username and password.");

  if (!elided_realm.empty()) {
    explanation += ASCIIToUTF16(" The server says: ");
    explanation += elided_realm;
    explanation += ASCIIToUTF16(".");
  }

  PlatformCreateDialog(explanation);
}

void ShellLoginDialog::SendAuthToRequester(bool success,
                                           const string16& username,
                                           const string16& password) {
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

}  // namespace content
