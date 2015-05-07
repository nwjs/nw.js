#ifndef NWJS_BASE_H
#define NWJS_BASE_H
#include "content/nw/src/nw_export.h"
#include "content/nw/src/nw_package.h"

namespace nw {
  NW_EXPORT Package* package();
  NW_EXPORT Package* InitNWPackage();
  NW_EXPORT void ReleaseNWPackage();
  NW_EXPORT int ExitCodeHook();
  NW_EXPORT void SetExitCode(int);
}

#endif
