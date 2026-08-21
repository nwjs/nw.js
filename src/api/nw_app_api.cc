#include "content/nw/src/api/nw_app_api.h"

#include "build/build_config.h"
#include "third_party/widevine/cdm/buildflags.h"

#include "chrome/browser/lifetime/browser_close_manager.h"
#include "chrome/browser/lifetime/application_lifetime_desktop.h"
#include "content/public/common/content_features.h"

#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/path_service.h"
#include "base/process/launch.h"
#include "base/strings/utf_string_conversions.h"
#include "base/task/current_thread.h"
#include "content/public/browser/browser_task_traits.h"
//#include "components/browsing_data/content/appcache_helper.h"
#include "components/browsing_data/content/browsing_data_helper.h"
#include "components/component_updater/component_updater_service.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/component_updater/widevine_cdm_component_installer.h"
#include "chrome/browser/devtools/devtools_window.h"
#include "chrome/browser/extensions/devtools_util.h"
#include "chrome/browser/extensions/extension_service.h"
#include "chrome/browser/net/profile_network_context_service.h"
#include "chrome/browser/net/profile_network_context_service_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "components/keep_alive_registry/keep_alive_registry.h"
#include "components/keep_alive_registry/keep_alive_types.h"
#include "content/nw/src/api/nw_app.h"
#include "content/nw/src/browser/nw_content_browser_hooks.h"
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

#if defined(OS_MAC)
#include "chrome/browser/mac/relauncher.h"
#endif

using namespace extensions::nwapi::nw__app;

