// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NW_APP_PROTOCOL_HANDLER_H_
#define NW_APP_PROTOCOL_HANDLER_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/files/file_path.h"
#include "net/url_request/url_request_job_factory.h"

class GURL;

namespace net {

class NetworkDelegate;
class URLRequestJob;

// Implements a ProtocolHandler for File jobs. If |network_delegate_| is NULL,
// then all file requests will fail with ERR_ACCESS_DENIED.
class AppProtocolHandler :
    public URLRequestJobFactory::ProtocolHandler {
 public:
  AppProtocolHandler(const base::FilePath& root);
  URLRequestJob* MaybeCreateJob(
      URLRequest* request, NetworkDelegate* network_delegate) const override;
  bool IsSafeRedirectTarget(const GURL& location) const override;

 private:
  base::FilePath root_path_;
  DISALLOW_COPY_AND_ASSIGN(AppProtocolHandler);
};

}  // namespace net

#endif
