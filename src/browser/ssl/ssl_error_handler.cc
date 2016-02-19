// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/ssl/ssl_error_handler.h"

#include "base/metrics/field_trial.h"
#include "base/time/time.h"
#include "content/nw/src/browser/ssl/ssl_blocking_page.h"
#include "content/public/browser/web_contents.h"

DEFINE_WEB_CONTENTS_USER_DATA_KEY(SSLErrorHandler);

void SSLErrorHandler::HandleSSLError(
    content::WebContents* web_contents,
    int cert_error,
    const net::SSLInfo& ssl_info,
    const GURL& request_url,
    int options_mask,
    const base::Callback<void(bool)>& callback) {
  DCHECK(!FromWebContents(web_contents));
  web_contents->SetUserData(UserDataKey(),
                            new SSLErrorHandler(web_contents, cert_error,
                                                ssl_info, request_url,
                                                options_mask, callback));

  SSLErrorHandler* error_handler =
      SSLErrorHandler::FromWebContents(web_contents);
  error_handler->StartHandlingError();
}

SSLErrorHandler::SSLErrorHandler(content::WebContents* web_contents,
                                 int cert_error,
                                 const net::SSLInfo& ssl_info,
                                 const GURL& request_url,
                                 int options_mask,
                                 const base::Callback<void(bool)>& callback)
    : web_contents_(web_contents),
      cert_error_(cert_error),
      ssl_info_(ssl_info),
      request_url_(request_url),
      options_mask_(options_mask),
      callback_(callback) {
}

SSLErrorHandler::~SSLErrorHandler() {
}

void SSLErrorHandler::StartHandlingError() {
  ShowSSLInterstitial();
}

void SSLErrorHandler::ShowSSLInterstitial() {
  // Show SSL blocking page. The interstitial owns the blocking page.
  (new SSLBlockingPage(web_contents_, cert_error_, ssl_info_, request_url_,
                       options_mask_, callback_))->Show();
  // Once an interstitial is displayed, no need to keep the handler around.
  // This is the equivalent of "delete this".
  web_contents_->RemoveUserData(UserDataKey());
}
