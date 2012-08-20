// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/geolocation/shell_access_token_store.h"

#include "base/bind.h"
#include "base/message_loop.h"
#include "base/utf_string_conversions.h"

ShellAccessTokenStore::ShellAccessTokenStore(
    net::URLRequestContextGetter* request_context)
    : request_context_(request_context) {
}

ShellAccessTokenStore::~ShellAccessTokenStore() {
}

void ShellAccessTokenStore::LoadAccessTokens(
    const LoadAccessTokensCallbackType& callback) {
  MessageLoop::current()->PostTask(
      FROM_HERE,
      base::Bind(&ShellAccessTokenStore::DidLoadAccessTokens,
                 request_context_, callback));
}

void ShellAccessTokenStore::DidLoadAccessTokens(
    net::URLRequestContextGetter* request_context,
    const LoadAccessTokensCallbackType& callback) {
  // Since content_shell is a test executable, rather than an end user program,
  // we provide a dummy access_token set to avoid hitting the server.
  AccessTokenSet access_token_set;
  access_token_set[GURL()] = ASCIIToUTF16("chromium_content_shell");
  callback.Run(access_token_set, request_context);
}

void ShellAccessTokenStore::SaveAccessToken(
    const GURL& server_url, const string16& access_token) {
}
