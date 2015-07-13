#ifndef NW_API_OBJECT_API_H_
#define NW_API_OBJECT_API_H_

#include <vector>

#include "extensions/browser/extension_function.h"

namespace extensions {

class NwObjectCreateFunction : public NWSyncExtensionFunction {
 public:
  NwObjectCreateFunction();
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwObjectCreateFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Object.create", UNKNOWN)
 private:
  DISALLOW_COPY_AND_ASSIGN(NwObjectCreateFunction);
};

class NwObjectDestroyFunction : public NWSyncExtensionFunction {
 public:
  NwObjectDestroyFunction();
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwObjectDestroyFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Object.destroy", UNKNOWN)
 private:
  DISALLOW_COPY_AND_ASSIGN(NwObjectDestroyFunction);
};

class NwObjectCallObjectMethodFunction : public NWSyncExtensionFunction {
 public:
  NwObjectCallObjectMethodFunction();
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwObjectCallObjectMethodFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Object.callObjectMethod", UNKNOWN)
 private:
  DISALLOW_COPY_AND_ASSIGN(NwObjectCallObjectMethodFunction);
};

class NwObjectCallObjectMethodSyncFunction : public NWSyncExtensionFunction {
 public:
  NwObjectCallObjectMethodSyncFunction();
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwObjectCallObjectMethodSyncFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Object.callObjectMethodSync", UNKNOWN)
 private:
  DISALLOW_COPY_AND_ASSIGN(NwObjectCallObjectMethodSyncFunction);
};

} // namespace extensions
#endif
