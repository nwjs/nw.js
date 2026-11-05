#ifndef NW_CONTENT_BROWSER_HOOKS_H_
#define NW_CONTENT_BROWSER_HOOKS_H_

#include <vector>

#include "content/common/content_export.h"
#include "content/public/browser/child_process_termination_info.h"
#include "third_party/blink/public/common/web_preferences/web_preferences.h"
#include "url/gurl.h"

class GURL;

namespace base {
class FilePath;
class CommandLine;
}  // namespace base

namespace gfx {
class Image;
}  // namespace gfx

namespace content {
class RenderProcessHost;
class NotificationDetails;
class WebContents;
struct WebPreferences;
class RenderViewHost;
}  // namespace content

namespace nw {

class Package;

// ---------------------------------------------------------------------------
// NW.js content/browser integration hooks
//
// Browser-process hooks used by NW.js to integrate with Chromium/Content.
// Implementations live primarily in:
//   - nw_content_browser_hooks.cc
//   - nw_extensions_browser_hooks.cc
//   - nw_chrome_browser_hooks.cc
//
// Android support (nwjs/nw.js#94): declarations below define the browser-hook
// contract for Activity/Intent lifecycle integration. Full Android enablement
// also requires build targets, Java/JNI bridge, activity/content-browser-client
// wiring, process/window model, and packaging/runtime work outside this header.
// ---------------------------------------------------------------------------

// ref in chrome/browser/chrome_browser_main.cc
int MainPartsPreCreateThreadsHook();
// ref in chrome/browser/chrome_browser_main.cc
void MainPartsPreMainMessageLoopRunHook();
// ref in chrome/browser/chrome_browser_main.cc
CONTENT_EXPORT void MainPartsPostDestroyThreadsHook();
// ref in chrome/browser/extensions/extension_service.cc
CONTENT_EXPORT void RendererProcessTerminatedHook(
    content::RenderProcessHost* process,
    const content::ChildProcessTerminationInfo& info);
// ref in chrome/browser/chrome_browser_main.cc
bool ProcessSingletonNotificationCallbackHook(
    const base::CommandLine& command_line,
    const base::FilePath& current_directory);
// ref in chrome/browser/chrome_content_browser_client.cc
CONTENT_EXPORT void OverrideWebkitPrefsHook(
    content::WebContents* web_contents,
    blink::web_pref::WebPreferences* web_prefs);
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

#if defined(OS_ANDROID)
// Android Activity/Intent lifecycle hooks (nwjs/nw.js#94).
// Intended call sites: NW/Chrome Android activity/browser-client glue on
// cold start, onNewIntent, and activity resume/pause. Call on UI thread.

// Delivers validated navigable URLs from Android intents (cold start /
// ACTION_VIEW / deep links). Callers must filter non-navigable/non-NW intents.
CONTENT_EXPORT void AndroidOpenURLsHook(const std::vector<GURL>& startup_urls);

// Routes a URL received while the host is already running (onNewIntent).
// Returns true only when NW consumed the URL; returns false for invalid,
// non-navigable, or unhandled URLs.
CONTENT_EXPORT bool AndroidHandleNewIntentHook(const GURL& url);

// Activity resumed: reconcile NW visibility/lifecycle state. Must be cheap and
// safe on the UI thread; implementations should tolerate repeated calls.
CONTENT_EXPORT void AndroidOnActivityResumeHook();

// Activity paused: track lifecycle state only; no blocking/heavy work.
CONTENT_EXPORT void AndroidOnActivityPauseHook();
#endif  // defined(OS_ANDROID)

// ref in extensions/browser/api/app_window/app_window_api.cc
CONTENT_EXPORT bool GetPackageImage(nw::Package* package,
                                    const base::FilePath& icon_path,
                                    gfx::Image* image);
// ref in extensions/browser/api/app_window/app_window_api.cc
CONTENT_EXPORT void SetPinningRenderer(bool pin);
CONTENT_EXPORT void SetMixedContext(bool mixed_context);

// browser
// ref in content/nw/src/api/tray/tray_aura.cc
// content/nw/src/api/menuitem/menuitem_views.cc
CONTENT_EXPORT bool GetImage(Package* package,
                             const base::FilePath& icon_path,
                             gfx::Image* image);

CONTENT_EXPORT void SetInWebViewApplyAttr(bool in_webview, bool allow_nw);
CONTENT_EXPORT bool GetInWebViewApplyAttr(bool* allow_nw);

// implemented in nw_extensions_browser_hooks.cc
// ref in content/browser/renderer_host/render_process_host_impl.cc
bool RphGuestFilterURLHook(content::RenderProcessHost* rph, const GURL* url);

// implemented in nw_chrome_browser_hooks.cc
// ref in content/browser/webui/url_data_manager_backend.cc
bool CheckStoragePartitionMatches(int render_process_id, const GURL& url);

}  // namespace nw

#endif  // NW_CONTENT_BROWSER_HOOKS_H_