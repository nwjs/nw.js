
#include "content/nw/src/embed_protocol_handler.h"

#include "content/nw/src/net/embed_request_job.h"
#include "net/url_request/url_request.h"

#include "embed_protocol_handler.h"

namespace nw {
  EmbedProtocolHandler::EmbedProtocolHandler() {
  }
  
  net::URLRequestJob* EmbedProtocolHandler::MaybeCreateJob(
   net::URLRequest* request, net::NetworkDelegate* network_delegate) const {
    return EmbedRequestJob::Factory(request, network_delegate);
  }
}