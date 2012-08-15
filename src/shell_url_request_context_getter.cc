// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/shell_url_request_context_getter.h"

#include "base/logging.h"
#include "base/string_split.h"
#include "base/threading/worker_pool.h"
#include "content/public/browser/browser_thread.h"
#include "content/shell/shell_network_delegate.h"
#include "net/base/cert_verifier.h"
#include "net/base/default_server_bound_cert_store.h"
#include "net/base/host_resolver.h"
#include "net/base/server_bound_cert_service.h"
#include "net/base/ssl_config_service_defaults.h"
#include "net/cookies/cookie_monster.h"
#include "net/http/http_auth_handler_factory.h"
#include "net/http/http_cache.h"
#include "net/http/http_server_properties_impl.h"
#include "net/proxy/proxy_service.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_storage.h"
#include "net/url_request/url_request_job_factory.h"

namespace content {

ShellURLRequestContextGetter::ShellURLRequestContextGetter(
    const FilePath& base_path,
    MessageLoop* io_loop,
    MessageLoop* file_loop)
    : base_path_(base_path),
      io_loop_(io_loop),
      file_loop_(file_loop) {
  // Must first be created on the UI thread.
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  // We must create the proxy config service on the UI loop on Linux because it
  // must synchronously run on the glib message loop. This will be passed to
  // the URLRequestContextStorage on the IO thread in GetURLRequestContext().
  proxy_config_service_.reset(
      net::ProxyService::CreateSystemProxyConfigService(
          io_loop_->message_loop_proxy(), file_loop_));
}

ShellURLRequestContextGetter::~ShellURLRequestContextGetter() {
}

net::URLRequestContext* ShellURLRequestContextGetter::GetURLRequestContext() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));

  if (!url_request_context_.get()) {
    url_request_context_.reset(new net::URLRequestContext());
    network_delegate_.reset(new ShellNetworkDelegate);
    url_request_context_->set_network_delegate(network_delegate_.get());
    storage_.reset(
        new net::URLRequestContextStorage(url_request_context_.get()));
    storage_->set_cookie_store(new net::CookieMonster(NULL, NULL));
    storage_->set_server_bound_cert_service(new net::ServerBoundCertService(
        new net::DefaultServerBoundCertStore(NULL),
        base::WorkerPool::GetTaskRunner(true)));
    url_request_context_->set_accept_language("en-us,en");
    url_request_context_->set_accept_charset("iso-8859-1,*,utf-8");

    storage_->set_host_resolver(
        net::CreateSystemHostResolver(net::HostResolver::kDefaultParallelism,
                                      net::HostResolver::kDefaultRetryAttempts,
                                      NULL));
    storage_->set_cert_verifier(net::CertVerifier::CreateDefault());
    // TODO(jam): use v8 if possible, look at chrome code.
    storage_->set_proxy_service(
        net::ProxyService::CreateUsingSystemProxyResolver(
        proxy_config_service_.release(),
        0,
        NULL));
    storage_->set_ssl_config_service(new net::SSLConfigServiceDefaults);
    storage_->set_http_auth_handler_factory(
        net::HttpAuthHandlerFactory::CreateDefault(
            url_request_context_->host_resolver()));
    storage_->set_http_server_properties(new net::HttpServerPropertiesImpl);

    FilePath cache_path = base_path_.Append(FILE_PATH_LITERAL("Cache"));
    net::HttpCache::DefaultBackend* main_backend =
        new net::HttpCache::DefaultBackend(
            net::DISK_CACHE,
            cache_path,
            0,
            BrowserThread::GetMessageLoopProxyForThread(
                BrowserThread::CACHE));

    net::HttpCache* main_cache = new net::HttpCache(
        url_request_context_->host_resolver(),
        url_request_context_->cert_verifier(),
        url_request_context_->server_bound_cert_service(),
        NULL, /* transport_security_state */
        url_request_context_->proxy_service(),
        "", /* ssl_session_cache_shard */
        url_request_context_->ssl_config_service(),
        url_request_context_->http_auth_handler_factory(),
        url_request_context_->network_delegate(),
        url_request_context_->http_server_properties(),
        NULL,
        main_backend,
        "" /* trusted_spdy_proxy */ );
    storage_->set_http_transaction_factory(main_cache);

    storage_->set_job_factory(new net::URLRequestJobFactory);
  }

  return url_request_context_.get();
}

scoped_refptr<base::SingleThreadTaskRunner>
    ShellURLRequestContextGetter::GetNetworkTaskRunner() const {
  return BrowserThread::GetMessageLoopProxyForThread(BrowserThread::IO);
}

net::HostResolver* ShellURLRequestContextGetter::host_resolver() {
  return url_request_context_->host_resolver();
}

}  // namespace content
