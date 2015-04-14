#ifndef NWJS_CONTENT_HOOKS_H
#define NWJS_CONTENT_HOOKS_H

#include "nw_package.h"

namespace base {
  class DictionaryValue;
}

namespace extensions {
  class ScriptContext;
}

namespace nw {
class Package;
int MainPartsPreCreateThreadsHook();
void MainPartsPostDestroyThreadsHook();
Package* package();
void ContextCreationHook(extensions::ScriptContext* context);
void LoadNWAppAsExtensionHook(base::DictionaryValue* manifest);
}

#endif
