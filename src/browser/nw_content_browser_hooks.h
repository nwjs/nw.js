#ifndef NW_CONTENT_BROWSER_HOOKS_H_
#define NW_CONTENT_BROWSER_HOOKS_H_

class GURL;

namespace base {
  class FilePath;
}

namespace gfx {
  class Image;
}

namespace content {
  class RenderProcessHost;
}

namespace nw {

class Package;

// browser
// ref in content/nw/src/api/tray/tray_aura.cc
// content/nw/src/api/menuitem/menuitem_views.cc
bool GetImage(Package* package, const base::FilePath& icon_path, gfx::Image* image);

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
