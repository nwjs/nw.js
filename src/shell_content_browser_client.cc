// Copyright (c) 2012 Intel Corp
// Copyright (c) 2012 The Chromium Authors
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell co
// pies of the Software, and to permit persons to whom the Software is furnished
//  to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in al
// l copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IM
// PLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNES
// S FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
//  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WH
// ETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "content/nw/src/shell_content_browser_client.h"

#include "base/command_line.h"
#include "base/file_path.h"
#include "base/file_util.h"
#include "base/threading/thread_restrictions.h"
#include "content/public/browser/browser_url_handler.h"
#include "content/public/browser/resource_dispatcher_host.h"
#include "content/nw/src/api/dispatcher_host.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/browser/shell_devtools_delegate.h"
#include "content/nw/src/browser/shell_resource_dispatcher_host_delegate.h"
#include "content/nw/src/media/media_internals.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/shell.h"
#include "content/nw/src/shell_browser_context.h"
#include "content/nw/src/shell_browser_main_parts.h"
#include "geolocation/shell_access_token_store.h"
#include "googleurl/src/gurl.h"
#include "webkit/glue/webpreferences.h"
#include "ui/base/l10n/l10n_util.h"

namespace content {

ShellContentBrowserClient::ShellContentBrowserClient()
    : shell_browser_main_parts_(NULL) {
}

ShellContentBrowserClient::~ShellContentBrowserClient() {
}

BrowserMainParts* ShellContentBrowserClient::CreateBrowserMainParts(
    const MainFunctionParams& parameters) {
  shell_browser_main_parts_ = new ShellBrowserMainParts(parameters);
  return shell_browser_main_parts_;
}

void ShellContentBrowserClient::RenderViewHostCreated(
    RenderViewHost* render_view_host) {
  new api::DispatcherHost(render_view_host);
}

std::string ShellContentBrowserClient::GetApplicationLocale() {
  return l10n_util::GetApplicationLocale("en-US");
}

void ShellContentBrowserClient::AppendExtraCommandLineSwitches(
    CommandLine* command_line,
    int child_process_id) {
  if (nw::GetManifest() && nw::GetUseNode()) {
    // Allow node.js
    command_line->AppendSwitch(switches::kmNodejs);

    // Set cwd
    command_line->AppendSwitchPath(switches::kWorkingDirectory,
                                   nw::GetPackageRoot());
  }
}

void ShellContentBrowserClient::ResourceDispatcherHostCreated() {
  resource_dispatcher_host_delegate_.reset(
      new ShellResourceDispatcherHostDelegate());
  ResourceDispatcherHost::Get()->SetDelegate(
      resource_dispatcher_host_delegate_.get());
}

std::string ShellContentBrowserClient::GetDefaultDownloadName() {
  return "download";
}

MediaObserver* ShellContentBrowserClient::GetMediaObserver() {
  return MediaInternals::GetInstance();
}

void ShellContentBrowserClient::BrowserURLHandlerCreated(
    BrowserURLHandler* handler) {
}

ShellBrowserContext* ShellContentBrowserClient::browser_context() {
  return shell_browser_main_parts_->browser_context();
}

ShellBrowserContext*
    ShellContentBrowserClient::off_the_record_browser_context() {
  return shell_browser_main_parts_->off_the_record_browser_context();
}

AccessTokenStore* ShellContentBrowserClient::CreateAccessTokenStore() {
  return new ShellAccessTokenStore(browser_context()->GetRequestContext());
}

void ShellContentBrowserClient::OverrideWebkitPrefs(
      RenderViewHost* render_view_host,
      const GURL& url,
      webkit_glue::WebPreferences* prefs) {
  // Disable web security
  prefs->web_security_enabled = false;
  prefs->allow_file_access_from_file_urls = true;

  // Disable plugins to speed up (TODO provide flag to enable them)
  prefs->plugins_enabled = false;
  prefs->java_enabled = false;

  // No caches
  prefs->uses_page_cache = false;
}

}  // namespace content
