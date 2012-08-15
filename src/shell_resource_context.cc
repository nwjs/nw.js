// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/shell_resource_context.h"
#include "content/shell/shell_url_request_context_getter.h"

namespace content {

ShellResourceContext::ShellResourceContext(
    ShellURLRequestContextGetter* getter)
    : getter_(getter) {
}

ShellResourceContext::~ShellResourceContext() {
}

net::HostResolver* ShellResourceContext::GetHostResolver() {
  return getter_->host_resolver();
}

net::URLRequestContext* ShellResourceContext::GetRequestContext() {
  return getter_->GetURLRequestContext();
}

}  // namespace content
