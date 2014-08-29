// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/net/clear_on_exit_policy.h"

#include "content/public/common/url_constants.h"
#include "url/gurl.h"
#include "webkit/browser/quota/special_storage_policy.h"

ClearOnExitPolicy::ClearOnExitPolicy(
    quota::SpecialStoragePolicy* special_storage_policy)
    : special_storage_policy_(special_storage_policy) {
}

ClearOnExitPolicy::~ClearOnExitPolicy() {
}

bool ClearOnExitPolicy::HasClearOnExitOrigins() {
  return special_storage_policy_.get() &&
         special_storage_policy_->HasSessionOnlyOrigins();
}

bool ClearOnExitPolicy::ShouldClearOriginOnExit(const std::string& domain,
                                                bool scheme_is_secure) {
  if (domain.length() == 0)
    return false;

  std::string scheme =
      scheme_is_secure ? url::kHttpsScheme : url::kHttpScheme;
  std::string host = domain[0] == '.' ? domain.substr(1) : domain;
  GURL url(scheme + url::kStandardSchemeSeparator + host);

  return special_storage_policy_->IsStorageSessionOnly(url);
}
