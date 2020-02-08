#include "content/nw/src/api/nw_app_api.h"

#include "chrome/browser/lifetime/browser_close_manager.h"
#include "chrome/browser/lifetime/application_lifetime.h"
#include "content/public/common/content_features.h"

#include "base/command_line.h"
#include "base/memory/ptr_util.h"
#include "base/message_loop/message_loop_current.h"
#include "base/task/post_task.h"
#include "content/public/browser/browser_task_traits.h"
#include "chrome/browser/browsing_data/browsing_data_appcache_helper.h"
#include "chrome/browser/browsing_data/browsing_data_helper.h"
#include "chrome/browser/devtools/devtools_window.h"
#include "chrome/browser/extensions/devtools_util.h"
#include "chrome/browser/extensions/extension_service.h"
#include "chrome/browser/net/profile_network_context_service.h"
#include "chrome/browser/net/profile_network_context_service_factory.h"
#include "components/keep_alive_registry/keep_alive_registry.h"
#include "components/keep_alive_registry/keep_alive_types.h"
#include "content/nw/src/api/nw_app.h"
#include "content/nw/src/nw_base.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/browser/web_contents.h"
#include "extensions/browser/app_window/app_window.h"
#include "extensions/browser/app_window/app_window_registry.h"
#include "extensions/browser/app_window/native_app_window.h"
#include "extensions/browser/extension_system.h"
#include "extensions/common/error_utils.h"
#include "net/proxy_resolution/proxy_config.h"
#include "net/proxy_resolution/proxy_config_service_fixed.h"
#include "net/proxy_resolution/configured_proxy_resolution_service.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_getter.h"
#include "net/traffic_annotation/network_traffic_annotation_test_helper.h"

using namespace extensions::nwapi::nw__app;

namespace extensions {
NwAppQuitFunction::NwAppQuitFunction() {

}

NwAppQuitFunction::~NwAppQuitFunction() {
}

void NwAppQuitFunction::DoJob(ExtensionService* service, std::string extension_id) {
  if (base::FeatureList::IsEnabled(::features::kNWNewWin)) {
    chrome::CloseAllBrowsersAndQuit(true);
    // trigger BrowserProcessImpl::Unpin()
    KeepAliveRegistry::GetInstance()->Register(KeepAliveOrigin::APP_CONTROLLER, KeepAliveRestartOption::ENABLED);
    KeepAliveRegistry::GetInstance()->Unregister(KeepAliveOrigin::APP_CONTROLLER, KeepAliveRestartOption::ENABLED);
    return;
  }
  base::ThreadTaskRunnerHandle::Get().get()->PostTask(
                                                      FROM_HERE,
                                                      base::Bind(&ExtensionService::TerminateExtension,
                                                                   service->AsWeakPtr(),
                                                                   extension_id));
}

ExtensionFunction::ResponseAction
NwAppQuitFunction::Run() {
  ExtensionService* service =
    ExtensionSystem::Get(browser_context())->extension_service();
  base::ThreadTaskRunnerHandle::Get().get()->PostTask(
        FROM_HERE,
        base::Bind(&NwAppQuitFunction::DoJob,
                   service,
                   extension_id()));
  return RespondNow(NoArguments());
}

void NwAppCloseAllWindowsFunction::DoJob(AppWindowRegistry* registry, std::string id) {
  if (base::FeatureList::IsEnabled(::features::kNWNewWin)) {
    chrome::CloseAllBrowsers();
  }
  AppWindowRegistry::AppWindowList windows =
    registry->GetAppWindowsForApp(id);

  for (AppWindow* window : windows) {
    if (window->NWCanClose())
      window->GetBaseWindow()->Close();
  }
}

ExtensionFunction::ResponseAction
NwAppCloseAllWindowsFunction::Run() {
  AppWindowRegistry* registry = AppWindowRegistry::Get(browser_context());
  if (!registry)
    return RespondNow(Error(""));
  base::ThreadTaskRunnerHandle::Get().get()->PostTask(
        FROM_HERE,
        base::Bind(&NwAppCloseAllWindowsFunction::DoJob, registry, extension()->id()));

  return RespondNow(NoArguments());
}

NwAppGetArgvSyncFunction::NwAppGetArgvSyncFunction() {
}

NwAppGetArgvSyncFunction::~NwAppGetArgvSyncFunction() {
}

bool NwAppGetArgvSyncFunction::RunNWSync(base::ListValue* response, std::string* error) {

  nw::Package* package = nw::package();
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  base::CommandLine::StringVector args = command_line->GetArgs();
  base::CommandLine::StringVector argv = command_line->original_argv();

  // Ignore first non-switch arg if it's not a standalone package.
  bool ignore_arg = !package->self_extract();
  for (unsigned i = 1; i < argv.size(); ++i) {
    if (ignore_arg && args.size() && argv[i] == args[0]) {
      ignore_arg = false;
      continue;
    }

    response->AppendString(argv[i]);
  }
  return true;
}

NwAppClearAppCacheFunction::NwAppClearAppCacheFunction() {
}

NwAppClearAppCacheFunction::~NwAppClearAppCacheFunction() {
}

bool NwAppClearAppCacheFunction::RunNWSync(base::ListValue* response, std::string* error) {
  std::string manifest;
  EXTENSION_FUNCTION_VALIDATE(args_->GetString(0, &manifest));

  GURL manifest_url(manifest);
  scoped_refptr<CannedBrowsingDataAppCacheHelper> helper(
                                                         new CannedBrowsingDataAppCacheHelper(content::BrowserContext::GetDefaultStoragePartition(context_)
                                                                                              ->GetAppCacheService()));

  helper->DeleteAppCaches(url::Origin::Create(manifest_url));
  return true;
}

NwAppClearCacheFunction::NwAppClearCacheFunction() {
}

NwAppClearCacheFunction::~NwAppClearCacheFunction() {
}

bool NwAppClearCacheFunction::RunNWSync(base::ListValue* response, std::string* error) {
  content::BrowsingDataRemover* remover = content::BrowserContext::GetBrowsingDataRemover(
      Profile::FromBrowserContext(context_));

  remover->AddObserver(this);
  remover->RemoveAndReply(base::Time(), base::Time::Max(),
                          content::BrowsingDataRemover::DATA_TYPE_CACHE,
                          content::BrowsingDataRemover::ORIGIN_TYPE_UNPROTECTED_WEB,
                          this);
  // BrowsingDataRemover deletes itself.
  base::MessageLoopCurrent::ScopedNestableTaskAllower allow;

  run_loop_.Run();
  remover->RemoveObserver(this);
  return true;
}

void NwAppClearCacheFunction::OnBrowsingDataRemoverDone() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  run_loop_.Quit();
}

