#ifndef NW_API_MENU_API_H_
#define NW_API_MENU_API_H_

#include <vector>

#include "extensions/browser/extension_function.h"

namespace extensions {

class NwMenuGetNSStringWithFixupFunction : public NWSyncExtensionFunction {
 public:
  NwMenuGetNSStringWithFixupFunction(){}
  bool RunNWSync(base::ListValue* response, std::string* error) override;
    
 protected:
  ~NwMenuGetNSStringWithFixupFunction() override {}
    
  DECLARE_EXTENSION_FUNCTION("nw.Menu.getNSStringWithFixup", UNKNOWN)
 private:
  DISALLOW_COPY_AND_ASSIGN(NwMenuGetNSStringWithFixupFunction);
};

class NwMenuGetNSStringFWithFixupFunction : public NWSyncExtensionFunction {
 public:
  NwMenuGetNSStringFWithFixupFunction() {}
  bool RunNWSync(base::ListValue* response, std::string* error) override;
    
 protected:
  ~NwMenuGetNSStringFWithFixupFunction() override {}
    
  DECLARE_EXTENSION_FUNCTION("nw.Menu.getNSStringFWithFixup", UNKNOWN)
 private:
  DISALLOW_COPY_AND_ASSIGN(NwMenuGetNSStringFWithFixupFunction);
};

} // namespace extensions
#endif
