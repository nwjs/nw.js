// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "extensions/shell/browser/shell_extension_host_delegate.h"

#include "base/logging.h"
#include "extensions/shell/browser/media_capture_util.h"
#include "extensions/shell/browser/shell_extension_web_contents_observer.h"

namespace extensions {

ShellExtensionHostDelegate::ShellExtensionHostDelegate() {
}

ShellExtensionHostDelegate::~ShellExtensionHostDelegate() {
}

void ShellExtensionHostDelegate::OnExtensionHostCreated(
    content::WebContents* web_contents) {
  ShellExtensionWebContentsObserver::CreateForWebContents(web_contents);
}

void ShellExtensionHostDelegate::OnRenderViewCreatedForBackgroundPage(
    ExtensionHost* host) {
}

content::JavaScriptDialogManager*
ShellExtensionHostDelegate::GetJavaScriptDialogManager() {
  // TODO(jamescook): Create a JavaScriptDialogManager or reuse the one from
  // content_shell.
  NOTREACHED();
  return NULL;
}

void ShellExtensionHostDelegate::CreateTab(content::WebContents* web_contents,
                                           const std::string& extension_id,
                                           WindowOpenDisposition disposition,
                                           const gfx::Rect& initial_pos,
                                           bool user_gesture) {
  // TODO(jamescook): Should app_shell support opening popup windows?
  NOTREACHED();
}

void ShellExtensionHostDelegate::ProcessMediaAccessRequest(
    content::WebContents* web_contents,
    const content::MediaStreamRequest& request,
    const content::MediaResponseCallback& callback,
    const Extension* extension) {
  // Allow access to the microphone and/or camera.
  media_capture_util::GrantMediaStreamRequest(
      web_contents, request, callback, extension);
}

bool ShellExtensionHostDelegate::CheckMediaAccessPermission(
    content::WebContents* web_contents,
    const GURL& security_origin,
    content::MediaStreamType type,
    const Extension* extension) {
  media_capture_util::VerifyMediaAccessPermission(type, extension);
  return true;
}

}  // namespace extensions
