#include "nw_base.h"

#include "nw_package.h"
#include "base/command_line.h"

#pragma clang diagnostic ignored "-Wexit-time-destructors"

namespace nw {

namespace {
Package* g_package;
int exit_code;
std::u16string g_current_new_win_manifest;
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
  if (!g_package) {
    const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
    std::string process_type = command_line.GetSwitchValueASCII("type");
    if (!process_type.empty()) {
      base::FilePath pkg_path = command_line.GetSwitchValuePath("nwapp-path");
      g_package = new Package(pkg_path);
    } else
      g_package = new Package();
  }
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

void SetCurrentNewWinManifest(const std::u16string& manifest) {
  g_current_new_win_manifest = manifest;
}

const std::u16string& GetCurrentNewWinManifest() {
  return g_current_new_win_manifest;
}

bool gcm_enabled() {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  return command_line->HasSwitch("enable-gcm");
}


} //namespace nw
