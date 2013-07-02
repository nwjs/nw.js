#ifndef CONTENT_NW_SRC_NW_EMBED_HANDLER_H_
#define CONTENT_NW_SRC_NW_EMBED_HANDLER_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "net/url_request/url_request_job_factory.h"

class GURL;

namespace net {
  class URLRequestJob;
}

namespace nw {
  
  class EmbedProtocolHandler :
  public net::URLRequestJobFactory::ProtocolHandler {
  public:
    EmbedProtocolHandler();
    virtual net::URLRequestJob* MaybeCreateJob(
        net::URLRequest* request,
        net::NetworkDelegate* network_delegate) const OVERRIDE;
    
  private:
    DISALLOW_COPY_AND_ASSIGN(EmbedProtocolHandler);
  };
  
}
#endif