namespace extensions {
namespace {

base::FilePath GetAbsolutePackagePath() {
  nw::Package* package = nw::package();
  if (!package)
    return base::FilePath();

  base::FilePath path = package->path().NormalizePathSeparators();
  if (path.empty() || path.IsAbsolute())
    return path;

  base::FilePath current_directory;
  if (!base::GetCurrentDirectory(&current_directory))
    return path;

  return current_directory.Append(path).NormalizePathSeparators();
}

bool ReplaceNwappSwitch(base::CommandLine::StringVector* argv,
                        const base::FilePath& package_path) {
#if defined(OS_WIN)
  const base::CommandLine::StringType nwapp_switch = L"--nwapp=";
  const base::CommandLine::StringType alt_nwapp_switch = L"-nwapp=";
  const base::CommandLine::StringType win_nwapp_switch = L"/nwapp=";
#else
  const base::CommandLine::StringType nwapp_switch = "--nwapp=";
  const base::CommandLine::StringType alt_nwapp_switch = "-nwapp=";
#endif

  for (auto& arg : *argv) {
    if (arg.compare(0, nwapp_switch.length(), nwapp_switch) == 0) {
      arg = nwapp_switch + package_path.value();
      return true;
    }
    if (arg.compare(0, alt_nwapp_switch.length(), alt_nwapp_switch) == 0) {
      arg = alt_nwapp_switch + package_path.value();
      return true;
    }
#if defined(OS_WIN)
    if (arg.compare(0, win_nwapp_switch.length(), win_nwapp_switch) == 0) {
      arg = win_nwapp_switch + package_path.value();
      return true;
    }
#endif
  }

  return false;
}

bool PathReferencesPackage(const base::CommandLine::StringType& arg,
                           const base::FilePath& package_path) {
  base::FilePath arg_path(arg);
  if (!arg_path.IsAbsolute()) {
    base::FilePath current_directory;
    if (!base::GetCurrentDirectory(&current_directory))
      return false;
    arg_path = current_directory.Append(arg_path);
  }

  return arg_path.NormalizePathSeparators() == package_path;
}

void ReplacePackagePathArg(base::CommandLine::StringVector* argv,
                           const base::FilePath& package_path) {
  for (auto it = argv->begin() + 1; it != argv->end(); ++it) {
    if (PathReferencesPackage(*it, package_path)) {
      *it = package_path.value();
      return;
    }
  }
}

base::CommandLine BuildRelaunchCommandLine() {
  const base::CommandLine& current_command_line =
      *base::CommandLine::ForCurrentProcess();
  base::CommandLine::StringVector argv = current_command_line.original_argv();

  base::FilePath exe;
  if (base::PathService::Get(base::FILE_EXE, &exe) && !argv.empty())
    argv[0] = exe.value();

  base::FilePath package_path = GetAbsolutePackagePath();
  if (!package_path.empty()) {
    bool has_nwapp_switch = ReplaceNwappSwitch(&argv, package_path);
    if (!has_nwapp_switch && argv.size() > 1)
      ReplacePackagePathArg(&argv, package_path);
  }

  return base::CommandLine(argv);
}

void RelaunchCurrentApp() {
  base::CommandLine command_line = BuildRelaunchCommandLine();
  base::FilePath exe;
  if (base::PathService::Get(base::FILE_EXE, &exe))
    command_line.SetProgram(exe);

#if defined(OS_MAC)
  if (!mac_relauncher::RelaunchApp(command_line.argv()))
    LOG(ERROR) << "Failed to relaunch NW app";
#else
  base::LaunchOptions options;
#if defined(OS_WIN)
  if (!exe.empty()) {
    options.current_directory = exe.DirName();
    options.grant_foreground_privilege = true;
  }
#endif

#if defined(OS_LINUX)
  options.allow_new_privs = true;
#endif

  if (!base::LaunchProcess(command_line, options).IsValid())
    LOG(ERROR) << "Failed to relaunch NW app";
#endif
}

void QuitApp(extensions::ExtensionRegistrar* registrar,
             const std::string& extension_id) {
  if (base::FeatureList::IsEnabled(::features::kNWNewWin)) {
    chrome::CloseAllBrowsersAndQuit(true);
    // trigger BrowserProcessImpl::Unpin()
    KeepAliveRegistry::GetInstance()->Register(KeepAliveOrigin::APP_CONTROLLER, KeepAliveRestartOption::ENABLED);
    KeepAliveRegistry::GetInstance()->Unregister(KeepAliveOrigin::APP_CONTROLLER, KeepAliveRestartOption::ENABLED);
    return;
  }

  base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(
      FROM_HERE,
      base::BindOnce(&ExtensionRegistrar::TerminateExtension,
                     registrar->GetWeakPtr(), extension_id));
}

}  // namespace

NwAppQuitFunction::NwAppQuitFunction() {

}

NwAppQuitFunction::~NwAppQuitFunction() {
}

void NwAppQuitFunction::DoJob(extensions::ExtensionRegistrar* registrar,
                              std::string extension_id) {
  QuitApp(registrar, extension_id);
}

ExtensionFunction::ResponseAction
NwAppQuitFunction::Run() {
  extensions::ExtensionRegistrar* registrar =
      extensions::ExtensionRegistrar::Get(browser_context());
  base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(
        FROM_HERE,
        base::BindOnce(&NwAppQuitFunction::DoJob,
                   registrar, extension_id()));
  return RespondNow(NoArguments());
}

void NwAppRestartFunction::DoJob(extensions::ExtensionRegistrar* registrar,
                                 std::string extension_id) {
  nw::ScheduleRelaunchOnShutdown(&RelaunchCurrentApp);
  QuitApp(registrar, extension_id);
}

ExtensionFunction::ResponseAction
NwAppRestartFunction::Run() {
  extensions::ExtensionRegistrar* registrar =
      extensions::ExtensionRegistrar::Get(browser_context());
  base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(
        FROM_HERE,
        base::BindOnce(&NwAppRestartFunction::DoJob,
                   registrar, extension_id()));

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
  base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(
        FROM_HERE,
        base::BindOnce(&NwAppCloseAllWindowsFunction::DoJob, registry, extension()->id()));

  return RespondNow(NoArguments());
}

ExtensionFunction::ResponseAction
NwAppEnableComponentFunction::Run() {
#if BUILDFLAG(ENABLE_WIDEVINE_CDM_COMPONENT)
  component_updater::RegisterWidevineCdmComponent(g_browser_process->component_updater(),
                                                  base::BindOnce(&NwAppEnableComponentFunction::OnRegistered,
                                                                 this));
  return RespondLater();
#else
  return RespondNow(NoArguments());
#endif
}

void NwAppEnableComponentFunction::OnRegistered() {
  std::string ret;
  const std::vector<component_updater::ComponentInfo> component_list =
    g_browser_process->component_updater()->GetComponents();

  for (const auto& component : component_list) {
    if (component.id == "oimompecagnajdejgnnjijobebaeigek") {
      ret = component.version.GetString();
    }
  }
  auto result_value = std::make_unique<base::Value>(ret);

  Respond(WithArguments(ret));
}

ExtensionFunction::ResponseAction
  NwAppUpdateComponentFunction::Run() {
  g_browser_process->component_updater()->GetOnDemandUpdater().OnDemandUpdate(
   "oimompecagnajdejgnnjijobebaeigek", component_updater::OnDemandUpdater::Priority::FOREGROUND,
   base::BindOnce(&NwAppUpdateComponentFunction::OnUpdated, this));

  return RespondLater();
}

void NwAppUpdateComponentFunction::OnUpdated(update_client::Error error) {
  bool ret = (error == update_client::Error::NONE);
  if (ret) {
    Respond(WithArguments(ret));
  } else {
    VLOG(1) << "update component error: " << (int)error;
    Respond(WithArguments(ret));
  }
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

  // Ignore the package path only when it was passed as a positional argument.
  bool ignore_arg = !package->self_extract() && !command_line->HasSwitch("nwapp");
  for (unsigned i = 1; i < argv.size(); ++i) {
    if (ignore_arg && args.size() && argv[i] == args[0]) {
      ignore_arg = false;
      continue;
    }

#if defined(OS_WIN)
    response->Append(base::WideToUTF16(argv[i]));
#else
    response->Append(argv[i]);
#endif
  }
  return true;
}

NwAppClearAppCacheFunction::NwAppClearAppCacheFunction() {
}

NwAppClearAppCacheFunction::~NwAppClearAppCacheFunction() {
}

bool NwAppClearAppCacheFunction::RunNWSync(base::ListValue* response, std::string* error) {
  // removed in upstream: https://chromium-review.googlesource.com/c/chromium/src/+/3214503
  return true;
}

NwAppClearCacheFunction::NwAppClearCacheFunction() : run_loop_(base::RunLoop::Type::kNestableTasksAllowed) {
}

NwAppClearCacheFunction::~NwAppClearCacheFunction() {
}

bool NwAppClearCacheFunction::RunNWSync(base::ListValue* response, std::string* error) {
  content::BrowsingDataRemover* remover =
    Profile::FromBrowserContext(browser_context())->GetBrowsingDataRemover();

  remover->AddObserver(this);
  remover->RemoveAndReply(base::Time(), base::Time::Max(),
                          content::BrowsingDataRemover::DATA_TYPE_CACHE,
                          content::BrowsingDataRemover::ORIGIN_TYPE_UNPROTECTED_WEB,
                          this);
  // BrowsingDataRemover deletes itself.
  run_loop_.Run();
  remover->RemoveObserver(this);
  return true;
}

void NwAppClearCacheFunction::OnBrowsingDataRemoverDone(uint64_t failed_data_types) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  run_loop_.Quit();
}

