// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NW_BROWSER_CONTENT_VERIFIER_IO_DATA_H_
#define NW_BROWSER_CONTENT_VERIFIER_IO_DATA_H_

#include <map>
#include <set>
#include <string>

#include "base/files/file_path.h"
#include "base/memory/linked_ptr.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/version.h"

namespace nw {

// A helper class for keeping track of data for the ContentVerifier that should
// only be accessed on the IO thread.
class ContentVerifierIOData
    : public base::RefCountedThreadSafe<ContentVerifierIOData> {
 public:
  struct ExtensionData {
    scoped_ptr<std::set<base::FilePath>> browser_image_paths;
    base::Version version;

    ExtensionData(scoped_ptr<std::set<base::FilePath>> browser_image_paths,
                  const base::Version& version);
    ~ExtensionData();
  };

  ContentVerifierIOData();

  void AddData(const std::string& extension_id, scoped_ptr<ExtensionData> data);
  void RemoveData(const std::string& extension_id);
  void Clear();

  // This should be called on the IO thread, and the return value should not
  // be retained or used on other threads.
  const ExtensionData* GetData(const std::string& extension_id);

 protected:
  friend class base::RefCountedThreadSafe<ContentVerifierIOData>;
  virtual ~ContentVerifierIOData();

  std::map<std::string, linked_ptr<ExtensionData> > data_map_;
};

}  // namespace nw

#endif  // NW_BROWSER_CONTENT_VERIFIER_IO_DATA_H_
