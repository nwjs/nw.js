// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/file_protocol.h"

#include <algorithm>
#include <string>
#include <vector>

#include "base/base64.h"
#include "base/compiler_specific.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/format_macros.h"
#include "base/logging.h"
#include "base/memory/weak_ptr.h"
#include "base/message_loop/message_loop.h"
#include "base/metrics/field_trial.h"
#include "base/metrics/histogram.h"
#include "base/metrics/sparse_histogram.h"
#include "base/sha1.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/sequenced_worker_pool.h"
#include "base/threading/thread_restrictions.h"
#include "build/build_config.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/resource_request_info.h"
#include "crypto/secure_hash.h"
#include "crypto/sha2.h"
#include "content/nw/src/browser/content_verifier.h"
#include "net/base/io_buffer.h"
#include "net/base/net_errors.h"
#include "net/base/filename_util.h"
#include "net/http/http_request_headers.h"
#include "net/http/http_response_headers.h"
#include "net/http/http_response_info.h"
#include "net/url_request/url_request_error_job.h"
#include "net/url_request/url_request_file_dir_job.h"
#include "net/url_request/url_request_simple_job.h"
#include "url/url_util.h"

#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_shell.h"
#include "content/nw/src/shell_browser_main_parts.h"
#include "content/nw/src/shell_content_browser_client.h"

using content::BrowserThread;
using content::ResourceRequestInfo;
using content::ResourceType;

