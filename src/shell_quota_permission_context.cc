// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/shell_quota_permission_context.h"

#include "webkit/common/quota/quota_types.h"

namespace content {

ShellQuotaPermissionContext::ShellQuotaPermissionContext() {}

void ShellQuotaPermissionContext::RequestQuotaPermission(
    const StorageQuotaParams& params,
    int render_process_id,
    const PermissionCallback& callback) {
  callback.Run(QUOTA_PERMISSION_RESPONSE_ALLOW);
}

ShellQuotaPermissionContext::~ShellQuotaPermissionContext() {}

}  // namespace content
