// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/geolocation/shell_access_token_store.h"

#include "base/bind.h"
#include "base/message_loop.h"
#include "base/utf_string_conversions.h"
#include "content/public/browser/browser_thread.h"
#include "content/nw/src/shell_browser_context.h"

namespace content {

ShellAccessTokenStore::ShellAccessTokenStore(
    content::ShellBrowserContext* shell_browser_context)
    : shell_browser_context_(shell_browser_context),
      system_request_context_(NULL) {
}

ShellAccessTokenStore::~ShellAccessTokenStore() {
}

void ShellAccessTokenStore::LoadAccessTokens(
    const LoadAccessTokensCallbackType& callback) {
  BrowserThread::PostTaskAndReply(
      BrowserThread::UI,
      FROM_HERE,
      base::Bind(&ShellAccessTokenStore::GetRequestContextOnUIThread,
                 this,
                 shell_browser_context_),
      base::Bind(&ShellAccessTokenStore::RespondOnOriginatingThread,
                 this,
                 callback));
}

void ShellAccessTokenStore::GetRequestContextOnUIThread(
    content::ShellBrowserContext* shell_browser_context) {
  system_request_context_ = shell_browser_context->GetRequestContext();
}

void ShellAccessTokenStore::RespondOnOriginatingThread(
    const LoadAccessTokensCallbackType& callback) {
  // Since content_shell is a test executable, rather than an end user program,
  // we provide a dummy access_token set to avoid hitting the server.
  AccessTokenSet access_token_set;
  access_token_set[GURL()] = ASCIIToUTF16("chromium_content_shell");
  callback.Run(access_token_set, system_request_context_);
}

void ShellAccessTokenStore::SaveAccessToken(
    const GURL& server_url, const string16& access_token) {
}

}  // namespace content
