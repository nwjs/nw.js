// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NW_BROWSER_CONTENT_HASH_TREE_H_
#define NW_BROWSER_CONTENT_HASH_TREE_H_

#include <string>
#include <vector>

namespace nw {

// This takes a list of sha256 hashes, considers them to be leaf nodes of a
// hash tree (aka Merkle tree), and computes the root node of the tree using
// the given branching factor to hash lower level nodes together. Tree hash
// implementations differ in how they handle the case where the number of
// leaves isn't an integral power of the branch factor. This implementation
// just hashes together however many are left at a given level, even if that is
// less than the branching factor (instead of, for instance, directly promoting
// elements). E.g., imagine we use a branch factor of 3 for a vector of 4 leaf
// nodes [A,B,C,D]. This implemention will compute the root hash G as follows:
//
// |      G      |
// |     / \     |
// |    E   F    |
// |   /|\   \   |
// |  A B C   D  |
//
// where E = Hash(A||B||C), F = Hash(D), and G = Hash(E||F)
//
// The one exception to this rule is when there is only one node left. This
// means that the root hash of any vector with just one leaf is the same as
// that leaf. Ie RootHash([A]) == A, not Hash(A).
std::string ComputeTreeHashRoot(const std::vector<std::string>& leaf_hashes,
                                int branch_factor);

}  // namespace nw

#endif  // NW_BROWSER_CONTENT_HASH_TREE_H_
