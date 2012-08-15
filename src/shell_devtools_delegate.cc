// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/shell_devtools_delegate.h"

#include "content/public/browser/devtools_http_handler.h"
#include "grit/shell_resources.h"
#include "net/base/tcp_listen_socket.h"
#include "net/url_request/url_request_context_getter.h"
#include "ui/base/layout.h"
#include "ui/base/resource/resource_bundle.h"

namespace content {

ShellDevToolsDelegate::ShellDevToolsDelegate(
    net::URLRequestContextGetter* context_getter)
    : context_getter_(context_getter) {
  devtools_http_handler_ = DevToolsHttpHandler::Start(
      new net::TCPListenSocketFactory("127.0.0.1", 0),
      "",
      context_getter_,
      this);
}

ShellDevToolsDelegate::~ShellDevToolsDelegate() {
}

void ShellDevToolsDelegate::Stop() {
  // The call below destroys this.
  devtools_http_handler_->Stop();
}

std::string ShellDevToolsDelegate::GetDiscoveryPageHTML() {
  return ResourceBundle::GetSharedInstance().GetRawDataResource(
      IDR_CONTENT_SHELL_DEVTOOLS_DISCOVERY_PAGE,
      ui::SCALE_FACTOR_NONE).as_string();
}

bool ShellDevToolsDelegate::BundlesFrontendResources() {
  return true;
}

std::string ShellDevToolsDelegate::GetFrontendResourcesBaseURL() {
  return "";
}

}  // namespace content
