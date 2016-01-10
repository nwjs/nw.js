// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NW_BROWSER_FILE_PROTOCOLS_H_
#define NW_BROWSER_FILE_PROTOCOLS_H_

#include <string>

#include "net/url_request/url_request_job_factory.h"
#include "net/url_request/url_request_file_job.h"
#include "content/nw/src/browser/content_verify_job.h"
#include "base/timer/elapsed_timer.h"

namespace base {
class Time;
}

namespace net {
class HttpResponseHeaders;
}

namespace nw {

// Builds HTTP headers for an extension request. Hashes the time to avoid
// exposing the exact user installation time of the extension.
net::HttpResponseHeaders* BuildHttpHeaders(
    const std::string& content_security_policy,
    bool send_cors_header,
    const base::Time& last_modified_time);

// Creates the handlers for the chrome-extension:// scheme. Pass true for
// |is_incognito| only for incognito profiles and not for Chrome OS guest mode
// profiles.
net::URLRequestJobFactory::ProtocolHandler* CreateNWFileProtocolHandler(bool is_incognito);

class URLRequestNWFileJob : public net::URLRequestFileJob {
 public:
  URLRequestNWFileJob(net::URLRequest* request,
                      net::NetworkDelegate* network_delegate,
                      const base::FilePath& directory_path,
                      const base::FilePath& relative_path,
                      ContentVerifyJob* verify_job);
  void GetResponseInfo(net::HttpResponseInfo* info) override;

  void Start() override;

  bool IsRedirectResponse(GURL* location, int* http_status_code) override;

  void SetExtraRequestHeaders(const net::HttpRequestHeaders& headers) override;

  void OnSeekComplete(int64 result) override;
  void OnReadComplete(net::IOBuffer* buffer, int result) override;
  void CanStart();
  void set_can_start(bool flag) { can_start_ = flag; }

  ~URLRequestNWFileJob() override;

 private:
  void OnFilePathAndLastModifiedTimeRead(base::FilePath* read_file_path,
                                         base::Time* last_modified_time);

  scoped_refptr<ContentVerifyJob> verify_job_;

  scoped_ptr<base::ElapsedTimer> request_timer_;

  // The position we seeked to in the file.
  int64 seek_position_;

  // The number of bytes of content we read from the file.
  int bytes_read_;

  net::HttpResponseInfo response_info_;
  base::FilePath directory_path_, relative_path_;
  bool can_start_, started_;
  base::WeakPtrFactory<URLRequestNWFileJob> weak_factory_;
};


}  // namespace nw

#endif
