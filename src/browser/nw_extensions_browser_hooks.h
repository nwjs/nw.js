#ifndef NW_EXTENSIONS_BROWSER_HOOKS_H_
#define NW_EXTENSIONS_BROWSER_HOOKS_H_

#if defined(WIN32)
#include <windows.h>
#include "base/win/scoped_gdi_object.h"
#endif

#include <string>
#include "third_party/blink/public/web/web_navigation_policy.h"

class GURL;

namespace base {
  class DictionaryValue;
  class FilePath;
}

namespace content {
  class RenderProcessHost;
  class WebContents;
}

namespace extensions {
  class AppWindow;
}

namespace gfx {
  class Image;
}

namespace nw {
// browser
// ref in extensions/common/file_util.cc
void LoadNWAppAsExtensionHook(base::DictionaryValue* manifest,
                              const base::FilePath& extension_path,
                              std::string* error);
// ref in extensions/browser/app_window/app_window.cc 
void CalcNewWinParams(content::WebContents* new_contents, void* params,
                     std::string* nw_inject_js_doc_start,
                      std::string* nw_inject_js_doc_end,
                      const std::string& in_manifest);
// ref in extensions/components/native_app_window/native_app_window_views.cc
bool ExecuteAppCommandHook(int command_id, extensions::AppWindow* app_window);


//
// implemented in nw_chrome_browser_hooks.cc
//
// ref in extensions/browser/guest_view/web_view/web_view_guest.cc
void ShowDevtools(bool show, content::WebContents* web_contents, content::WebContents* container = nullptr);
// ref in content/nw/src/browser/nw_extensions_browser_hooks.cc
void SetAppIcon(gfx::Image &icon);

#if defined(OS_WIN)
// ref in content/nw/src/browser/nw_extensions_browser_hooks.cc
void SetAppHIcon(base::win::ScopedHICON icon);
// ref in content/nw/src/browser/nw_extensions_browser_hooks.cc
void SetWindowHIcon(base::win::ScopedHICON icon);
#endif

}

#endif // NW_EXTENSIONS_BROWSER_HOOKS_H_
