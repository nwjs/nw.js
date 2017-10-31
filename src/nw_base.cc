#include "nw_base.h"

#include "nw_package.h"
#include "base/command_line.h"

namespace nw {

namespace {
Package* g_package;
int exit_code;
base::string16 g_current_new_win_manifest;
}

Package* package(const base::FilePath* path) {
  if (path) {
    if (g_package)
      delete g_package;
    g_package = new Package(*path);
  }
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
  const base::CommandLine* cmdline = base::CommandLine::ForCurrentProcess();
  if (cmdline->HasSwitch("nwjs-test-mode"))
    return 0;
  return exit_code;
}

void SetCurrentNewWinManifest(const base::string16& manifest) {
  g_current_new_win_manifest = manifest;
}

const base::string16& GetCurrentNewWinManifest() {
  return g_current_new_win_manifest;
}

bool gcm_enabled() {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  return command_line->HasSwitch("enable-gcm");
}


} //namespace nw
