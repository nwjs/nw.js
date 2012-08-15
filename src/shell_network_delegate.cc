// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/shell_network_delegate.h"

#include "net/base/net_errors.h"

namespace content {

ShellNetworkDelegate::ShellNetworkDelegate() {
}

ShellNetworkDelegate::~ShellNetworkDelegate() {
}

int ShellNetworkDelegate::OnBeforeURLRequest(
    net::URLRequest* request,
    const net::CompletionCallback& callback,
    GURL* new_url) {
  return net::OK;
}

int ShellNetworkDelegate::OnBeforeSendHeaders(
    net::URLRequest* request,
    const net::CompletionCallback& callback,
    net::HttpRequestHeaders* headers) {
  return net::OK;
}

void ShellNetworkDelegate::OnSendHeaders(
    net::URLRequest* request,
    const net::HttpRequestHeaders& headers) {
}

int ShellNetworkDelegate::OnHeadersReceived(
    net::URLRequest* request,
    const net::CompletionCallback& callback,
    net::HttpResponseHeaders* original_response_headers,
    scoped_refptr<net::HttpResponseHeaders>* override_response_headers) {
  return net::OK;
}

void ShellNetworkDelegate::OnBeforeRedirect(net::URLRequest* request,
                                            const GURL& new_location) {
}

void ShellNetworkDelegate::OnResponseStarted(net::URLRequest* request) {
}

void ShellNetworkDelegate::OnRawBytesRead(const net::URLRequest& request,
                                          int bytes_read) {
}

void ShellNetworkDelegate::OnCompleted(net::URLRequest* request, bool started) {
}

void ShellNetworkDelegate::OnURLRequestDestroyed(net::URLRequest* request) {
}

void ShellNetworkDelegate::OnPACScriptError(int line_number,
                                            const string16& error) {
}

ShellNetworkDelegate::AuthRequiredResponse ShellNetworkDelegate::OnAuthRequired(
    net::URLRequest* request,
    const net::AuthChallengeInfo& auth_info,
    const AuthCallback& callback,
    net::AuthCredentials* credentials) {
  return AUTH_REQUIRED_RESPONSE_NO_ACTION;
}

bool ShellNetworkDelegate::OnCanGetCookies(const net::URLRequest& request,
                                           const net::CookieList& cookie_list) {
  return true;
}

bool ShellNetworkDelegate::OnCanSetCookie(const net::URLRequest& request,
                                          const std::string& cookie_line,
                                          net::CookieOptions* options) {
  return true;
}

bool ShellNetworkDelegate::OnCanAccessFile(const net::URLRequest& request,
                                           const FilePath& path) const {
  return true;
}

bool ShellNetworkDelegate::OnCanThrottleRequest(
    const net::URLRequest& request) const {
  return false;
}

int ShellNetworkDelegate::OnBeforeSocketStreamConnect(
    net::SocketStream* socket,
    const net::CompletionCallback& callback) {
  return net::OK;
}

void ShellNetworkDelegate::OnCacheWaitStateChange(
    const net::URLRequest& request,
    CacheWaitState waiting) {
}

}  // namespace content
