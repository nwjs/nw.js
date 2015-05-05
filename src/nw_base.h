#ifndef NWJS_BASE_H
#define NWJS_BASE_H

#include "content/nw/src/nw_package.h"

namespace nw {
  Package* package();
  Package* InitNWPackage();
  void ReleaseNWPackage();
}

#endif
