#include "nw_chrome_browser_hooks.h"

// base
#include "base/command_line.h"
#include "base/files/file_util.h"
#include "base/path_service.h"
#include "base/threading/thread_restrictions.h"

#if defined(OS_WIN)
#include "base/win/scoped_gdi_object.h"
#endif // OS_WIN

// chrome
#include "chrome/browser/browser_process.h"
#include "chrome/browser/devtools/devtools_window.h"
#include "chrome/browser/io_thread.h"
#include "chrome/browser/lifetime/application_lifetime.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "chrome/browser/lifetime/keep_alive_registry.h"
#include "chrome/browser/lifetime/keep_alive_types.h"
#include "chrome/common/chrome_constants.h"
#include "chrome/common/chrome_paths.h"

#include "components/content_settings/core/browser/content_settings_utils.h"

// content
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/child_process_security_policy.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/web_contents.h"

// content/nw
#include "content/nw/src/nw_base.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/policy_cert_verifier.h"

// extensions
#include "extensions/browser/event_router.h"
#include "extensions/browser/extension_prefs.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/common/extension.h"

#include "net/cert/x509_certificate.h"

#if defined(OS_WIN)
#define _USE_MATH_DEFINES
#include <math.h>
#endif

using content::DevToolsAgentHost;
using content::RenderFrameHost;
using extensions::EventRouter;
using extensions::Extension;
using extensions::ExtensionPrefs;
using extensions::ExtensionRegistry;

namespace nw
{

namespace {

gfx::Image g_app_icon;
int g_cdt_process_id = -1;
std::string g_extension_id;
bool g_reloading_app = false;

#if defined(OS_WIN)
base::win::ScopedHICON g_window_hicon;
base::win::ScopedHICON g_app_hicon;
#endif

}

void ReloadExtensionHook(const extensions::Extension* extension) {
  g_reloading_app = true;
  KeepAliveRegistry::GetInstance()->Register(KeepAliveOrigin::APP_CONTROLLER, KeepAliveRestartOption::ENABLED);
  //keep alive count is paired with
  //appwindow's lifetime, so it's
  //needed to keep alive during
  //reload
}

void CreateAppWindowHook(extensions::AppWindow* app_window) {
  if (g_reloading_app) {
    g_reloading_app = false;
    KeepAliveRegistry::GetInstance()->Unregister(KeepAliveOrigin::APP_CONTROLLER, KeepAliveRestartOption::ENABLED);
  }
}

bool IsReloadingApp() {
  return g_reloading_app;
}

void SendEventToApp(const std::string& event_name, scoped_ptr<base::ListValue> event_args) {
  Profile* profile = ProfileManager::GetActiveUserProfile();
  const extensions::ExtensionSet& extensions =
    ExtensionRegistry::Get(profile)->enabled_extensions();
  ExtensionPrefs* extension_prefs = ExtensionPrefs::Get(profile);

  for (extensions::ExtensionSet::const_iterator it = extensions.begin();
       it != extensions.end(); ++it) {
    const Extension* extension = it->get();
    if (extension_prefs->IsExtensionRunning(extension->id()) &&
        extension->location() == extensions::Manifest::COMMAND_LINE) {
      scoped_ptr<extensions::Event> event(new extensions::Event(extensions::events::UNKNOWN,
                                                                event_name,
                                                                std::move(event_args)));
      event->restrict_to_browser_context = profile;
      EventRouter::Get(profile)
        ->DispatchEventToExtension(extension->id(), std::move(event));
    }
  }
}

bool CheckStoragePartitionMatches(int render_process_id, const GURL& url) {
  return render_process_id == g_cdt_process_id && url.SchemeIs(content_settings::kChromeDevToolsScheme);
}

void ShowDevtools(bool show, content::WebContents* web_contents, content::WebContents* container) {
  content::RenderFrameHost* rfh = web_contents->GetMainFrame();
  if (container) {
    scoped_refptr<DevToolsAgentHost> agent_host(DevToolsAgentHost::GetOrCreateFor(rfh));
    g_cdt_process_id = container->GetRenderProcessHost()->GetID();
    content::ChildProcessSecurityPolicy::GetInstance()->GrantAll(g_cdt_process_id);
    
    DevToolsWindow* window = DevToolsWindow::FindDevToolsWindow(agent_host.get());
    if (!window) {
      Profile* profile = Profile::FromBrowserContext(
             web_contents->GetBrowserContext());
      window = DevToolsWindow::Create(profile, GURL(), nullptr, false, std::string(), false, std::string(), container);
      if (!window)
        return;
      window->bindings_->AttachTo(agent_host);
    }
    return;
  }

  if (show) {
    DevToolsWindow::InspectElement(rfh, 0, 0);
    return;
  }
  scoped_refptr<DevToolsAgentHost> agent(
      DevToolsAgentHost::GetOrCreateFor(rfh));
  DevToolsWindow* window = DevToolsWindow::FindDevToolsWindow(agent.get());
  if (window)
    window->InspectedContentsClosing();
}

const char* GetChromiumVersion() {
  return chrome::kChromeVersion;
}

bool GetDirUserData(base::FilePath *user_data_dir) {
  return PathService::Get(chrome::DIR_USER_DATA, user_data_dir);
}

void SetTrustAnchorsOnIOThread(IOThread* io_thread, const net::CertificateList& trust_anchors) {
  PolicyCertVerifier* verifier =
    (PolicyCertVerifier*)io_thread->globals()->cert_verifier.get();
  verifier->SetTrustAnchors(trust_anchors);
}

void SetTrustAnchors(net::CertificateList& trust_anchors) {
  // LOG(INFO)
  //   << "Added " << trust_anchors.size() << " certificates to trust anchors.";
  content::BrowserThread::PostTask(
    content::BrowserThread::IO,
    FROM_HERE,
    base::Bind(SetTrustAnchorsOnIOThread, g_browser_process->io_thread(), trust_anchors));
}

void SetAppIcon(gfx::Image &icon) {
  g_app_icon = icon;
}

gfx::ImageSkia* GetAppIcon() {
  if (g_app_icon.IsEmpty())
    return nullptr;
  return const_cast<gfx::ImageSkia*>(g_app_icon.ToImageSkia());
}

void SetMainExtensionId(const std::string& id) {
  g_extension_id = id;
}

const std::string& GetMainExtensionId() {
  return g_extension_id;
}

#if defined(OS_WIN)
HICON GetAppHIcon() {
  return g_app_hicon.get();
}

HICON GetWindowHIcon() {
  return g_window_hicon.get();
}

void SetAppHIcon(base::win::ScopedHICON icon) {
  g_app_hicon.reset(icon);
}

void SetWindowHIcon(base::win::ScopedHICON icon) {
  g_window_hicon.reset(icon);
}

#endif

} // namespace nw