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
#include "base/string_util.h"
#include "base/threading/thread_restrictions.h"
#include "base/values.h"
#include "content/public/browser/browser_url_handler.h"
#include "content/public/browser/resource_dispatcher_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/renderer_preferences.h"
#include "content/nw/src/api/dispatcher_host.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/browser/shell_devtools_delegate.h"
#include "content/nw/src/browser/shell_resource_dispatcher_host_delegate.h"
#include "content/nw/src/media/media_internals.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_shell.h"
#include "content/nw/src/nw_version.h"
#include "content/nw/src/shell_browser_context.h"
#include "content/nw/src/shell_browser_main_parts.h"
#include "geolocation/shell_access_token_store.h"
#include "googleurl/src/gurl.h"
#include "webkit/glue/webpreferences.h"
#include "webkit/user_agent/user_agent_util.h"
#include "ui/base/l10n/l10n_util.h"
#include "webkit/plugins/npapi/plugin_list.h"

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

WebContentsView* ShellContentBrowserClient::OverrideCreateWebContentsView(
      WebContents* web_contents,
      RenderViewHostDelegateView** render_view_host_delegate_view) {
  std::string user_agent, rules;
  nw::Package* package = shell_browser_main_parts()->package();
  content::RendererPreferences* prefs = web_contents->GetMutableRendererPrefs();
  if (package->root()->GetString(switches::kmUserAgent, &user_agent)) {
    std::string name, version;
    package->root()->GetString(switches::kmName, &name);
    package->root()->GetString("version", &version);
    ReplaceSubstringsAfterOffset(&user_agent, 0, "%name", name);
    ReplaceSubstringsAfterOffset(&user_agent, 0, "%ver", version);
    ReplaceSubstringsAfterOffset(&user_agent, 0, "%nwver", NW_VERSION_STRING);
    ReplaceSubstringsAfterOffset(&user_agent, 0, "%webkit_ver", webkit_glue::GetWebKitVersion());
    ReplaceSubstringsAfterOffset(&user_agent, 0, "%osinfo", webkit_glue::BuildOSInfo());
    prefs->user_agent_override = user_agent;
  }
  if (package->root()->GetString(switches::kmRemotePages, &rules))
      prefs->nw_remote_page_rules = rules;
  return NULL;
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
  nw::Package* package = shell_browser_main_parts()->package();
  if (package && package->GetUseNode()) {
    // Allow node.js
    command_line->AppendSwitch(switches::kmNodejs);

    // Set cwd
    command_line->AppendSwitchPath(switches::kWorkingDirectory,
                                   package->path());

    // Check if we have 'node-main'.
    std::string node_main;
    if (package->root()->GetString(switches::kNodeMain, &node_main))
      command_line->AppendSwitchASCII(switches::kNodeMain, node_main);
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
  nw::Package* package = shell_browser_main_parts()->package();

  // Disable web security.
  prefs->dom_paste_enabled = true;
  prefs->javascript_can_access_clipboard = true;
  prefs->web_security_enabled = false;
  prefs->allow_file_access_from_file_urls = true;

  // Open experimental features.
  prefs->css_sticky_position_enabled = true;
  prefs->css_regions_enabled = true;
  prefs->css_shaders_enabled = true;
  prefs->css_variables_enabled = true;

  // Disable plugins and cache by default.
  prefs->plugins_enabled = false;
  prefs->java_enabled = false;
  prefs->uses_page_cache = false;
  
  // Force compositing mode to support transparency
  prefs->force_compositing_mode = true;
  prefs->accelerated_2d_canvas_enabled = true;
  prefs->accelerated_animation_enabled = true;
  prefs->accelerated_compositing_enabled = true;
  prefs->accelerated_filters_enabled = true;
  prefs->accelerated_layers_enabled = true;
  prefs->accelerated_painting_enabled = true;

  base::DictionaryValue* webkit;
  if (package->root()->GetDictionary(switches::kmWebkit, &webkit)) {
    webkit->GetBoolean(switches::kmJava, &prefs->java_enabled);
    webkit->GetBoolean(switches::kmPlugin, &prefs->plugins_enabled);
    webkit->GetBoolean(switches::kmPageCache, &prefs->uses_page_cache);
    FilePath plugins_dir = package->path();
    //PathService::Get(base::DIR_CURRENT, &plugins_dir);
    plugins_dir = plugins_dir.AppendASCII("plugins");

    webkit::npapi::PluginList::Singleton()->AddExtraPluginDir(plugins_dir);
  }
}

bool ShellContentBrowserClient::ShouldTryToUseExistingProcessHost(
      BrowserContext* browser_context, const GURL& url) {
  ShellBrowserContext* shell_browser_context =
    static_cast<ShellBrowserContext*>(browser_context);
  if (shell_browser_context->pinning_renderer())
    return true;
  else
    return false;
}

bool ShellContentBrowserClient::IsSuitableHost(RenderProcessHost* process_host,
                                          const GURL& site_url) {
  return true;
}

}  // namespace content
