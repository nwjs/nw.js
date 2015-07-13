#ifndef NW_API_MENU_API_H_
#define NW_API_MENU_API_H_

#include <vector>

#include "extensions/browser/extension_function.h"

namespace extensions {

class NwMenuCreateItemFunction : public NWSyncExtensionFunction {
 public:
  NwMenuCreateItemFunction();
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwMenuCreateItemFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Menu.createItem", UNKNOWN)
 private:
  DISALLOW_COPY_AND_ASSIGN(NwMenuCreateItemFunction);
};

} // namespace extensions
#endif
