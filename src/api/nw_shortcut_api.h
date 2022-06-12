#ifndef NW_SRC_API_NW_SHORTCUT_API_H
#define NW_SRC_API_NW_SHORTCUT_API_H

#include "extensions/browser/extension_function.h"

namespace extensions {

  class NwShortcutRegisterAcceleratorFunction : public NWSyncExtensionFunction
  {
  public:
    NwShortcutRegisterAcceleratorFunction() {}
    bool RunNWSync(base::Value::List* response, std::string* error) override;

  protected:
    ~NwShortcutRegisterAcceleratorFunction() override {}
    DECLARE_EXTENSION_FUNCTION("nw.Shortcut.registerAccelerator", UNKNOWN)

  };

  class NwShortcutUnregisterAcceleratorFunction : public NWSyncExtensionFunction
  {
  public:
    NwShortcutUnregisterAcceleratorFunction() {}
    bool RunNWSync(base::Value::List* response, std::string* error) override;

  protected:
    ~NwShortcutUnregisterAcceleratorFunction() override {}
    DECLARE_EXTENSION_FUNCTION("nw.Shortcut.unregisterAccelerator", UNKNOWN)
  };

} // extensions

#endif // NW_SRC_API_NW_SHORTCUT_API_H
