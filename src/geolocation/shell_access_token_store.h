// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_GEOLOCATION_SHELL_ACCESS_TOKEN_STORE_H_
#define CONTENT_SHELL_GEOLOCATION_SHELL_ACCESS_TOKEN_STORE_H_

#include "content/public/browser/access_token_store.h"

// Dummy access token store used to initialise the network location provider.
class ShellAccessTokenStore : public content::AccessTokenStore {
 public:
  explicit ShellAccessTokenStore(net::URLRequestContextGetter* request_context);

 private:
  virtual ~ShellAccessTokenStore();

  // AccessTokenStore
  virtual void LoadAccessTokens(
      const LoadAccessTokensCallbackType& callback) OVERRIDE;

  virtual void SaveAccessToken(
      const GURL& server_url, const string16& access_token) OVERRIDE;

  static void DidLoadAccessTokens(
      net::URLRequestContextGetter* request_context,
      const LoadAccessTokensCallbackType& callback);

  net::URLRequestContextGetter* request_context_;

  DISALLOW_COPY_AND_ASSIGN(ShellAccessTokenStore);
};

#endif  // CONTENT_SHELL_GEOLOCATION_SHELL_ACCESS_TOKEN_STORE_H_
