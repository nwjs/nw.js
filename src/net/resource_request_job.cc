// Copyright (c) 2012 Intel Corp
// Copyright (c) 2012 The Chromium Authors
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell co
// pies of the Software, and to permit persons to whom the Software is furnished
//  to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in al
// l copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IM
// PLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNES
// S FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
//  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WH
// ETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "content/nw/src/net/resource_request_job.h"

#include "base/bind.h"
#include "base/compiler_specific.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/message_loop.h"
#include "base/logging.h"
#include "googleurl/src/gurl.h"
#include "net/base/io_buffer.h"
#include "net/base/net_errors.h"
#include "net/http/http_response_headers.h"
#include "net/url_request/url_request.h"
#include "net/url_request/url_request_job.h"
#include "grit/nw_resources.h"
#include "ui/base/resource/resource_bundle.h"

namespace nw {

ResourceRequestJob::ResourceRequestJob(
    net::URLRequest* request,
    net::NetworkDelegate* network_delegate,
    std::string mime_type,
    int resource_id)
    : net::URLRequestJob(request, network_delegate),
      data_offset_(0),
      pending_buf_size_(0),
      mime_type_(mime_type),
      resource_id_(resource_id),
      weak_factory_(this) {
}

// static
ResourceRequestJob* ResourceRequestJob::Factory(
    net::URLRequest* request,
    net::NetworkDelegate* network_delegate) {
  std::string url = request->url().spec();
  if (url == "nw:version") {
    return new ResourceRequestJob(request,
        network_delegate, "text/html", IDR_NW_VERSION);
  } else if (url == "nw:gpu") {
    return new ResourceRequestJob(request,
        network_delegate, "text/html", IDR_NW_GPU);
  }

  return new ResourceRequestJob(request,
        network_delegate, "text/html", IDR_NW_BLANK);
}

ResourceRequestJob::~ResourceRequestJob() {
}

void ResourceRequestJob::Start() {
  // Start reading asynchronously so that all error reporting and data
  // callbacks happen as they would for network requests.
  MessageLoop::current()->PostTask(
      FROM_HERE,
      base::Bind(&ResourceRequestJob::StartAsync, weak_factory_.GetWeakPtr()));
}

void ResourceRequestJob::DataAvailable(base::RefCountedMemory* bytes) {
  if (bytes) {
    // The request completed, and we have all the data.
    // Clear any IO pending status.
    SetStatus(net::URLRequestStatus());

    data_ = bytes;
    int bytes_read;
    if (pending_buf_.get()) {
      CHECK(pending_buf_->data());
      CompleteRead(pending_buf_, pending_buf_size_, &bytes_read);
      pending_buf_ = NULL;
      NotifyReadComplete(bytes_read);
    }
  } else {
    // The request failed.
    NotifyDone(net::URLRequestStatus(net::URLRequestStatus::FAILED,
                                     net::ERR_FAILED));
  }
}

bool ResourceRequestJob::ReadRawData(net::IOBuffer* buf,
                                     int buf_size, int* bytes_read) {
  if (!data_.get()) {
    SetStatus(net::URLRequestStatus(net::URLRequestStatus::IO_PENDING, 0));
    DCHECK(!pending_buf_.get());
    CHECK(buf->data());
    pending_buf_ = buf;
    pending_buf_size_ = buf_size;
    return false;  // Tell the caller we're still waiting for data.
  }

  // Otherwise, the data is available.
  CompleteRead(buf, buf_size, bytes_read);
  return true;
}

void ResourceRequestJob::CompleteRead(net::IOBuffer* buf, int buf_size,
                                      int* bytes_read) {
  int remaining = static_cast<int>(data_->size()) - data_offset_;
  if (buf_size > remaining)
    buf_size = remaining;
  if (buf_size > 0) {
    memcpy(buf->data(), data_->front() + data_offset_, buf_size);
    data_offset_ += buf_size;
  }
  *bytes_read = buf_size;
}

bool ResourceRequestJob::GetMimeType(std::string* mime_type) const {
  *mime_type = "text/html";
  return true;
}


void ResourceRequestJob::GetResponseInfo(net::HttpResponseInfo* info) {
  DCHECK(!info->headers);
  info->headers = new net::HttpResponseHeaders("HTTP/1.1 200 OK");
  info->headers->AddHeader("Cache-Control: no-cache");
}

void ResourceRequestJob::StartAsync() {
  if (!request_)
    return;

  NotifyHeadersComplete();
  DataAvailable(ResourceBundle::GetSharedInstance().LoadDataResourceBytes(
      resource_id_));
}

}  // namespace nw

