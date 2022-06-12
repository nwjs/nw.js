#include "content/nw/src/api/nw_menu_api.h"

#include "chrome/browser/extensions/extension_service.h"
#include "content/nw/src/api/menuitem/menuitem.h"
#include "content/nw/src/api/object_manager.h"
#include "content/public/browser/render_view_host.h"
#include "extensions/browser/extension_system.h"
#include "extensions/common/error_utils.h"

using nw::MenuItem;

namespace extensions {

#ifndef OS_MAC
bool NwMenuGetNSStringWithFixupFunction::RunNWSync(base::Value::List* response, std::string* error) {
  SetError("NwMenuGetNSStringWithFixupFunction is only for OSX");
  return false;
}

bool NwMenuGetNSStringFWithFixupFunction::RunNWSync(base::Value::List* response, std::string* error) {
  SetError("NwMenuGetNSStringFWithFixupFunction is only for OSX");
  return false;
}
#endif
} // namespace extensions
