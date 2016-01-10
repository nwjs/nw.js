// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/content_hash_tree.h"

#include "base/memory/scoped_ptr.h"
#include "base/stl_util.h"
#include "crypto/secure_hash.h"
#include "crypto/sha2.h"

namespace nw {

std::string ComputeTreeHashRoot(const std::vector<std::string>& leaf_hashes,
                                int branch_factor) {
  if (leaf_hashes.empty() || branch_factor < 2)
    return std::string();

  // The nodes of the tree we're currently operating on.
  std::vector<std::string> current_nodes;

  // We avoid having to copy all of the input leaf nodes into |current_nodes|
  // by using a pointer. So the first iteration of the loop this points at
  // |leaf_hashes|, but thereafter it points at |current_nodes|.
  const std::vector<std::string>* current = &leaf_hashes;

  // Where we're inserting new hashes computed from the current level.
  std::vector<std::string> parent_nodes;

  while (current->size() > 1) {
    // Iterate over the current level of hashes, computing the hash of up to
    // |branch_factor| elements to form the hash of each parent node.
    std::vector<std::string>::const_iterator i = current->begin();
    while (i != current->end()) {
      scoped_ptr<crypto::SecureHash> hash(
          crypto::SecureHash::Create(crypto::SecureHash::SHA256));
      for (int j = 0; j < branch_factor && i != current->end(); j++) {
        DCHECK_EQ(i->size(), crypto::kSHA256Length);
        hash->Update(i->data(), i->size());
        ++i;
      }
      parent_nodes.push_back(std::string(crypto::kSHA256Length, 0));
      std::string* output = &(parent_nodes.back());
      hash->Finish(string_as_array(output), output->size());
    }
    current_nodes.swap(parent_nodes);
    parent_nodes.clear();
    current = &current_nodes;
  }
  DCHECK_EQ(1u, current->size());
  return (*current)[0];
}

}  // namespace nw
