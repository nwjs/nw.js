#ifndef NW_API_SHELL_API_H_
#define NW_API_SHELL_API_H_

#include <vector>

#include "extensions/browser/extension_function.h"

namespace extensions {

class NwShellOpenItemFunction : public NWSyncExtensionFunction {
 public:
  NwShellOpenItemFunction();
  bool RunNWSync(base::Value::List* response, std::string* error) override;

 protected:
  ~NwShellOpenItemFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Shell.openItem", UNKNOWN)
};

class NwShellOpenExternalFunction : public NWSyncExtensionFunction {
 public:
  NwShellOpenExternalFunction();
  bool RunNWSync(base::Value::List* response, std::string* error) override;

 protected:
  ~NwShellOpenExternalFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Shell.openExternal", UNKNOWN)
};

class NwShellShowItemInFolderFunction : public NWSyncExtensionFunction {
 public:
  NwShellShowItemInFolderFunction();
  bool RunNWSync(base::Value::List* response, std::string* error) override;

 protected:
  ~NwShellShowItemInFolderFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Shell.showItemInFolder", UNKNOWN)
};

} // namespace extensions
#endif
