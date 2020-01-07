#ifndef NW_CHROME_BROWSER_HOOKS_H_
#define NW_CHROME_BROWSER_HOOKS_H_

#include <string>
#include <vector>

#include "content/common/content_export.h"

#if defined(OS_WIN)
#include <windows.h>
#endif

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
const extensions::Extension* GetMainExtension();

#if defined(OS_WIN)
// ref in chrome/browser/app_icon_win.cc
HICON GetAppHIcon();
// ref in chrome/browser/app_icon_win.cc
HICON GetWindowHIcon();
#endif

}

#endif // NW_CHROME_BROWSER_HOOKS_H_
