// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NW_BROWSER_COMPUTED_HASHES_H_
#define NW_BROWSER_COMPUTED_HASHES_H_

#include <map>
#include <string>
#include <vector>

#include "base/memory/scoped_ptr.h"

namespace base {
class FilePath;
class ListValue;
}

namespace nw {

// A pair of classes for serialization of a set of SHA256 block hashes computed
// over the files inside an extension.
class ComputedHashes {
 public:
  class Reader {
   public:
    Reader();
    ~Reader();
    bool InitFromFile(const base::FilePath& path);

    // The block size and hashes for |relative_path| will be copied into the
    // out parameters.
    bool GetHashes(const base::FilePath& relative_path,
                   int* block_size,
                   std::vector<std::string>* hashes);

   private:
    typedef std::pair<int, std::vector<std::string> > HashInfo;

    // This maps a relative path to a pair of (block size, hashes)
    std::map<base::FilePath, HashInfo> data_;
  };

  class Writer {
   public:
    Writer();
    ~Writer();

    // Adds hashes for |relative_path|. Should not be called more than once
    // for a given |relative_path|.
    void AddHashes(const base::FilePath& relative_path,
                   int block_size,
                   const std::vector<std::string>& hashes);

    bool WriteToFile(const base::FilePath& path);

   private:
    // Each element of this list contains the path and block hashes for one
    // file.
    scoped_ptr<base::ListValue> file_list_;
  };

  // Computes the SHA256 hash of each |block_size| chunk in |contents|, placing
  // the results into |hashes|.
  static void ComputeHashesForContent(const std::string& contents,
                                      size_t block_size,
                                      std::vector<std::string>* hashes);
};

}  // namespace nw

#endif  // NW_BROWSER_COMPUTED_HASHES_H_
