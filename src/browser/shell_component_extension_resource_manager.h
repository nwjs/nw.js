// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NW_BROWSER_EXTENSIONS_CHROME_COMPONENT_EXTENSION_RESOURCE_MANAGER_H_
#define NW_BROWSER_EXTENSIONS_CHROME_COMPONENT_EXTENSION_RESOURCE_MANAGER_H_

#include <map>

#include "base/basictypes.h"
#include "base/files/file_path.h"
#include "extensions/browser/component_extension_resource_manager.h"

struct GritResourceMap;

namespace extensions {

class ShellComponentExtensionResourceManager
    : public ComponentExtensionResourceManager {
 public:
  ShellComponentExtensionResourceManager();
  ~ShellComponentExtensionResourceManager() override;

  // Overridden from ComponentExtensionResourceManager:
  bool IsComponentExtensionResource(const base::FilePath& extension_path,
                                    const base::FilePath& resource_path,
                                    int* resource_id) const override;

 private:
  void AddComponentResourceEntries(const GritResourceMap* entries, size_t size);

  // A map from a resource path to the resource ID.  Used by
  // IsComponentExtensionResource.
  std::map<base::FilePath, int> path_to_resource_id_;

  DISALLOW_COPY_AND_ASSIGN(ShellComponentExtensionResourceManager);
};

}  // namespace extensions

#endif  // NW_BROWSER_EXTENSIONS_CHROME_COMPONENT_EXTENSION_RESOURCE_MANAGER_H_
