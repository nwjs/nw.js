#ifndef NW_API_WINDOW_API_H_
#define NW_API_WINDOW_API_H_

#include <vector>

#include "extensions/browser/extension_function.h"

namespace extensions {

class NwCurrentWindowInternalShowDevToolsFunction : public AsyncExtensionFunction {
 public:
  NwCurrentWindowInternalShowDevToolsFunction();

 protected:
  ~NwCurrentWindowInternalShowDevToolsFunction() override;

  // ExtensionFunction:
  bool RunAsync() override;
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.showDevTools", NW_CURRENTWINDOWINTERNAL_SHOWDEVTOOLS)
 private:
  void Callback();
};

} // namespace extensions
#endif
