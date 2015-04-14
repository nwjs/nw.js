#include "content/nw/src/api/nw_window_api.h"

#include "chrome/browser/devtools/devtools_window.h"
#include "chrome/browser/extensions/devtools_util.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "extensions/common/error_utils.h"

namespace extensions {
NwCurrentWindowInternalShowDevToolsFunction::NwCurrentWindowInternalShowDevToolsFunction() {

}

NwCurrentWindowInternalShowDevToolsFunction::~NwCurrentWindowInternalShowDevToolsFunction() {
}

bool NwCurrentWindowInternalShowDevToolsFunction::RunAsync() {
  content::RenderViewHost* rvh = render_view_host();
  DevToolsWindow::OpenDevToolsWindow(
      content::WebContents::FromRenderViewHost(rvh));
  SendResponse(true);
  return true;
}

} // namespace extensions
