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

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/path_service.h"
#include "base/strings/string_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/threading/thread_restrictions.h"
#include "base/values.h"
//#include "chrome/common/child_process_logging.h"
#include "components/crash/app/crash_reporter_client.h"
#include "content/browser/child_process_security_policy_impl.h"
#include "content/browser/gpu/compositor_util.h"
#include "chrome/browser/printing/printing_message_filter.h"
#include "content/public/browser/browser_url_handler.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host_iterator.h"
#include "content/public/browser/resource_dispatcher_host.h"
#include "content/public/browser/site_instance.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/content_descriptors.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/renderer_preferences.h"
#include "content/public/common/url_constants.h"
#include "content/nw/src/api/dispatcher_host.h"
#if defined(OS_MACOSX)
#include "content/nw/src/breakpad_mac.h"
#endif
#include "content/nw/src/common/shell_switches.h"
#include "chrome/browser/printing/print_job_manager.h"
#include "content/nw/src/browser/shell_devtools_manager_delegate.h"
//#include "content/nw/src/browser/media_capture_devices_dispatcher.h"
#include "content/nw/src/shell_quota_permission_context.h"
#include "content/nw/src/browser/shell_resource_dispatcher_host_delegate.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_shell.h"
#include "content/nw/src/nw_notification_manager.h"
#include "content/nw/src/nw_version.h"
#include "content/nw/src/shell_browser_context.h"
#include "content/nw/src/shell_browser_main_parts.h"
#include "geolocation/shell_access_token_store.h"
#include "url/gurl.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/ui_base_switches.h"
#include "content/common/dom_storage/dom_storage_map.h"
#include "content/public/common/web_preferences.h"
#include "content/public/common/user_agent.h"
#include "content/common/plugin_list.h"
#include "content/public/browser/plugin_service.h"
#include "chrome/common/chrome_switches.h"
#if defined(OS_LINUX)
#include "base/linux_util.h"
#endif

#if defined(OS_POSIX) && !defined(OS_MACOSX)
#include "base/debug/leak_annotations.h"
#include "components/crash/app/breakpad_linux.h"
#include "components/crash/browser/crash_handler_host_linux.h"
#include "content/public/common/content_descriptors.h"
#endif

#include "extensions/common/constants.h"
#include "extensions/common/extension.h"
#include "extensions/common/switches.h"

#include "content/nw/src/browser/shell_extension_system.h"
#include "extensions/browser/extension_protocols.h"
#include "extensions/browser/extension_message_filter.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/browser/info_map.h"
#include "extensions/browser/process_map.h"

//using base::FileDescriptor;

using extensions::Extension;
using extensions::ProcessMap;
using extensions::ExtensionRegistry;
using extensions::InfoMap;

namespace {

#if defined(OS_POSIX) && !defined(OS_MACOSX)
breakpad::CrashHandlerHostLinux* CreateCrashHandlerHost(
    const std::string& process_type) {
  base::FilePath dumps_path =
      CommandLine::ForCurrentProcess()->GetSwitchValuePath(
          switches::kCrashDumpsDir);
  if (dumps_path.value().empty())
    PathService::Get(base::DIR_TEMP, &dumps_path);
  {
    ANNOTATE_SCOPED_MEMORY_LEAK;
    breakpad::CrashHandlerHostLinux* crash_handler =
        new breakpad::CrashHandlerHostLinux(
            process_type, dumps_path, false);
    crash_handler->StartUploaderThread();
    return crash_handler;
  }
}

int GetCrashSignalFD(const CommandLine& command_line) {
  if (!breakpad::IsCrashReporterEnabled())
    return -1;

  std::string process_type =
      command_line.GetSwitchValueASCII(switches::kProcessType);

  if (process_type == switches::kRendererProcess) {
    static breakpad::CrashHandlerHostLinux* crash_handler = NULL;
    if (!crash_handler)
      crash_handler = CreateCrashHandlerHost(process_type);
    return crash_handler->GetDeathSignalSocket();
  }

  if (process_type == switches::kPluginProcess) {
    static breakpad::CrashHandlerHostLinux* crash_handler = NULL;
    if (!crash_handler)
      crash_handler = CreateCrashHandlerHost(process_type);
    return crash_handler->GetDeathSignalSocket();
  }

  if (process_type == switches::kPpapiPluginProcess) {
    static breakpad::CrashHandlerHostLinux* crash_handler = NULL;
    if (!crash_handler)
      crash_handler = CreateCrashHandlerHost(process_type);
    return crash_handler->GetDeathSignalSocket();
  }

  if (process_type == switches::kGpuProcess) {
    static breakpad::CrashHandlerHostLinux* crash_handler = NULL;
    if (!crash_handler)
      crash_handler = CreateCrashHandlerHost(process_type);
    return crash_handler->GetDeathSignalSocket();
  }

  return -1;
}
#endif  // defined(OS_POSIX) && !defined(OS_MACOSX)

}

