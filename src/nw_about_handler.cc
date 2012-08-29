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

#include "content/nw/src/nw_about_handler.h"

#include "base/bind.h"
#include "base/compiler_specific.h"
#include "base/memory/weak_ptr.h"
#include "base/message_loop.h"
#include "base/logging.h"
#include "googleurl/src/gurl.h"
#include "net/url_request/url_request.h"
#include "net/url_request/url_request_job.h"

namespace {
  
class NET_EXPORT URLRequestAboutJob : public net::URLRequestJob {
 public:
  URLRequestAboutJob(net::URLRequest* request,
                     net::NetworkDelegate* network_delegate)
      : net::URLRequestJob(request, network_delegate),
        ALLOW_THIS_IN_INITIALIZER_LIST(weak_factory_(this)) {
  }

  static net::URLRequest::ProtocolFactory Factory;

  void Start() {
    // Start reading asynchronously so that all error reporting and data
    // callbacks happen as they would for network requests.
    MessageLoop::current()->PostTask(
        FROM_HERE,
        base::Bind(&URLRequestAboutJob::StartAsync, weak_factory_.GetWeakPtr()));
  }

  bool GetMimeType(std::string* mime_type) const {
    *mime_type = "text/html";
    return true;
  }

 private:
  virtual ~URLRequestAboutJob() {
  }

  void StartAsync() {
    NotifyHeadersComplete();
  }

  base::WeakPtrFactory<URLRequestAboutJob> weak_factory_;
};

}  // namespace

namespace nw {

AboutProtocolHandler::AboutProtocolHandler() {
}

net::URLRequestJob* AboutProtocolHandler::MaybeCreateJob(
    net::URLRequest* request, net::NetworkDelegate* network_delegate) const {
  return new URLRequestAboutJob(request, network_delegate);
}

}  // namespace nw
