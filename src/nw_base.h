#ifndef NWJS_BASE_H
#define NWJS_BASE_H
#include "content/nw/src/nw_export.h"
#include "content/nw/src/nw_package.h"

namespace base {
class FilePath;
}

namespace nw {
  NW_EXPORT Package* package(const base::FilePath* path = nullptr);
  NW_EXPORT Package* InitNWPackage();
  NW_EXPORT void ReleaseNWPackage();
  NW_EXPORT int ExitCodeHook();
  NW_EXPORT void SetExitCode(int);
  NW_EXPORT void SetCurrentNewWinManifest(const std::u16string& manifest);
  NW_EXPORT const std::u16string& GetCurrentNewWinManifest();
  NW_EXPORT bool gcm_enabled();
}

#endif