namespace content {

ShellContentBrowserClient::ShellContentBrowserClient()
  : shell_browser_main_parts_(NULL),
    master_rph_(NULL) {
}

ShellContentBrowserClient::~ShellContentBrowserClient() {
}

BrowserMainParts* ShellContentBrowserClient::CreateBrowserMainParts(
    const MainFunctionParams& parameters) {
  shell_browser_main_parts_ = new ShellBrowserMainParts(parameters);
  return shell_browser_main_parts_;
}

bool ShellContentBrowserClient::GetUserAgentManifest(std::string* agent) {
  std::string user_agent;
  nw::Package* package = shell_browser_main_parts()->package();
  if (package->root()->GetString(switches::kmUserAgent, &user_agent)) {
    std::string name, version;
    package->root()->GetString(switches::kmName, &name);
    package->root()->GetString("version", &version);
    ReplaceSubstringsAfterOffset(&user_agent, 0, "%name", name);
    ReplaceSubstringsAfterOffset(&user_agent, 0, "%ver", version);
    ReplaceSubstringsAfterOffset(&user_agent, 0, "%nwver", NW_VERSION_STRING);
    ReplaceSubstringsAfterOffset(&user_agent, 0, "%webkit_ver", content::GetWebKitVersion());
    ReplaceSubstringsAfterOffset(&user_agent, 0, "%osinfo", content::BuildOSInfo());
    *agent = user_agent;
    return true;
  }
  return false;
}

void ShellContentBrowserClient::OverrideCreateWebContentsView(
      WebContents* web_contents,
      RenderViewHostDelegateView** render_view_host_delegate_view,
      const WebContents::CreateParams& params) {
  std::string user_agent, rules;
  nw::Package* package = shell_browser_main_parts()->package();
  content::RendererPreferences* prefs = web_contents->GetMutableRendererPrefs();
  if (GetUserAgentManifest(&user_agent)) {
    prefs->user_agent_override = user_agent;
  }
  if (package->root()->GetString(switches::kmRemotePages, &rules))
      prefs->nw_remote_page_rules = rules;

  prefs->nw_app_root_path        = package->path();
  prefs->nw_inject_css_fn        = params.nw_inject_css_fn;
  prefs->nw_inject_js_doc_start  = params.nw_inject_js_doc_start;
  prefs->nw_inject_js_doc_end    = params.nw_inject_js_doc_end;
}

std::string ShellContentBrowserClient::GetApplicationLocale() {
  base::ThreadRestrictions::ScopedAllowIO allow_io;

  CommandLine* cmd_line = CommandLine::ForCurrentProcess();
  std::string pref_locale;
  if (cmd_line->HasSwitch(switches::kLang)) {
    pref_locale = cmd_line->GetSwitchValueASCII(switches::kLang);
  }
  return l10n_util::GetApplicationLocale(pref_locale);
}

void ShellContentBrowserClient::AppendExtraCommandLineSwitches(
    CommandLine* command_line,
    int child_process_id) {
  bool is_isolated_guest = false;

#if defined(OS_MACOSX)
  command_line->AppendSwitch(switches::kDisableRemoteCoreAnimation);
  if (breakpad::IsCrashReporterEnabled()) {
    command_line->AppendSwitch(switches::kEnableCrashReporter);
  }
#elif defined(OS_POSIX)
  if (breakpad::IsCrashReporterEnabled()) {
    command_line->AppendSwitch(switches::kEnableCrashReporter);
  }

#endif  // OS_MACOSX

  if (command_line->GetSwitchValueASCII("type") != "renderer")
    return;

#if 0
  if (content::IsThreadedCompositingEnabled())
    command_line->AppendSwitch(switches::kEnableThreadedCompositing);
#endif //FIXME

  nw::Package* package = shell_browser_main_parts()->package();
  if (package && package->GetUseExtension())
    command_line->AppendSwitch(extensions::switches::kExtensionProcess);

  std::string user_agent;
  if (!command_line->HasSwitch(switches::kUserAgent) &&
      GetUserAgentManifest(&user_agent)) {
    command_line->AppendSwitchASCII(switches::kUserAgent, user_agent);
  }
  if (child_process_id > 0) {
    scoped_ptr<content::RenderWidgetHostIterator> widgets =
      content::RenderWidgetHost::GetRenderWidgetHosts();
    while (RenderWidgetHost* widget = widgets->GetNextHost()) {
      if (widget->GetProcess()->GetID() != child_process_id)
        continue;
      if (!widget->IsRenderView())
        continue;

      content::RenderViewHost* host = content::RenderViewHost::From(
        const_cast<content::RenderWidgetHost*>(widget));
      content::Shell* shell = content::Shell::FromRenderViewHost(host);
      if (shell) {
        if (!shell->nodejs())
          return;
        if (shell->is_devtools()) {
          // DevTools should have powerful permissions to load local
          // files by XHR (e.g. for source map)
          command_line->AppendSwitch(switches::kNodejs);
          return;
        }
      }
    }
    content::RenderProcessHost* process = content::RenderProcessHost::FromID(child_process_id);
    if (process && process->IsIsolatedGuest())
      is_isolated_guest = true;
  }

  if (package && package->GetUseNode() && !is_isolated_guest) {
    // Allow node.js
    command_line->AppendSwitch(switches::kNodejs);

    // Set cwd
    command_line->AppendSwitchPath(switches::kWorkingDirectory,
                                   package->path());

    // Check if we have 'node-main'.
    std::string node_main;
    if (package->root()->GetString(switches::kNodeMain, &node_main))
      command_line->AppendSwitchASCII(switches::kNodeMain, node_main);

    std::string snapshot_path;
    if (package->root()->GetString(switches::kSnapshot, &snapshot_path))
      command_line->AppendSwitchASCII(switches::kSnapshot, snapshot_path);

    int dom_storage_quota_mb;
    if (package->root()->GetInteger("dom_storage_quota", &dom_storage_quota_mb)) {
      content::DOMStorageMap::SetQuotaOverride(dom_storage_quota_mb * 1024 * 1024);
      command_line->AppendSwitchASCII(switches::kDomStorageQuota, base::IntToString(dom_storage_quota_mb));
    }
  }

  // without the switch, the destructor of the shell object will
  // shutdown renderwidgethost (RenderWidgetHostImpl::Shutdown) and
  // destory rph immediately. Then the channel error msg is caught by
  // SuicideOnChannelErrorFilter and the renderer is killed
  // immediately
#if 0 //FIXME
#if defined(OS_POSIX)
  command_line->AppendSwitch(switches::kChildCleanExit);
#endif
#endif

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

#if 0
MediaObserver* ShellContentBrowserClient::GetMediaObserver() {
  return MediaCaptureDevicesDispatcher::GetInstance();
}
#endif

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
  return new ShellAccessTokenStore(browser_context());
}

void ShellContentBrowserClient::OverrideWebkitPrefs(
      RenderViewHost* render_view_host,
      const GURL& url,
      content::WebPreferences* prefs) {
  nw::Package* package = shell_browser_main_parts()->package();

  // Disable web security.
  prefs->dom_paste_enabled = true;
  prefs->javascript_can_access_clipboard = true;
  prefs->web_security_enabled = true;
  prefs->allow_file_access_from_file_urls = true;

  // Open experimental features.
  prefs->css_variables_enabled = true;

  // Disable plugins and cache by default.
  prefs->plugins_enabled = true;
  prefs->java_enabled = false;

  base::DictionaryValue* webkit;
  if (package->root()->GetDictionary(switches::kmWebkit, &webkit)) {
    webkit->GetBoolean(switches::kmJava, &prefs->java_enabled);
    webkit->GetBoolean(switches::kmPlugin, &prefs->plugins_enabled);
    FilePath plugins_dir = package->path();
    //PathService::Get(base::DIR_CURRENT, &plugins_dir);
    plugins_dir = plugins_dir.AppendASCII("plugins");

    PluginService* plugin_service = PluginService::GetInstance();
    plugin_service->AddExtraPluginDir(plugins_dir);
  }
}

bool ShellContentBrowserClient::ShouldTryToUseExistingProcessHost(
      BrowserContext* browser_context, const GURL& url) {
  ShellBrowserContext* shell_browser_context =
    static_cast<ShellBrowserContext*>(browser_context);
  if (url.SchemeIs(content::kGuestScheme))
    return false;

  if (shell_browser_context->pinning_renderer())
    return true;
  else
    return false;
}

bool ShellContentBrowserClient::IsSuitableHost(RenderProcessHost* process_host,
                                          const GURL& site_url) {
  return process_host == master_rph_;
}

net::URLRequestContextGetter* ShellContentBrowserClient::CreateRequestContext(
    BrowserContext* content_browser_context,
    ProtocolHandlerMap* protocol_handlers,
    URLRequestInterceptorScopedVector request_interceptors) {
  ShellBrowserContext* shell_browser_context =
      ShellBrowserContextForBrowserContext(content_browser_context);
  extensions::InfoMap* extension_info_map =
      shell_browser_main_parts_->extension_system()->info_map();
  (*protocol_handlers)[extensions::kExtensionScheme] =
      linked_ptr<net::URLRequestJobFactory::ProtocolHandler>(
            extensions::CreateExtensionProtocolHandler(false /* is_incognito */,
                                         extension_info_map));
  return shell_browser_context->CreateRequestContext(protocol_handlers, request_interceptors.Pass(), extension_info_map);
}

net::URLRequestContextGetter*
ShellContentBrowserClient::CreateRequestContextForStoragePartition(
    BrowserContext* content_browser_context,
    const base::FilePath& partition_path,
    bool in_memory,
    ProtocolHandlerMap* protocol_handlers,
    URLRequestInterceptorScopedVector request_interceptors) {
  ShellBrowserContext* shell_browser_context =
      ShellBrowserContextForBrowserContext(content_browser_context);
  return shell_browser_context->CreateRequestContextForStoragePartition(
                                                                        partition_path, in_memory, protocol_handlers, request_interceptors.Pass());
}


ShellBrowserContext*
ShellContentBrowserClient::ShellBrowserContextForBrowserContext(
    BrowserContext* content_browser_context) {
  if (content_browser_context == browser_context())
    return browser_context();
  DCHECK_EQ(content_browser_context, off_the_record_browser_context());
  return off_the_record_browser_context();
}

printing::PrintJobManager* ShellContentBrowserClient::print_job_manager() {
  return shell_browser_main_parts_->print_job_manager();
}

void ShellContentBrowserClient::RenderProcessWillLaunch(
    RenderProcessHost* host) {
  int id = host->GetID();
  if (!master_rph_)
    master_rph_ = host;
  // Grant file: scheme to the whole process, since we impose
  // per-view access checks.
  content::ChildProcessSecurityPolicy::GetInstance()->GrantScheme(
      host->GetID(), "file");
  content::ChildProcessSecurityPolicy::GetInstance()->GrantScheme(
      host->GetID(), "app");

#if defined(ENABLE_PRINTING)
  host->AddFilter(new printing::PrintingMessageFilter(id));
#endif
  host->AddFilter(
                  new extensions::ExtensionMessageFilter(id, browser_context()));
}

bool ShellContentBrowserClient::IsHandledURL(const GURL& url) {
  if (!url.is_valid())
    return false;
  DCHECK_EQ(url.scheme(), base::StringToLowerASCII(url.scheme()));
  // Keep in sync with ProtocolHandlers added by
  // ShellURLRequestContextGetter::GetURLRequestContext().
  static const char* const kProtocolList[] = {
    url::kFileSystemScheme,
    url::kFileScheme,
    "app",
    extensions::kExtensionScheme,
    extensions::kExtensionResourceScheme,
  };
  for (size_t i = 0; i < arraysize(kProtocolList); ++i) {
    if (url.scheme() == kProtocolList[i])
      return true;
  }
  return false;
}

void ShellContentBrowserClient::AllowCertificateError(int render_process_id,
                                     int render_frame_id,
                                     int cert_error,
                                     const net::SSLInfo& ssl_info,
                                     const GURL& request_url,
                                     ResourceType resource_type,
                                     bool overridable,
                                     bool strict_enforcement,
                                     bool expired_previous_decision,
                                     const base::Callback<void(bool)>& callback,
                                     CertificateRequestResultType* result) {
  VLOG(1) << "AllowCertificateError: " << request_url;
  CommandLine* cmd_line = CommandLine::ForCurrentProcess();
  if (cmd_line->HasSwitch(switches::kIgnoreCertificateErrors)) {
    *result = content::CERTIFICATE_REQUEST_RESULT_TYPE_CONTINUE;
  }
  else
    *result = content::CERTIFICATE_REQUEST_RESULT_TYPE_DENY;
  return;
}

void ShellContentBrowserClient::GetAdditionalAllowedSchemesForFileSystem(
    std::vector<std::string>* additional_allowed_schemes) {
  ContentBrowserClient::GetAdditionalAllowedSchemesForFileSystem(
      additional_allowed_schemes);
  additional_allowed_schemes->push_back("app");
}

#if defined(OS_POSIX) && !defined(OS_MACOSX)
void ShellContentBrowserClient::GetAdditionalMappedFilesForChildProcess(
    const CommandLine& command_line,
    int child_process_id,
    FileDescriptorInfo* mappings) {
  int crash_signal_fd = GetCrashSignalFD(command_line);
  if (crash_signal_fd >= 0) {
    mappings->Share(kCrashDumpSignal, crash_signal_fd);
  }
}
#endif  // defined(OS_POSIX) && !defined(OS_MACOSX)

QuotaPermissionContext*
ShellContentBrowserClient::CreateQuotaPermissionContext() {
  return new ShellQuotaPermissionContext();
}

void CancelDesktopNotification(int render_process_id, int notification_id) {
  nw::NotificationManager *notificationManager = nw::NotificationManager::getSingleton();
  if (notificationManager == NULL) {
    NOTIMPLEMENTED();
    return;
  }
  notificationManager->CancelDesktopNotification(render_process_id, notification_id);
}

blink::WebNotificationPermission
ShellContentBrowserClient::CheckDesktopNotificationPermission(
    const GURL& source_origin,
    content::ResourceContext* context,
    int render_process_id) {
  return blink::WebNotificationPermissionAllowed;
}

void ShellContentBrowserClient::ShowDesktopNotification(
      const ShowDesktopNotificationHostMsgParams& params,
      BrowserContext* browser_context,
      int render_process_id,
      scoped_ptr<DesktopNotificationDelegate> delegate,
      base::Closure* cancel_callback) {
#if ENABLE_NOTIFICATIONS
  nw::NotificationManager *notificationManager = nw::NotificationManager::getSingleton();
  if (notificationManager == NULL) {
    NOTIMPLEMENTED();
    return;
  }
  content::RenderProcessHost* process = content::RenderProcessHost::FromID(render_process_id);
  notificationManager->AddDesktopNotification(params, process->GetID(),
                                              delegate->notification_id(),
                                              false);
  *cancel_callback = base::Bind(&CancelDesktopNotification, process->GetID(), delegate->notification_id());
#else
  NOTIMPLEMENTED();
#endif

}

void ShellContentBrowserClient::RequestPermission(
    content::PermissionType permission,
    content::WebContents* web_contents,
    int bridge_id,
    const GURL& requesting_frame,
    bool user_gesture,
    const base::Callback<void(bool)>& result_callback) {
  result_callback.Run(true);
}

DevToolsManagerDelegate*
ShellContentBrowserClient::GetDevToolsManagerDelegate() {
  return new ShellDevToolsManagerDelegate(browser_context());
}

bool ShellContentBrowserClient::ShouldUseProcessPerSite(
    content::BrowserContext* browser_context,
    const GURL& effective_url) {
  return effective_url.SchemeIs(content::kGuestScheme);
}

bool ShellContentBrowserClient::CheckMediaAccessPermission(BrowserContext* browser_context,
                                                           const GURL& security_origin,
                                                           MediaStreamType type) {
  return true;
}

void ShellContentBrowserClient::SiteInstanceGotProcess(
    content::SiteInstance* site_instance) {
  // If this isn't an extension renderer there's nothing to do.
  const Extension* extension = GetExtension(site_instance);
  if (!extension)
    return;

  ProcessMap::Get(shell_browser_main_parts_->browser_context())
      ->Insert(extension->id(),
               site_instance->GetProcess()->GetID(),
               site_instance->GetId());

  BrowserThread::PostTask(
      BrowserThread::IO,
      FROM_HERE,
      base::Bind(&InfoMap::RegisterExtensionProcess,
                 shell_browser_main_parts_->extension_system()->info_map(),
                 extension->id(),
                 site_instance->GetProcess()->GetID(),
                 site_instance->GetId()));
}

void ShellContentBrowserClient::SiteInstanceDeleting(
    content::SiteInstance* site_instance) {
  // If this isn't an extension renderer there's nothing to do.
  const Extension* extension = GetExtension(site_instance);
  if (!extension)
    return;

  ProcessMap::Get(shell_browser_main_parts_->browser_context())
      ->Remove(extension->id(),
               site_instance->GetProcess()->GetID(),
               site_instance->GetId());

  BrowserThread::PostTask(
      BrowserThread::IO,
      FROM_HERE,
      base::Bind(&InfoMap::UnregisterExtensionProcess,
                 shell_browser_main_parts_->extension_system()->info_map(),
                 extension->id(),
                 site_instance->GetProcess()->GetID(),
                 site_instance->GetId()));
}

const Extension* ShellContentBrowserClient::GetExtension(
    content::SiteInstance* site_instance) {
  ExtensionRegistry* registry =
      ExtensionRegistry::Get(site_instance->GetBrowserContext());
  return registry->enabled_extensions().GetExtensionOrAppByURL(
      site_instance->GetSiteURL());
}

}  // namespace content