NwAppSetProxyConfigFunction::NwAppSetProxyConfigFunction() {
}

NwAppSetProxyConfigFunction::~NwAppSetProxyConfigFunction() {
}

bool NwAppSetProxyConfigFunction::RunNWSync(base::ListValue* response, std::string* error) {
  net::ProxyConfigWithAnnotation config;
  std::unique_ptr<nwapi::nw__app::SetProxyConfig::Params> params(
      nwapi::nw__app::SetProxyConfig::Params::Create(*args_));
  EXTENSION_FUNCTION_VALIDATE(params.get());

  std::string pac_url = params->pac_url.get() ? *params->pac_url : "";
  if (!pac_url.empty()) {
    if (pac_url == "<direct>")
      config = net::ProxyConfigWithAnnotation::CreateDirect();
    else if (pac_url == "<auto>")
      config = net::ProxyConfigWithAnnotation(net::ProxyConfig::CreateAutoDetect(), TRAFFIC_ANNOTATION_FOR_TESTS);
    else
      config = net::ProxyConfigWithAnnotation(net::ProxyConfig::CreateFromCustomPacURL(GURL(pac_url)), TRAFFIC_ANNOTATION_FOR_TESTS);
  } else {
    std::string proxy_config;
    net::ProxyConfig pc;
    EXTENSION_FUNCTION_VALIDATE(args_->GetString(0, &proxy_config));
    pc.proxy_rules().ParseFromString(proxy_config);
    config = net::ProxyConfigWithAnnotation(pc, TRAFFIC_ANNOTATION_FOR_TESTS);
  }

  Profile* profile = Profile::FromBrowserContext(context_);
  auto* profile_network_context =
    ProfileNetworkContextServiceFactory::GetForContext(profile);
  profile_network_context->UpdateProxyConfig(config);
  return true;
}

bool NwAppGetDataPathFunction::RunNWSync(base::ListValue* response, std::string* error) {
  response->AppendString(browser_context()->GetPath().value());
  return true;
}

ExtensionFunction::ResponseAction
NwAppCrashBrowserFunction::Run() {
  int* ptr = nullptr;
  *ptr = 1;
  return RespondNow(NoArguments());
}


} // namespace extensions
