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

namespace content {

class ShellBrowserContext;
class ShellBrowserMainParts;
class ShellResourceDispatcherHostDelegate;

class ShellContentBrowserClient : public ContentBrowserClient {
 public:
  ShellContentBrowserClient();
  virtual ~ShellContentBrowserClient();

  // ContentBrowserClient overrides.
  virtual BrowserMainParts* CreateBrowserMainParts(
      const MainFunctionParams& parameters) OVERRIDE;
  virtual void RenderViewHostCreated(
      RenderViewHost* render_view_host) OVERRIDE;
  virtual WebContentsView* OverrideCreateWebContentsView(
      WebContents* web_contents,
      RenderViewHostDelegateView** render_view_host_delegate_view) OVERRIDE;
  virtual std::string GetApplicationLocale() OVERRIDE;
  virtual void AppendExtraCommandLineSwitches(CommandLine* command_line,
                                              int child_process_id) OVERRIDE;
  virtual void ResourceDispatcherHostCreated() OVERRIDE;
  virtual AccessTokenStore* CreateAccessTokenStore() OVERRIDE;
  virtual void OverrideWebkitPrefs(
      RenderViewHost* render_view_host,
      const GURL& url,
      webkit_glue::WebPreferences* prefs) OVERRIDE;
  virtual std::string GetDefaultDownloadName() OVERRIDE;
  virtual MediaObserver* GetMediaObserver() OVERRIDE;
  virtual void BrowserURLHandlerCreated(BrowserURLHandler* handler) OVERRIDE;
  virtual bool ShouldTryToUseExistingProcessHost(
      BrowserContext* browser_context, const GURL& url) OVERRIDE;
  virtual bool IsSuitableHost(RenderProcessHost* process_host,
                              const GURL& site_url) OVERRIDE;
  ShellBrowserContext* browser_context();
  ShellBrowserContext* off_the_record_browser_context();
  ShellResourceDispatcherHostDelegate* resource_dispatcher_host_delegate() {
    return resource_dispatcher_host_delegate_.get();
  }
  ShellBrowserMainParts* shell_browser_main_parts() {
    return shell_browser_main_parts_;
  }

 private:
  scoped_ptr<ShellResourceDispatcherHostDelegate>
      resource_dispatcher_host_delegate_;

  ShellBrowserMainParts* shell_browser_main_parts_;
};

}  // namespace content

#endif  // CONTENT_SHELL_SHELL_CONTENT_BROWSER_CLIENT_H_
