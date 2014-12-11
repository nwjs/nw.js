// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EXTENSIONS_SHELL_BROWSER_SHELL_EXTENSIONS_BROWSER_CLIENT_H_
#define EXTENSIONS_SHELL_BROWSER_SHELL_EXTENSIONS_BROWSER_CLIENT_H_

#include "base/compiler_specific.h"
#include "extensions/browser/extensions_browser_client.h"

class PrefService;

namespace extensions {

class ExtensionsAPIClient;
class ShellComponentExtensionResourceManager;
// An ExtensionsBrowserClient that supports a single content::BrowserContent
// with no related incognito context.
class ShellExtensionsBrowserClient : public ExtensionsBrowserClient {
 public:
  // |context| is the single BrowserContext used for IsValidContext() below.
  explicit ShellExtensionsBrowserClient(content::BrowserContext* context);
  ~ShellExtensionsBrowserClient() override;

  // ExtensionsBrowserClient overrides:
  bool IsShuttingDown() override;
  bool AreExtensionsDisabled(const base::CommandLine& command_line,
                             content::BrowserContext* context) override;
  bool IsValidContext(content::BrowserContext* context) override;
  bool IsSameContext(content::BrowserContext* first,
                     content::BrowserContext* second) override;
  bool HasOffTheRecordContext(content::BrowserContext* context) override;
  content::BrowserContext* GetOffTheRecordContext(
      content::BrowserContext* context) override;
  content::BrowserContext* GetOriginalContext(
      content::BrowserContext* context) override;
  bool IsGuestSession(content::BrowserContext* context) const override;
  bool IsExtensionIncognitoEnabled(
      const std::string& extension_id,
      content::BrowserContext* context) const override;
  bool CanExtensionCrossIncognito(
      const Extension* extension,
      content::BrowserContext* context) const override;
  net::URLRequestJob* MaybeCreateResourceBundleRequestJob(
      net::URLRequest* request,
      net::NetworkDelegate* network_delegate,
      const base::FilePath& directory_path,
      const std::string& content_security_policy,
      bool send_cors_header) override;
  bool AllowCrossRendererResourceLoad(net::URLRequest* request,
                                      bool is_incognito,
                                      const Extension* extension,
                                      InfoMap* extension_info_map) override;
  PrefService* GetPrefServiceForContext(
      content::BrowserContext* context) override;
  void GetEarlyExtensionPrefsObservers(
      content::BrowserContext* context,
      std::vector<ExtensionPrefsObserver*>* observers) const override;
  ProcessManagerDelegate* GetProcessManagerDelegate() const override;
  scoped_ptr<ExtensionHostDelegate> CreateExtensionHostDelegate() override;
  bool DidVersionUpdate(content::BrowserContext* context) override;
  void PermitExternalProtocolHandler() override;
  scoped_ptr<AppSorting> CreateAppSorting() override;
  bool IsRunningInForcedAppMode() override;
  ApiActivityMonitor* GetApiActivityMonitor(
      content::BrowserContext* context) override;
  ExtensionSystemProvider* GetExtensionSystemFactory() override;
  void RegisterExtensionFunctions(
      ExtensionFunctionRegistry* registry) const override;
  scoped_ptr<RuntimeAPIDelegate> CreateRuntimeAPIDelegate(
      content::BrowserContext* context) const override;
  ComponentExtensionResourceManager* GetComponentExtensionResourceManager()
      override;
  void BroadcastEventToRenderers(const std::string& event_name,
                                 scoped_ptr<base::ListValue> args) override;
  net::NetLog* GetNetLog() override;
  ExtensionCache* GetExtensionCache() override;
  bool IsBackgroundUpdateAllowed() override;
  bool IsMinBrowserVersionSupported(const std::string& min_version) override;

 private:
  // The single BrowserContext for app_shell. Not owned.
  content::BrowserContext* browser_context_;

  // Support for extension APIs.
  scoped_ptr<ExtensionsAPIClient> api_client_;

  // The PrefService for |browser_context_|.
  scoped_ptr<PrefService> prefs_;

  // The extension cache used for download and installation.
  scoped_ptr<ExtensionCache> extension_cache_;

  scoped_ptr<ShellComponentExtensionResourceManager> resource_manager_;

  DISALLOW_COPY_AND_ASSIGN(ShellExtensionsBrowserClient);
};

}  // namespace extensions

#endif  // EXTENSIONS_SHELL_BROWSER_SHELL_EXTENSIONS_BROWSER_CLIENT_H_