NwAppSetProxyConfigFunction::NwAppSetProxyConfigFunction() {
}

NwAppSetProxyConfigFunction::~NwAppSetProxyConfigFunction() {
}

bool NwAppSetProxyConfigFunction::RunNWSync(base::ListValue* response, std::string* error) {
  net::ProxyConfigWithAnnotation config;
  std::optional<nwapi::nw__app::SetProxyConfig::Params> params(
          nwapi::nw__app::SetProxyConfig::Params::Create(args()));
  EXTENSION_FUNCTION_VALIDATE(params.has_value());

  std::string pac_url = params->pac_url ? *params->pac_url : "";
  if (!pac_url.empty()) {
    if (pac_url == "<direct>")
      config = net::ProxyConfigWithAnnotation::CreateDirect();
    else if (pac_url == "<auto>")
      config = net::ProxyConfigWithAnnotation(net::ProxyConfig::CreateAutoDetect(), TRAFFIC_ANNOTATION_FOR_TESTS);
    else
      config = net::ProxyConfigWithAnnotation(net::ProxyConfig::CreateFromCustomPacURL(GURL(pac_url)), TRAFFIC_ANNOTATION_FOR_TESTS);
  } else {
    net::ProxyConfig pc;
    EXTENSION_FUNCTION_VALIDATE(args().size() && args()[0].is_string());
    std::string proxy_config = args()[0].GetString();

    pc.proxy_rules().ParseFromString(proxy_config);
    config = net::ProxyConfigWithAnnotation(pc, TRAFFIC_ANNOTATION_FOR_TESTS);
  }

  Profile* profile = Profile::FromBrowserContext(browser_context());
  auto* profile_network_context =
    ProfileNetworkContextServiceFactory::GetForContext(profile);
  profile_network_context->UpdateProxyConfig(config);
  return true;
}

bool NwAppGetDataPathFunction::RunNWSync(base::ListValue* response, std::string* error) {
#if defined(OS_WIN)
  response->Append(base::WideToUTF16(browser_context()->GetPath().value()));
#else
  response->Append(browser_context()->GetPath().value());
#endif
  return true;
}

ExtensionFunction::ResponseAction
NwAppCrashBrowserFunction::Run() {
  int* ptr = nullptr;
  *ptr = 1;
  return RespondNow(NoArguments());
}


} // namespace extensions
