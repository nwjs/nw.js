// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_SHELL_BROWSER_CONTEXT_H_
#define CONTENT_SHELL_SHELL_BROWSER_CONTEXT_H_

#include "base/compiler_specific.h"
#include "base/files/file_path.h"
#include "base/files/scoped_temp_dir.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/content_browser_client.h"
#include "net/url_request/url_request_job_factory.h"

namespace nw {
class Package;
class NwFormDatabaseService;
}

namespace extensions {
  class InfoMap;
}

namespace content {

using base::FilePath;

class DownloadManagerDelegate;
class ResourceContext;
class ShellDownloadManagerDelegate;
class ShellURLRequestContextGetter;
class SSLHostStateDelegate;

class ShellBrowserContext : public BrowserContext {
 public:
  explicit ShellBrowserContext(bool off_the_record,
                               nw::Package* package);
   ~ShellBrowserContext() final;

  // BrowserContext implementation.
   FilePath GetPath() const override;
   bool IsOffTheRecord() const override;
   DownloadManagerDelegate* GetDownloadManagerDelegate() override;
   net::URLRequestContextGetter* GetRequestContext() override;
   net::URLRequestContextGetter* GetRequestContextForRenderProcess(
      int renderer_child_id) override;
   net::URLRequestContextGetter* GetMediaRequestContext() override;
   net::URLRequestContextGetter* GetMediaRequestContextForRenderProcess(
      int renderer_child_id) override;
   net::URLRequestContextGetter*
      GetMediaRequestContextForStoragePartition(
          const FilePath& partition_path,
          bool in_memory) override;
   ResourceContext* GetResourceContext() override;
   storage::SpecialStoragePolicy* GetSpecialStoragePolicy() override;
   BrowserPluginGuestManager* GetGuestManager() override;
   PushMessagingService* GetPushMessagingService() override;
   SSLHostStateDelegate* GetSSLHostStateDelegate() override;
  scoped_ptr<ZoomLevelDelegate> CreateZoomLevelDelegate(
      const base::FilePath& partition_path) override;

  nw::NwFormDatabaseService* GetFormDatabaseService();

  // Maps to BrowserMainParts::PreMainMessageLoopRun.
  void PreMainMessageLoopRun();

  net::URLRequestContextGetter* CreateRequestContext(
        ProtocolHandlerMap* protocol_handlers,
        URLRequestInterceptorScopedVector request_interceptors,
        extensions::InfoMap*);
  net::URLRequestContextGetter* CreateRequestContextForStoragePartition(
      const base::FilePath& partition_path,
      bool in_memory,
      ProtocolHandlerMap* protocol_handlers,
      URLRequestInterceptorScopedVector request_interceptors);

  bool pinning_renderer() { return !disable_pinning_renderer_; }
  void set_pinning_renderer(bool val) { disable_pinning_renderer_ = !val; }

 private:
  class ShellResourceContext;

  // Performs initialization of the ShellBrowserContext while IO is still
  // allowed on the current thread.
  void InitWhileIOAllowed();

  bool disable_pinning_renderer_;  // whether dev reload is in process
                                   // or we want to disable pinning
                                   // temporarily
  bool off_the_record_;
  bool ignore_certificate_errors_;
  nw::Package* package_;
  FilePath path_;
  scoped_ptr<ShellResourceContext> resource_context_;
  scoped_refptr<ShellDownloadManagerDelegate> download_manager_delegate_;
  SSLHostStateDelegate* ssl_host_state_delegate_;
  scoped_refptr<ShellURLRequestContextGetter> url_request_getter_;
  scoped_ptr<nw::NwFormDatabaseService> form_database_service_;

  DISALLOW_COPY_AND_ASSIGN(ShellBrowserContext);
};

}  // namespace content

#endif  // CONTENT_SHELL_SHELL_BROWSER_CONTEXT_H_
