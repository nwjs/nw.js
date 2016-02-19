// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NWJS_BROWSER_SSL_SSL_ERROR_HANDLER_H_
#define NWJS_BROWSER_SSL_SSL_ERROR_HANDLER_H_

#include <string>

#include "base/callback_forward.h"
#include "base/macros.h"
#include "content/public/browser/web_contents_user_data.h"
#include "net/ssl/ssl_info.h"
#include "url/gurl.h"

namespace content {
class RenderViewHost;
class WebContents;
}

// This class is responsible for deciding whether to show an SSL warning or a
// captive portal error page. It makes this decision by delaying the display of
// SSL interstitial for a few seconds (2 by default), and waiting for a captive
// portal result to arrive during this window. If a captive portal detected
// result arrives in this window, a captive portal error page is shown.
// Otherwise, an SSL interstitial is shown.
//
// An SSLErrorHandler is associated with a particular WebContents, and is
// deleted if the WebContents is destroyed, or an interstitial is displayed.
// It should only be used on the UI thread because its implementation uses
// captive_portal::CaptivePortalService which can only be accessed on the UI
// thread.
class SSLErrorHandler : public content::WebContentsUserData<SSLErrorHandler> {
 public:

  static void HandleSSLError(content::WebContents* web_contents,
                             int cert_error,
                             const net::SSLInfo& ssl_info,
                             const GURL& request_url,
                             int options_mask,
                             const base::Callback<void(bool)>& callback);

 protected:
  SSLErrorHandler(content::WebContents* web_contents,
                  int cert_error,
                  const net::SSLInfo& ssl_info,
                  const GURL& request_url,
                  int options_mask,
                  const base::Callback<void(bool)>& callback);

  ~SSLErrorHandler() override;

  // Called when an SSL cert error is encountered. Triggers a captive portal
  // check and fires a one shot timer to wait for a "captive portal detected"
  // result to arrive.
  void StartHandlingError();

 private:
  virtual void ShowSSLInterstitial();

  content::WebContents* web_contents_;
  const int cert_error_;
  const net::SSLInfo ssl_info_;
  const GURL request_url_;
  const int options_mask_;
  const base::Callback<void(bool)> callback_;

  DISALLOW_COPY_AND_ASSIGN(SSLErrorHandler);
};

#endif  // NWJS_BROWSER_SSL_SSL_ERROR_HANDLER_H_
