#ifndef NW_API_SHELL_API_H_
#define NW_API_SHELL_API_H_

#include <vector>

#include "extensions/browser/extension_function.h"

namespace extensions {

class NwShellOpenItemFunction : public NWSyncExtensionFunction {
 public:
  NwShellOpenItemFunction();
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwShellOpenItemFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Shell.openItem", UNKNOWN)
 private:
  DISALLOW_COPY_AND_ASSIGN(NwShellOpenItemFunction);
};

class NwShellOpenExternalFunction : public NWSyncExtensionFunction {
 public:
  NwShellOpenExternalFunction();
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwShellOpenExternalFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Shell.openExternal", UNKNOWN)
 private:
  DISALLOW_COPY_AND_ASSIGN(NwShellOpenExternalFunction);
};

class NwShellShowItemInFolderFunction : public NWSyncExtensionFunction {
 public:
  NwShellShowItemInFolderFunction();
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwShellShowItemInFolderFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Shell.showItemInFolder", UNKNOWN)
 private:
  DISALLOW_COPY_AND_ASSIGN(NwShellShowItemInFolderFunction);
};

} // namespace extensions
#endif
