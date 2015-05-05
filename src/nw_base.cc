#include "nw_base.h"

#include "nw_package.h"

namespace nw {

namespace {
Package* g_package;
}

Package* package() {
  return g_package;
}

Package* InitNWPackage() {
  if (!g_package)
    g_package = new Package();
  return g_package;
}

void ReleaseNWPackage() {
  if (g_package) {
    delete g_package;
    g_package = nullptr;
  }
}

} //namespace nw
