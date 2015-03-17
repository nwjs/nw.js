// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#include "content/nw/src/browser/shell_web_view_guest_delegate.h"

#include "content/public/browser/child_process_security_policy.h"
#include "content/public/browser/render_process_host.h"
#include "extensions/browser/api/web_request/web_request_api.h"
#include "extensions/browser/guest_view/web_view/web_view_constants.h"

namespace extensions {

ShellWebViewGuestDelegate::ShellWebViewGuestDelegate(
    WebViewGuest* web_view_guest)
    : web_view_guest_(web_view_guest),
      nw_disabled_(false),
      weak_ptr_factory_(this) {
}

ShellWebViewGuestDelegate::~ShellWebViewGuestDelegate() {
}

void ShellWebViewGuestDelegate::OnAttachWebViewHelpers(
    content::WebContents* contents) {
}

void ShellWebViewGuestDelegate::OnDidAttachToEmbedder() {
  web_view_guest_->attach_params()->GetBoolean("nwdisable", &nw_disabled_);
  if (nw_disabled_)
    return;
  content::RenderProcessHost* host =
    web_view_guest_->web_contents()->GetRenderProcessHost();
  content::ChildProcessSecurityPolicy::GetInstance()->GrantScheme(
      host->GetID(), url::kFileScheme);
}

void ShellWebViewGuestDelegate::OnDidCommitProvisionalLoadForFrame(
    bool is_main_frame) {
}

void ShellWebViewGuestDelegate::OnDidInitialize() {
}

void ShellWebViewGuestDelegate::OnDocumentLoadedInFrame(
    content::RenderFrameHost* render_frame_host) {
}


void ShellWebViewGuestDelegate::OnGuestDestroyed() {
}

}  // namespace extensions
