#ifndef NW_API_APP_API_H_
#define NW_API_APP_API_H_

#include <vector>

#include "extensions/browser/extension_function.h"

namespace extensions {

class NwAppQuitFunction : public AsyncExtensionFunction {
 public:
  NwAppQuitFunction();

 protected:
  ~NwAppQuitFunction() override;

  // ExtensionFunction:
  bool RunAsync() override;
  DECLARE_EXTENSION_FUNCTION("nw.App.quit", UNKNOWN)
 private:
  void Callback();
};

} // namespace extensions
#endif
