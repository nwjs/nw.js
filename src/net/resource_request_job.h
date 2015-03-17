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

#ifndef CONTENT_NW_SRC_NET_RESOURCE_REQUEST_JOB_H_
#define CONTENT_NW_SRC_NET_RESOURCE_REQUEST_JOB_H_

#include "base/memory/ref_counted_memory.h"
#include "net/url_request/url_request_job.h"

namespace net {
class HttpResponseHeaders;
class URLRequest;
}

namespace nw {

class ResourceRequestJob : public net::URLRequestJob {
 public:
  ResourceRequestJob(net::URLRequest* request,
                     net::NetworkDelegate* network_delegate,
                     std::string mime_type,
                     int resource_id);

  // net::URLRequestJob methods.
   void Start() override;
   bool ReadRawData(net::IOBuffer* dest, int dest_size, int* bytes_read)
      override;
   bool GetMimeType(std::string* mime_type) const override;
   void GetResponseInfo(net::HttpResponseInfo* info) override;

  static ResourceRequestJob* Factory(net::URLRequest* request,
                                     net::NetworkDelegate* network_delegate);

  // Called by ChromeURLDataManager to notify us that the data blob is ready
  // for us.
  void DataAvailable(base::RefCountedMemory* bytes);

 private:
   ~ResourceRequestJob() override;

  // Helper for Start(), to let us start asynchronously.
  // (This pattern is shared by most net::URLRequestJob implementations.)
  void StartAsync();

  // Do the actual copy from data_ (the data we're serving) into |buf|.
  // Separate from ReadRawData so we can handle async I/O.
  void CompleteRead(net::IOBuffer* buf, int buf_size, int* bytes_read);

  // The actual data we're serving.  NULL until it's been fetched.
  scoped_refptr<base::RefCountedMemory> data_;
  // The current offset into the data that we're handing off to our
  // callers via the Read interfaces.
  int data_offset_;

  // For async reads, we keep around a pointer to the buffer that
  // we're reading into.
  scoped_refptr<net::IOBuffer> pending_buf_;
  int pending_buf_size_;
  std::string mime_type_;

  int resource_id_;
  base::WeakPtrFactory<ResourceRequestJob> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(ResourceRequestJob);
};

}  // namespace nw

#endif  // CONTENT_NW_SRC_NET_RESOURCE_REQUEST_JOB_H_

