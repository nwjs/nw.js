#ifndef NW_CHROME_BROWSER_HOOKS_H_
#define NW_CHROME_BROWSER_HOOKS_H_

#include <string>
#include <vector>

class GURL;

namespace base {
  class CommandLine;
  class FilePath;
}

namespace gfx {
  class ImageSkia;
  class Image;
}

namespace content {
  class NotificationDetails;
  class WebContents;
  struct WebPreferences;
  class RenderProcessHost;
  class RenderViewHost;
}

namespace extensions {
  class AppWindow;
  class Extension;
}

namespace nw {
// browser
// ref in chrome/browser/ui/apps/chrome_app_window_client.cc
void CreateAppWindowHook(extensions::AppWindow*);
// ref in chrome/browser/extensions/extension_service.cc
void ReloadExtensionHook(const extensions::Extension*);
// ref in chrome/browser/apps/app_shim/app_shim_handler_mac.cc
bool IsReloadingApp();
// ref in chrome/browser/ui/views/chrome_views_delegate.cc
gfx::ImageSkia* GetAppIcon();
// ref in apps/app_load_service.cc
void SetMainExtensionId(const std::string& id);
// ref in chrome/browser/extensions/extension_service.cc
const std::string& GetMainExtensionId();

#if defined(OS_WIN)
// ref in chrome/browser/app_icon_win.cc
HICON GetAppHIcon();
// ref in chrome/browser/app_icon_win.cc
HICON GetWindowHIcon();
#endif

//
// implemented in nw_content_browser_hooks.cc
//
// ref in chrome/browser/chrome_browser_main.cc
int MainPartsPreCreateThreadsHook();
// ref in chrome/browser/chrome_browser_main.cc
void MainPartsPreMainMessageLoopRunHook();
// ref in chrome/browser/chrome_browser_main.cc
void MainPartsPostDestroyThreadsHook();
// ref in chrome/browser/extensions/extension_service.cc
void RendererProcessTerminatedHook(content::RenderProcessHost* process,
                                  const content::NotificationDetails& details);
// ref in chrome/browser/chrome_browser_main.cc
bool ProcessSingletonNotificationCallbackHook(const base::CommandLine& command_line,
                                             const base::FilePath& current_directory);
// ref in chrome/browser/chrome_content_browser_client.cc
void OverrideWebkitPrefsHook(content::RenderViewHost* rvh, content::WebPreferences* web_prefs);
// ref in chrome/browser/chrome_content_browser_client.cc
// content/browser/renderer_host/render_process_host_impl.cc
// content/browser/site_instance_impl.cc
bool PinningRenderer();

#if defined(OS_MACOSX)
// ref in chrome/browser/app_controller_mac.mm
bool ApplicationShouldHandleReopenHook(bool hasVisibleWindows);
// ref in chrome/browser/app_controller_mac.mm
void OSXOpenURLsHook(const std::vector<GURL>& startup_urls);
#endif

}

#endif // NW_CHROME_BROWSER_HOOKS_H_