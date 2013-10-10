// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NW_SHELL_QUOTA_PERMISSION_CONTEXT_H_
#define NW_SHELL_QUOTA_PERMISSION_CONTEXT_H_

#include "base/compiler_specific.h"
#include "content/public/browser/quota_permission_context.h"

namespace content {

class ShellQuotaPermissionContext : public QuotaPermissionContext {
 public:
  ShellQuotaPermissionContext();

  // The callback will be dispatched on the IO thread.
  virtual void RequestQuotaPermission(
      const GURL& origin_url,
      quota::StorageType type,
      int64 new_quota,
      int render_process_id,
      int render_view_id,
      const PermissionCallback& callback) OVERRIDE;

 private:
  virtual ~ShellQuotaPermissionContext();

  DISALLOW_COPY_AND_ASSIGN(ShellQuotaPermissionContext);
};

}  // namespace content

#endif
