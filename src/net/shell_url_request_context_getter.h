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

#ifndef CONTENT_NW_SRC_NET_SHELL_URL_REQUEST_CONTEXT_GETTER_H_
#define CONTENT_NW_SRC_NET_SHELL_URL_REQUEST_CONTEXT_GETTER_H_

#include "base/compiler_specific.h"
#include "base/files/file_path.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "content/public/browser/content_browser_client.h"
#include "net/url_request/url_request_context_getter.h"
#include "net/url_request/url_request_job_factory.h"


namespace net {
class HostResolver;
class NetworkDelegate;
class ProxyConfigService;
class URLRequestContextStorage;
}

namespace base{
class MessageLoop;
}

namespace content {

class ShellURLRequestContextGetter : public net::URLRequestContextGetter {
 public:
  ShellURLRequestContextGetter(
      bool ignore_certificate_errors,
      const base::FilePath& base_path,
      base::MessageLoop* io_loop,
      base::MessageLoop* file_loop,
      ProtocolHandlerMap* protocol_handlers);

  // net::URLRequestContextGetter implementation.
  virtual net::URLRequestContext* GetURLRequestContext() OVERRIDE;
  virtual scoped_refptr<base::SingleThreadTaskRunner>
      GetNetworkTaskRunner() const OVERRIDE;

  net::HostResolver* host_resolver();

 protected:
  virtual ~ShellURLRequestContextGetter();

 private:
  bool ignore_certificate_errors_;
  base::FilePath base_path_;
  base::MessageLoop* io_loop_;
  base::MessageLoop* file_loop_;

  scoped_ptr<net::ProxyConfigService> proxy_config_service_;
  scoped_ptr<net::NetworkDelegate> network_delegate_;
  scoped_ptr<net::URLRequestContextStorage> storage_;
  scoped_ptr<net::URLRequestContext> url_request_context_;
  ProtocolHandlerMap protocol_handlers_;

  DISALLOW_COPY_AND_ASSIGN(ShellURLRequestContextGetter);
};

}  // namespace content

#endif  // CONTENT_NW_SRC_NET_SHELL_URL_REQUEST_CONTEXT_GETTER_H_
