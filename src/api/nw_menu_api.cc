#include "content/nw/src/api/nw_menu_api.h"

#include "chrome/browser/devtools/devtools_window.h"
#include "chrome/browser/extensions/devtools_util.h"
#include "chrome/browser/extensions/extension_service.h"
#include "content/nw/src/api/menuitem/menuitem.h"
#include "content/nw/src/api/object_manager.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "extensions/browser/extension_system.h"
#include "extensions/common/error_utils.h"
#include "ui/base/clipboard/clipboard.h"

using nw::MenuItem;

namespace extensions {

NwMenuCreateItemFunction::NwMenuCreateItemFunction() {
}

NwMenuCreateItemFunction::~NwMenuCreateItemFunction() {
}

bool NwMenuCreateItemFunction::RunNWSync(base::ListValue* response, std::string* error) {
  base::DictionaryValue* props = nullptr;
  int id = 0;
  EXTENSION_FUNCTION_VALIDATE(args_->GetDictionary(0, &props));
  EXTENSION_FUNCTION_VALIDATE(
        props->GetInteger("generatedId", &id));

  nw::ObjectManager* manager = nw::ObjectManager::Get(browser_context());
  manager->OnAllocateObject(id, "MenuItem", *props, extension_id());
  return true;
}

} // namespace extensions
