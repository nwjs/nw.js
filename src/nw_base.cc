#include "nw_base.h"

#include "nw_package.h"

namespace nw {

namespace {
Package* g_package;
int exit_code;
base::string16 g_current_new_win_manifest;
}

Package* package() {
  return g_package;
}

Package* InitNWPackage() {
  if (!g_package)
    g_package = new Package();
  exit_code = 0;
  return g_package;
}

void ReleaseNWPackage() {
  if (g_package) {
    delete g_package;
    g_package = nullptr;
  }
}

void SetExitCode(int code) {
  exit_code = code;
}

int ExitCodeHook() {
  return exit_code;
}

void SetCurrentNewWinManifest(const base::string16& manifest) {
  g_current_new_win_manifest = manifest;
}

const base::string16& GetCurrentNewWinManifest() {
  return g_current_new_win_manifest;
}


} //namespace nw
