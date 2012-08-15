// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/shell_devtools_delegate.h"

#include "base/stringprintf.h"
#include "content/public/browser/android/devtools_auth.h"
#include "content/public/browser/devtools_http_handler.h"
#include "grit/shell_resources.h"
#include "net/base/unix_domain_socket_posix.h"
#include "net/url_request/url_request_context_getter.h"
#include "ui/base/layout.h"
#include "ui/base/resource/resource_bundle.h"

namespace {

// TODO(mnaganov): This hardcoded version should be replaced with the webkit
// revision of this build of content shell. This requires a feature addition
// to the devtools frontend.
const char* kFrontendVersion = "21.0.1175.0";
const char kSocketName[] = "content_shell_devtools_remote";
const char kFrontEndURL[] =
    "http://chrome-devtools-frontend.appspot.com/static/%s/devtools.html";

}

namespace content {

ShellDevToolsDelegate::ShellDevToolsDelegate(
    net::URLRequestContextGetter* context_getter)
    : context_getter_(context_getter) {
  devtools_http_handler_ = DevToolsHttpHandler::Start(
      new net::UnixDomainSocketWithAbstractNamespaceFactory(
          kSocketName,
          base::Bind(&CanUserConnectToDevTools)),
      StringPrintf(kFrontEndURL, kFrontendVersion),
      context_getter,
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
  return false;
}

std::string ShellDevToolsDelegate::GetFrontendResourcesBaseURL() {
  return "";
}

}  // namespace content
