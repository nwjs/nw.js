#ifndef NW_API_CLIPBOARD_API_H_
#define NW_API_CLIPBOARD_API_H_

#include <vector>

#include "extensions/browser/extension_function.h"

namespace extensions {

class NwClipboardGetListSyncFunction : public NWSyncExtensionFunction {
 public:
  NwClipboardGetListSyncFunction();
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwClipboardGetListSyncFunction() override;


  DECLARE_EXTENSION_FUNCTION("nw.Clipboard.getListSync", UNKNOWN)
 private:
};

class NwClipboardSetListSyncFunction : public NWSyncExtensionFunction {
 public:
  NwClipboardSetListSyncFunction();
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwClipboardSetListSyncFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Clipboard.setListSync", UNKNOWN)
 private:
};

class NwClipboardClearSyncFunction : public NWSyncExtensionFunction {
 public:
  NwClipboardClearSyncFunction();
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwClipboardClearSyncFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Clipboard.clearSync", UNKNOWN)
 private:
};

class NwClipboardReadAvailableTypesFunction : public NWSyncExtensionFunction {
 public:
  NwClipboardReadAvailableTypesFunction();
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwClipboardReadAvailableTypesFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Clipboard.readAvailableTypes", UNKNOWN)
 private:
};


} // namespace extensions
#endif
