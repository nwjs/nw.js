// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_SHELL_CONTENT_BROWSER_CLIENT_H_
#define CONTENT_SHELL_SHELL_CONTENT_BROWSER_CLIENT_H_

#include <string>
#include <map>

#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "content/public/browser/content_browser_client.h"
#include "third_party/WebKit/public/platform/WebNotificationPermission.h"

namespace printing {
class PrintJobManager;
}

namespace extensions {
class Extension;
}

namespace content {

class ShellBrowserContext;
class ShellBrowserMainParts;
class ShellResourceDispatcherHostDelegate;
class RenderProcessHost;

class ShellContentBrowserClient : public ContentBrowserClient {
 public:
  ShellContentBrowserClient();
  ~ShellContentBrowserClient() final;

  // ContentBrowserClient overrides.
   BrowserMainParts* CreateBrowserMainParts(
      const MainFunctionParams& parameters) override;
   void OverrideCreateWebContentsView(
      WebContents* web_contents,
      RenderViewHostDelegateView** render_view_host_delegate_view,
      const WebContents::CreateParams& params) override;
   std::string GetApplicationLocale() override;
   void AppendExtraCommandLineSwitches(base::CommandLine* command_line,
                                              int child_process_id) override;
   void ResourceDispatcherHostCreated() override;
   AccessTokenStore* CreateAccessTokenStore() override;
   void OverrideWebkitPrefs(
      RenderViewHost* render_view_host,
      const GURL& url,
      WebPreferences* prefs) override;
   std::string GetDefaultDownloadName() override;
#if 0
   MediaObserver* GetMediaObserver() override;
#endif
   bool CheckMediaAccessPermission(BrowserContext* browser_context,
                                        const GURL& security_origin,
                                        MediaStreamType type) override;

   void BrowserURLHandlerCreated(BrowserURLHandler* handler) override;
   bool IsHandledURL(const GURL& url) override;
  void SiteInstanceGotProcess(content::SiteInstance* site_instance) override;
  void SiteInstanceDeleting(content::SiteInstance* site_instance) override;
   bool ShouldTryToUseExistingProcessHost(
      BrowserContext* browser_context, const GURL& url) override;
   bool IsSuitableHost(RenderProcessHost* process_host,
                              const GURL& site_url) override;
  ShellBrowserContext* browser_context();
  ShellBrowserContext* off_the_record_browser_context();
  ShellResourceDispatcherHostDelegate* resource_dispatcher_host_delegate() {
    return resource_dispatcher_host_delegate_.get();
  }
  ShellBrowserMainParts* shell_browser_main_parts() {
    return shell_browser_main_parts_;
  }
   printing::PrintJobManager* print_job_manager();
   void RenderProcessWillLaunch(RenderProcessHost* host) override;
   net::URLRequestContextGetter* CreateRequestContext(
      BrowserContext* browser_context,
      ProtocolHandlerMap* protocol_handlers, URLRequestInterceptorScopedVector request_interceptors) override;
   net::URLRequestContextGetter* CreateRequestContextForStoragePartition(
      BrowserContext* browser_context,
      const base::FilePath& partition_path,
      bool in_memory,
      ProtocolHandlerMap* protocol_handlers,
      URLRequestInterceptorScopedVector request_interceptors) override;
   void AllowCertificateError(int render_process_id,
                                     int render_frame_id,
                                     int cert_error,
                                     const net::SSLInfo& ssl_info,
                                     const GURL& request_url,
                                     ResourceType resource_type,
                                     bool overridable,
                                     bool strict_enforcement,
                                     bool expired_previous_decision,
                                     const base::Callback<void(bool)>& callback,
                                     CertificateRequestResultType* result) override;
   void GetAdditionalAllowedSchemesForFileSystem(
      std::vector<std::string>* additional_schemes) override;
#if defined(OS_POSIX) && !defined(OS_MACOSX)
  void GetAdditionalMappedFilesForChildProcess(
      const base::CommandLine& command_line,
      int child_process_id,
      FileDescriptorInfo* mappings) override;
#endif
   QuotaPermissionContext* CreateQuotaPermissionContext() override;

  DevToolsManagerDelegate* GetDevToolsManagerDelegate() override;
   bool ShouldUseProcessPerSite(
    content::BrowserContext* browser_context,
    const GURL& effective_url) override;

  PlatformNotificationService* GetPlatformNotificationService() override;

  content::SpeechRecognitionManagerDelegate*
  CreateSpeechRecognitionManagerDelegate() override;

 private:
  ShellBrowserContext* ShellBrowserContextForBrowserContext(
      BrowserContext* content_browser_context);
  bool GetUserAgentManifest(std::string* agent);
  // Returns the extension or app associated with |site_instance| or NULL.
  const extensions::Extension* GetExtension(content::SiteInstance* site_instance);
  scoped_ptr<ShellResourceDispatcherHostDelegate>
      resource_dispatcher_host_delegate_;

  ShellBrowserMainParts* shell_browser_main_parts_;
  content::RenderProcessHost* master_rph_;
};

}  // namespace content

#endif  // CONTENT_SHELL_SHELL_CONTENT_BROWSER_CLIENT_H_
