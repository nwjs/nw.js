#ifndef NW_CONTENT_BROWSER_HOOKS_H_
#define NW_CONTENT_BROWSER_HOOKS_H_

#include "content/common/content_export.h"
#include "content/public/browser/child_process_termination_info.h"
#include "third_party/blink/public/common/web_preferences/web_preferences.h"

class GURL;

namespace base {
  class FilePath;
  class CommandLine;
}

namespace gfx {
  class Image;
}

namespace content {
  class RenderProcessHost;
  class NotificationDetails;
  class WebContents;
  struct WebPreferences;
  class RenderProcessHost;
  class RenderViewHost;
}

namespace nw {

class Package;

//
// implemented in nw_content_browser_hooks.cc
//
// ref in chrome/browser/chrome_browser_main.cc
int MainPartsPreCreateThreadsHook();
// ref in chrome/browser/chrome_browser_main.cc
void MainPartsPreMainMessageLoopRunHook();
// ref in chrome/browser/chrome_browser_main.cc
CONTENT_EXPORT void MainPartsPostDestroyThreadsHook();
// ref in chrome/browser/extensions/extension_service.cc
CONTENT_EXPORT void RendererProcessTerminatedHook(content::RenderProcessHost* process,
						  const content::ChildProcessTerminationInfo& info);
// ref in chrome/browser/chrome_browser_main.cc
bool ProcessSingletonNotificationCallbackHook(const base::CommandLine& command_line,
                                             const base::FilePath& current_directory);
// ref in chrome/browser/chrome_content_browser_client.cc
CONTENT_EXPORT void OverrideWebkitPrefsHook(content::WebContents* web_conents, blink::web_pref::WebPreferences* web_prefs);
// ref in chrome/browser/chrome_content_browser_client.cc
// content/browser/renderer_host/render_process_host_impl.cc
// content/browser/site_instance_impl.cc
CONTENT_EXPORT bool PinningRenderer();
CONTENT_EXPORT bool MixedContext();

#if defined(OS_MAC)
// ref in chrome/browser/app_controller_mac.mm
CONTENT_EXPORT bool ApplicationShouldHandleReopenHook(bool hasVisibleWindows);
// ref in chrome/browser/app_controller_mac.mm
CONTENT_EXPORT void OSXOpenURLsHook(const std::vector<GURL>& startup_urls);
#endif

// ref in extensions/browser/api/app_window/app_window_api.cc
CONTENT_EXPORT bool GetPackageImage(nw::Package* package,
                    const base::FilePath& icon_path,
                    gfx::Image* image);
// ref in extensions/browser/api/app_window/app_window_api.cc
CONTENT_EXPORT void SetPinningRenderer(bool pin);
CONTENT_EXPORT void SetMixedContext(bool);

// browser
// ref in content/nw/src/api/tray/tray_aura.cc
// content/nw/src/api/menuitem/menuitem_views.cc
CONTENT_EXPORT bool GetImage(Package* package, const base::FilePath& icon_path, gfx::Image* image);

CONTENT_EXPORT void SetInWebViewApplyAttr(bool, bool);
CONTENT_EXPORT bool GetInWebViewApplyAttr(bool* allow_nw);

//
// implemented in nw_extensions_browser_hooks.cc
//
// ref in content/browser/renderer_host/render_process_host_impl.cc 
bool RphGuestFilterURLHook(content::RenderProcessHost* rph, const GURL* url);

//
// implemented in nw_chrome_browser_hooks.cc
//
// ref in content/browser/webui/url_data_manager_backend.cc
bool CheckStoragePartitionMatches(int render_process_id, const GURL& url);

// unknown ref
// bool ShouldServiceRequestHook(int child_id, const GURL& url);
}

#endif // NW_CONTENT_BROWSER_HOOKS_H_
