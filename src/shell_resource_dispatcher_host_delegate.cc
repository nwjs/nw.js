// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/shell_resource_dispatcher_host_delegate.h"

#include "content/shell/shell_login_dialog.h"

namespace content {

ShellResourceDispatcherHostDelegate::ShellResourceDispatcherHostDelegate() {
}

ShellResourceDispatcherHostDelegate::~ShellResourceDispatcherHostDelegate() {
}

bool ShellResourceDispatcherHostDelegate::AcceptAuthRequest(
    net::URLRequest* request,
    net::AuthChallengeInfo* auth_info) {
  // Why not give it a try?
  return true;
}

ResourceDispatcherHostLoginDelegate*
ShellResourceDispatcherHostDelegate::CreateLoginDelegate(
    net::AuthChallengeInfo* auth_info, net::URLRequest* request) {
  if (!login_request_callback_.is_null()) {
    login_request_callback_.Run();
    login_request_callback_.Reset();
    return NULL;
  }

#if !defined(OS_MACOSX) && !defined(TOOLKIT_GTK)
// TODO: implement ShellLoginDialog for other platforms, drop this #if
  return NULL;
#else
  return new ShellLoginDialog(auth_info, request);
#endif
}

}  // namespace content
