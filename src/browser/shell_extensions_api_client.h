// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NW_BROWSER_EXTENSIONS_API_CHROME_EXTENSIONS_API_CLIENT_H_
#define NW_BROWSER_EXTENSIONS_API_CHROME_EXTENSIONS_API_CLIENT_H_

#include "base/compiler_specific.h"
#include "extensions/browser/api/extensions_api_client.h"

namespace extensions {

// Extra support for extensions APIs in Chrome.
class ShellExtensionsAPIClient : public ExtensionsAPIClient {
 public:
  ShellExtensionsAPIClient();
  ~ShellExtensionsAPIClient() override;

  // ExtensionsApiClient implementation.
  void AddAdditionalValueStoreCaches(
      content::BrowserContext* context,
      const scoped_refptr<SettingsStorageFactory>& factory,
      const scoped_refptr<ObserverListThreadSafe<SettingsObserver>>& observers,
      std::map<settings_namespace::Namespace, ValueStoreCache*>* caches)
      override;
  AppViewGuestDelegate* CreateAppViewGuestDelegate() const override;
  ExtensionOptionsGuestDelegate* CreateExtensionOptionsGuestDelegate(
      ExtensionOptionsGuest* guest) const override;
  scoped_ptr<MimeHandlerViewGuestDelegate> CreateMimeHandlerViewGuestDelegate(
      MimeHandlerViewGuest* guest) const override;
  WebViewGuestDelegate* CreateWebViewGuestDelegate(
      WebViewGuest* web_view_guest) const override;
  WebViewPermissionHelperDelegate* CreateWebViewPermissionHelperDelegate(
      WebViewPermissionHelper* web_view_permission_helper) const override;
  WebRequestEventRouterDelegate* CreateWebRequestEventRouterDelegate()
      const override;
  scoped_refptr<ContentRulesRegistry> CreateContentRulesRegistry(
      content::BrowserContext* browser_context,
      RulesCacheDelegate* cache_delegate) const override;
  scoped_ptr<DevicePermissionsPrompt> CreateDevicePermissionsPrompt(
      content::WebContents* web_contents) const override;
  scoped_ptr<VirtualKeyboardDelegate> CreateVirtualKeyboardDelegate()
      const override;
  ManagementAPIDelegate* CreateManagementAPIDelegate() const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(ShellExtensionsAPIClient);
};

}  // namespace extensions

#endif  // CHROME_BROWSER_EXTENSIONS_API_CHROME_EXTENSIONS_API_CLIENT_H_
