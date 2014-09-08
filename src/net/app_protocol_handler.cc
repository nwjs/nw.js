// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/net/app_protocol_handler.h"

#include "base/base64.h"
#include "base/file_util.h"
#include "base/files/file_path.h"
#include "base/format_macros.h"
#include "base/logging.h"
#include "base/sha1.h"
#include "base/strings/stringprintf.h"
#include "base/threading/sequenced_worker_pool.h"
#include "content/public/browser/browser_thread.h"
#include "net/base/mime_util.h"
#include "net/base/net_errors.h"
#include "net/base/net_util.h"
#include "net/http/http_request_headers.h"
#include "net/http/http_response_headers.h"
#include "net/http/http_response_info.h"
#include "net/url_request/url_request.h"
#include "net/url_request/url_request_error_job.h"
#include "net/url_request/url_request_file_dir_job.h"
#include "net/url_request/url_request_file_job.h"

namespace net {

namespace {

net::HttpResponseHeaders* BuildHttpHeaders(
    const std::string& content_security_policy, bool send_cors_header,
    const base::Time& last_modified_time) {
  std::string raw_headers;
  raw_headers.append("HTTP/1.1 200 OK");
  if (!content_security_policy.empty()) {
    raw_headers.append(1, '\0');
    raw_headers.append("Content-Security-Policy: ");
    raw_headers.append(content_security_policy);
  }

  if (send_cors_header) {
    raw_headers.append(1, '\0');
    raw_headers.append("Access-Control-Allow-Origin: *");
  }

  if (!last_modified_time.is_null()) {
    // Hash the time and make an etag to avoid exposing the exact
    // user installation time of the extension.
    std::string hash = base::StringPrintf("%" PRId64,
                                          last_modified_time.ToInternalValue());
    hash = base::SHA1HashString(hash);
    std::string etag;
    base::Base64Encode(hash, &etag);
    raw_headers.append(1, '\0');
    raw_headers.append("ETag: \"");
    raw_headers.append(etag);
    raw_headers.append("\"");
    // Also force revalidation.
    raw_headers.append(1, '\0');
    raw_headers.append("cache-control: no-cache");
  }

  raw_headers.append(2, '\0');
  return new net::HttpResponseHeaders(raw_headers);
}

base::Time GetFileLastModifiedTime(const base::FilePath& filename) {
  if (base::PathExists(filename)) {
    base::File::Info info;
    if (base::GetFileInfo(filename, &info))
      return info.last_modified;
  }
  return base::Time();
}

void ReadResourceFilePathAndLastModifiedTime(
    const base::FilePath& file_path,
    base::Time* last_modified_time) {
  *last_modified_time = GetFileLastModifiedTime(file_path);
}

class URLRequestNWAppJob : public net::URLRequestFileJob {
 public:
  URLRequestNWAppJob(net::URLRequest* request,
                         net::NetworkDelegate* network_delegate,
                         const base::FilePath& file_path,
                         const std::string& content_security_policy,
                         bool send_cors_header)
    : net::URLRequestFileJob(
          request, network_delegate, base::FilePath(),
          content::BrowserThread::GetBlockingPool()->
              GetTaskRunnerWithShutdownBehavior(
                  base::SequencedWorkerPool::SKIP_ON_SHUTDOWN)),
      content_security_policy_(content_security_policy),
      send_cors_header_(send_cors_header),
      weak_factory_(this) {
    file_path_ = file_path;
    // response_info_.headers = BuildHttpHeaders(content_security_policy,
    //                                           send_cors_header,
    //                                           base::Time());
  }

  virtual void GetResponseInfo(net::HttpResponseInfo* info) OVERRIDE {
    *info = response_info_;
  }

  virtual void Start() OVERRIDE {
    base::Time* last_modified_time = new base::Time();
    bool posted = content::BrowserThread::PostBlockingPoolTaskAndReply(
        FROM_HERE,
        base::Bind(&ReadResourceFilePathAndLastModifiedTime,
                   file_path_,
                   base::Unretained(last_modified_time)),
        base::Bind(&URLRequestNWAppJob::OnFilePathAndLastModifiedTimeRead,
                   weak_factory_.GetWeakPtr(),
                   base::Owned(last_modified_time)));
    DCHECK(posted);
  }

 private:
  virtual ~URLRequestNWAppJob() {}

  void OnFilePathAndLastModifiedTimeRead(base::Time* last_modified_time) {
    response_info_.headers = BuildHttpHeaders(
        content_security_policy_,
        send_cors_header_,
        *last_modified_time);
    URLRequestFileJob::Start();
  }

  net::HttpResponseInfo response_info_;
  std::string content_security_policy_;
  bool send_cors_header_;
  base::WeakPtrFactory<URLRequestNWAppJob> weak_factory_;
};

} // namespace

AppProtocolHandler::AppProtocolHandler(const base::FilePath& root)
  :root_path_(root)
{
}

URLRequestJob* AppProtocolHandler::MaybeCreateJob(
    URLRequest* request, NetworkDelegate* network_delegate) const {
  base::FilePath file_path;
  GURL url(request->url());
  url_canon::Replacements<char> replacements;
  replacements.SetScheme("file", url_parse::Component(0, 4));
  replacements.ClearHost();
  url = url.ReplaceComponents(replacements);

  const bool is_file = FileURLToFilePath(url, &file_path);

  file_path = root_path_.Append(file_path);
  // Check file access permissions.
  if (!network_delegate ||
      !network_delegate->CanAccessFile(*request, file_path)) {
    return new URLRequestErrorJob(request, network_delegate, ERR_ACCESS_DENIED);
  }

  // We need to decide whether to create URLRequestFileJob for file access or
  // URLRequestFileDirJob for directory access. To avoid accessing the
  // filesystem, we only look at the path string here.
  // The code in the URLRequestFileJob::Start() method discovers that a path,
  // which doesn't end with a slash, should really be treated as a directory,
  // and it then redirects to the URLRequestFileDirJob.
  if (is_file &&
      file_path.EndsWithSeparator() &&
      file_path.IsAbsolute()) {
    return new URLRequestFileDirJob(request, network_delegate, file_path);
  }

  // Use a regular file request job for all non-directories (including invalid
  // file names).
  return new URLRequestNWAppJob(request, network_delegate, file_path, "", false);
}

bool AppProtocolHandler::IsSafeRedirectTarget(const GURL& location) const {
  return false;
}

}  // namespace net