namespace nw {
namespace {

base::Time GetFileLastModifiedTime(const base::FilePath& filename) {
  if (base::PathExists(filename)) {
    base::File::Info info;
    if (base::GetFileInfo(filename, &info))
      return info.last_modified;
  }
  return base::Time();
}

base::Time GetFileCreationTime(const base::FilePath& filename) {
  if (base::PathExists(filename)) {
    base::File::Info info;
    if (base::GetFileInfo(filename, &info))
      return info.creation_time;
  }
  return base::Time();
}

void ReadResourceFilePathAndLastModifiedTime(
    const base::FilePath& directory,
    const base::FilePath& relative_path,
    base::FilePath* file_path,
    base::Time* last_modified_time) {
  *file_path = directory.Append(relative_path);
  *last_modified_time = GetFileLastModifiedTime(*file_path);
  // While we're here, log the delta between extension directory
  // creation time and the resource's last modification time.
  base::ElapsedTimer query_timer;
  base::Time dir_creation_time = GetFileCreationTime(directory);
  UMA_HISTOGRAM_TIMES("Extensions.ResourceDirectoryTimestampQueryLatency",
                      query_timer.Elapsed());
  int64 delta_seconds = (*last_modified_time - dir_creation_time).InSeconds();
  if (delta_seconds >= 0) {
    UMA_HISTOGRAM_CUSTOM_COUNTS("Extensions.ResourceLastModifiedDelta",
                                delta_seconds,
                                0,
                                base::TimeDelta::FromDays(30).InSeconds(),
                                50);
  } else {
    UMA_HISTOGRAM_CUSTOM_COUNTS("Extensions.ResourceLastModifiedNegativeDelta",
                                -delta_seconds,
                                1,
                                base::TimeDelta::FromDays(30).InSeconds(),
                                50);
  }
}

}  // namespace

URLRequestNWFileJob::URLRequestNWFileJob(net::URLRequest* request,
                      net::NetworkDelegate* network_delegate,
                      const base::FilePath& directory_path,
                      const base::FilePath& relative_path,
                      ContentVerifyJob* verify_job)
      : net::URLRequestFileJob(
            request,
            network_delegate,
            base::FilePath(),
            BrowserThread::GetBlockingPool()->GetTaskRunnerWithShutdownBehavior(
                base::SequencedWorkerPool::SKIP_ON_SHUTDOWN)),
        verify_job_(verify_job),
        seek_position_(0),
        bytes_read_(0),
        directory_path_(directory_path),
        relative_path_(relative_path),
        // TODO(tc): Move all of these files into resources.pak so we don't
        // break when updating on Linux.
        can_start_(false),
        started_(false),
        weak_factory_(this) {
}

void URLRequestNWFileJob::GetResponseInfo(net::HttpResponseInfo* info) {
    *info = response_info_;
}

void URLRequestNWFileJob::Start() {
    request_timer_.reset(new base::ElapsedTimer());
    base::FilePath* read_file_path = new base::FilePath;
    base::Time* last_modified_time = new base::Time();
    bool posted = BrowserThread::PostBlockingPoolTaskAndReply(
        FROM_HERE,
        base::Bind(&ReadResourceFilePathAndLastModifiedTime,
                   directory_path_,
                   relative_path_,
                   base::Unretained(read_file_path),
                   base::Unretained(last_modified_time)),
        base::Bind(&URLRequestNWFileJob::OnFilePathAndLastModifiedTimeRead,
                   weak_factory_.GetWeakPtr(),
                   base::Owned(read_file_path),
                   base::Owned(last_modified_time)));
    DCHECK(posted);
}

bool URLRequestNWFileJob::IsRedirectResponse(GURL* location, int* http_status_code) {
    return false;
}

void URLRequestNWFileJob::SetExtraRequestHeaders(const net::HttpRequestHeaders& headers) {
    // TODO(asargent) - we'll need to add proper support for range headers.
    // crbug.com/369895.
    std::string range_header;
    if (headers.GetHeader(net::HttpRequestHeaders::kRange, &range_header)) {
      if (verify_job_.get())
        verify_job_ = NULL;
    }
    URLRequestFileJob::SetExtraRequestHeaders(headers);
}

void URLRequestNWFileJob::OnSeekComplete(int64 result) {
    DCHECK_EQ(seek_position_, 0);
    seek_position_ = result;
    // TODO(asargent) - we'll need to add proper support for range headers.
    // crbug.com/369895.
    if (result > 0 && verify_job_.get())
      verify_job_ = NULL;
}

void URLRequestNWFileJob::OnReadComplete(net::IOBuffer* buffer, int result) {
    if (result >= 0)
      UMA_HISTOGRAM_COUNTS("ExtensionUrlRequest.OnReadCompleteResult", result);
    else
      UMA_HISTOGRAM_SPARSE_SLOWLY("ExtensionUrlRequest.OnReadCompleteError",
                                  -result);
    if (result > 0) {
      bytes_read_ += result;
#if 0
      if (verify_job_.get()) {
        verify_job_->BytesRead(result, buffer->data());
        if (!remaining_bytes())
          verify_job_->DoneReading();
      }
#endif
    }
}

void URLRequestNWFileJob::CanStart() {
    can_start_ = true;
    if (!started_) {
      started_ = true;
      URLRequestFileJob::Start();
    }
}


URLRequestNWFileJob::~URLRequestNWFileJob()  {
    UMA_HISTOGRAM_COUNTS("ExtensionUrlRequest.TotalKbRead", bytes_read_ / 1024);
    UMA_HISTOGRAM_COUNTS("ExtensionUrlRequest.SeekPosition", seek_position_);
    if (request_timer_.get())
      UMA_HISTOGRAM_TIMES("ExtensionUrlRequest.Latency",
                          request_timer_->Elapsed());
}

void URLRequestNWFileJob::OnFilePathAndLastModifiedTimeRead(base::FilePath* read_file_path,
                                                            base::Time* last_modified_time) {
    file_path_ = *read_file_path;
#if 0
    response_info_.headers = BuildHttpHeaders(
        content_security_policy_,
        send_cors_header_,
        *last_modified_time);
#endif
    if (can_start_) {
      started_ = true;
      URLRequestFileJob::Start();
    }
}

class NWFileProtocoHandler
    : public net::URLRequestJobFactory::ProtocolHandler {
 public:
  NWFileProtocoHandler(bool is_incognito)
    : is_incognito_(is_incognito) {}

  ~NWFileProtocoHandler() override {}

  net::URLRequestJob* MaybeCreateJob(
      net::URLRequest* request,
      net::NetworkDelegate* network_delegate) const override;

 private:
  const bool is_incognito_;
  DISALLOW_COPY_AND_ASSIGN(NWFileProtocoHandler);
};

net::URLRequestJob*
NWFileProtocoHandler::MaybeCreateJob(
    net::URLRequest* request, net::NetworkDelegate* network_delegate) const {

  base::FilePath file_path;
  const bool is_file = net::FileURLToFilePath(request->url(), &file_path);

  if (!network_delegate ||
      !network_delegate->CanAccessFile(*request, file_path)) {
    return new net::URLRequestErrorJob(request, network_delegate, net::ERR_ACCESS_DENIED);
  }

  if (is_file &&
      file_path.EndsWithSeparator() &&
      file_path.IsAbsolute()) {
    return new net::URLRequestFileDirJob(request, network_delegate, file_path);
  }
  base::ThreadRestrictions::ScopedAllowIO allow_io;
  base::FilePath directory_path = content::Shell::GetPackage()->path();
  base::File::Info info;
  base::GetFileInfo(directory_path, &info);
  if (!info.is_directory)
    directory_path = directory_path.DirName();

  if (!directory_path.IsParent(file_path)) {
    return new net::URLRequestFileJob(request, network_delegate, file_path,
                                 BrowserThread::GetBlockingPool()->GetTaskRunnerWithShutdownBehavior(
                                                    base::SequencedWorkerPool::SKIP_ON_SHUTDOWN));
  }
  base::FilePath relative_path;
  directory_path.AppendRelativePath(file_path, &relative_path);

  ContentVerifyJob* verify_job = NULL;
  content::ShellContentBrowserClient* browser_client = 
      static_cast<content::ShellContentBrowserClient*>(
          content::GetContentClient()->browser());
  ContentVerifier* verifier =
    browser_client->shell_browser_main_parts()->content_verifier();
  if (verifier) {
    verify_job =
        verifier->CreateJobFor("", directory_path, relative_path);
  }

  URLRequestNWFileJob* job = new URLRequestNWFileJob(request,
                                 network_delegate,
                                 directory_path,
                                 relative_path,
                                 verify_job);
  if (verify_job) {
    verify_job->SetSuccessCallback(base::Bind(&URLRequestNWFileJob::CanStart, base::Unretained(job)));
    verify_job->Start();
  } else {
    job->set_can_start(true);
  }
  return job;
}


net::HttpResponseHeaders* BuildHttpHeaders(
    const std::string& content_security_policy,
    bool send_cors_header,
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
    std::string hash =
        base::StringPrintf("%" PRId64, last_modified_time.ToInternalValue());
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

net::URLRequestJobFactory::ProtocolHandler* CreateNWFileProtocolHandler(bool is_incognito) {
  return new NWFileProtocoHandler(is_incognito);
}

}  // namespace nw
