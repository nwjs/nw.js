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
  DISALLOW_COPY_AND_ASSIGN(NwObjAllocateIdFunction);
};

class NwObjCreateFunction : public NWSyncExtensionFunction {
 public:
  NwObjCreateFunction();
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwObjCreateFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Obj.create", UNKNOWN)
 private:
  DISALLOW_COPY_AND_ASSIGN(NwObjCreateFunction);
};

class NwObjDestroyFunction : public NWSyncExtensionFunction {
 public:
  NwObjDestroyFunction();
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwObjDestroyFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Obj.destroy", UNKNOWN)
 private:
  DISALLOW_COPY_AND_ASSIGN(NwObjDestroyFunction);
};

class NwObjCallObjectMethodFunction : public NWSyncExtensionFunction {
 public:
  NwObjCallObjectMethodFunction();
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwObjCallObjectMethodFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Obj.callObjectMethod", UNKNOWN)
 private:
  DISALLOW_COPY_AND_ASSIGN(NwObjCallObjectMethodFunction);
};

class NwObjCallObjectMethodSyncFunction : public NWSyncExtensionFunction {
 public:
  NwObjCallObjectMethodSyncFunction();
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwObjCallObjectMethodSyncFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Obj.callObjectMethodSync", UNKNOWN)
 private:
  DISALLOW_COPY_AND_ASSIGN(NwObjCallObjectMethodSyncFunction);
};

class NwObjCallObjectMethodAsyncFunction : public ExtensionFunction {
 public:
  NwObjCallObjectMethodAsyncFunction();

 protected:
  ResponseAction Run() override;
  ~NwObjCallObjectMethodAsyncFunction() override;

  DECLARE_EXTENSION_FUNCTION("nw.Obj.callObjectMethodAsync", UNKNOWN)
 private:
  DISALLOW_COPY_AND_ASSIGN(NwObjCallObjectMethodAsyncFunction);
};

} // namespace extensions
#endif
