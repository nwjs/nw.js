// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// A wrapper around quota::PersistentStoragePolicy used by the net storage
// backends in chrome/browser/net to decide what to delete at shutdown.

#ifndef CHROME_BROWSER_NET_CLEAR_ON_EXIT_POLICY_H_
#define CHROME_BROWSER_NET_CLEAR_ON_EXIT_POLICY_H_

#include <string>

#include "base/basictypes.h"
#include "base/memory/ref_counted.h"

namespace quota {
class SpecialStoragePolicy;
}

class ClearOnExitPolicy : public base::RefCountedThreadSafe<ClearOnExitPolicy> {
 public:
  // |special_storage_policy| can be NULL, if no policy is to be applied.
  // Otherwise, will keep a scoped_refptr to |special_storage_policy|
  // throughout its lifetime.
  explicit ClearOnExitPolicy(
      quota::SpecialStoragePolicy* special_storage_policy);

  // True if there are origins that should be cleared on exit.
  bool HasClearOnExitOrigins();

  // True if the given origin (defined by the |domain| and whether or not the
  // |scheme_is_secure|) should be cleared on exit.
  bool ShouldClearOriginOnExit(const std::string& domain,
                               bool scheme_is_secure);

 private:
  friend class base::RefCountedThreadSafe<ClearOnExitPolicy>;

  virtual ~ClearOnExitPolicy();

  scoped_refptr<quota::SpecialStoragePolicy> special_storage_policy_;

  DISALLOW_COPY_AND_ASSIGN(ClearOnExitPolicy);
};

#endif  // CHROME_BROWSER_NET_CLEAR_ON_EXIT_POLICY_H_
