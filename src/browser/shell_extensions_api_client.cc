// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/shell_extensions_api_client.h"

#include "base/files/file_path.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_thread.h"
#include "content/nw/src/browser/shell_web_view_guest_delegate.h"
#include "extensions/browser/api/device_permissions_prompt.h"
#include "extensions/browser/api/virtual_keyboard_private/virtual_keyboard_delegate.h"
#include "extensions/browser/api/web_request/web_request_event_router_delegate.h"
#include "extensions/browser/guest_view/mime_handler_view/mime_handler_view_guest_delegate.h"
#include "extensions/browser/guest_view/web_view/web_view_permission_helper_delegate.h"
#include "extensions/browser/guest_view/web_view/web_view_guest.h"

namespace extensions {

ShellExtensionsAPIClient::ShellExtensionsAPIClient() {}

ShellExtensionsAPIClient::~ShellExtensionsAPIClient() {}

void ShellExtensionsAPIClient::AddAdditionalValueStoreCaches(
    content::BrowserContext* context,
    const scoped_refptr<SettingsStorageFactory>& factory,
    const scoped_refptr<ObserverListThreadSafe<SettingsObserver> >& observers,
    std::map<settings_namespace::Namespace, ValueStoreCache*>* caches) {
}

AppViewGuestDelegate* ShellExtensionsAPIClient::CreateAppViewGuestDelegate()
    const {
  return NULL;
}

double ShellWebViewGuestDelegate::GetZoom() {
  return 1.0;
}

void ShellWebViewGuestDelegate::OnSetZoom(double zoom_factor) {
}

void ShellWebViewGuestDelegate::OnShowContextMenu(
    int request_id,
    const MenuItemVector* items) {
}

bool ShellWebViewGuestDelegate::HandleContextMenu(
    const content::ContextMenuParams& params) {
  return false;
}

ExtensionOptionsGuestDelegate*
ShellExtensionsAPIClient::CreateExtensionOptionsGuestDelegate(
    ExtensionOptionsGuest* guest) const {
  return NULL;
}

scoped_ptr<MimeHandlerViewGuestDelegate>
ShellExtensionsAPIClient::CreateMimeHandlerViewGuestDelegate(
    MimeHandlerViewGuest* guest) const {
  return scoped_ptr<MimeHandlerViewGuestDelegate>();
}

WebViewGuestDelegate* ShellExtensionsAPIClient::CreateWebViewGuestDelegate(
    WebViewGuest* web_view_guest) const {
  return new ShellWebViewGuestDelegate(web_view_guest);
}

WebViewPermissionHelperDelegate* ShellExtensionsAPIClient::
    CreateWebViewPermissionHelperDelegate(
        WebViewPermissionHelper* web_view_permission_helper) const {
  return new WebViewPermissionHelperDelegate(web_view_permission_helper);
}

WebRequestEventRouterDelegate*
ShellExtensionsAPIClient::CreateWebRequestEventRouterDelegate() const {
  return new WebRequestEventRouterDelegate();
}

scoped_refptr<ContentRulesRegistry>
ShellExtensionsAPIClient::CreateContentRulesRegistry(
    content::BrowserContext* browser_context,
    RulesCacheDelegate* cache_delegate) const {
  return scoped_refptr<ContentRulesRegistry>();
}

scoped_ptr<DevicePermissionsPrompt>
ShellExtensionsAPIClient::CreateDevicePermissionsPrompt(
    content::WebContents* web_contents) const {
  return nullptr;
}

scoped_ptr<VirtualKeyboardDelegate>
ShellExtensionsAPIClient::CreateVirtualKeyboardDelegate() const {
  return nullptr;
}

ManagementAPIDelegate* ShellExtensionsAPIClient::CreateManagementAPIDelegate()
    const {
  return nullptr;
}

}  // namespace extensions
