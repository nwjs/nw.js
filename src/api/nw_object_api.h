#ifndef NW_API_OBJECT_API_H_
#define NW_API_OBJECT_API_H_

#include <vector>

#include "extensions/browser/extension_function.h"

namespace extensions {

class NwObjAllocateIdFunction : public NWSyncExtensionFunction {
 public:
  NwObjAllocateIdFunction();
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwObjAllocateIdFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Obj.allocateId", UNKNOWN)
 private:
};

class NwObjCreateFunction : public NWSyncExtensionFunction {
 public:
  NwObjCreateFunction();
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwObjCreateFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Obj.create", UNKNOWN)
 private:
};

class NwObjDestroyFunction : public NWSyncExtensionFunction {
 public:
  NwObjDestroyFunction();
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwObjDestroyFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Obj.destroy", UNKNOWN)
 private:
};

class NwObjCallObjectMethodFunction : public NWSyncExtensionFunction {
 public:
  NwObjCallObjectMethodFunction();
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwObjCallObjectMethodFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Obj.callObjectMethod", UNKNOWN)
 private:
};

class NwObjCallObjectMethodSyncFunction : public NWSyncExtensionFunction {
 public:
  NwObjCallObjectMethodSyncFunction();
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwObjCallObjectMethodSyncFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Obj.callObjectMethodSync", UNKNOWN)
 private:
};

class NwObjCallObjectMethodAsyncFunction : public ExtensionFunction {
 public:
  NwObjCallObjectMethodAsyncFunction();

 protected:
  ResponseAction Run() override;
  ~NwObjCallObjectMethodAsyncFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Obj.callObjectMethodAsync", UNKNOWN)
 private:
};

} // namespace extensions
#endif
