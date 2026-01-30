#include "content/nw/src/api/nw_shell_api.h"

#include "chrome/browser/devtools/devtools_window.h"
#include "chrome/browser/extensions/devtools_util.h"
#include "chrome/browser/extensions/extension_service.h"
#include "content/nw/src/api/object_manager.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/web_contents.h"
#include "extensions/browser/extension_system.h"
#include "extensions/common/error_utils.h"

namespace extensions {

NwShellOpenItemFunction::NwShellOpenItemFunction() {
}

NwShellOpenItemFunction::~NwShellOpenItemFunction() {
}

bool NwShellOpenItemFunction::RunNWSync(base::ListValue* response, std::string* error) {
  nw::ObjectManager* manager = nw::ObjectManager::Get(browser_context());
  manager->OnCallStaticMethod(render_frame_host(), "Shell", "OpenItem", args());
  return true;
}

NwShellOpenExternalFunction::NwShellOpenExternalFunction() {
}

NwShellOpenExternalFunction::~NwShellOpenExternalFunction() {
}

bool NwShellOpenExternalFunction::RunNWSync(base::ListValue* response, std::string* error) {
  nw::ObjectManager* manager = nw::ObjectManager::Get(browser_context());
  manager->OnCallStaticMethod(render_frame_host(), "Shell", "OpenExternal", args());
  return true;
}

NwShellShowItemInFolderFunction::NwShellShowItemInFolderFunction() {
}

NwShellShowItemInFolderFunction::~NwShellShowItemInFolderFunction() {
}

bool NwShellShowItemInFolderFunction::RunNWSync(base::ListValue* response, std::string* error) {
  nw::ObjectManager* manager = nw::ObjectManager::Get(browser_context());
  manager->OnCallStaticMethod(render_frame_host(), "Shell", "ShowItemInFolder", args());
  return true;
}

} // namespace extensions
