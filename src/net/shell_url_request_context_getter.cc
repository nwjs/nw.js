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

#include "content/nw/src/net/shell_url_request_context_getter.h"

#include "base/logging.h"
#include "base/string_split.h"
#include "base/string_util.h"
#include "base/threading/worker_pool.h"
#include "content/public/browser/browser_thread.h"
#include "content/nw/src/net/shell_network_delegate.h"
#include "content/nw/src/net/sqlite_persistent_cookie_store.h"
#include "content/nw/src/nw_protocol_handler.h"
#include "content/nw/src/nw_shell.h"
#include "net/base/cert_verifier.h"
#include "net/base/default_server_bound_cert_store.h"
#include "net/base/host_resolver.h"
#include "net/base/mapped_host_resolver.h"
#include "net/base/server_bound_cert_service.h"
#include "net/base/ssl_config_service_defaults.h"
#include "net/cookies/cookie_monster.h"
#include "net/http/http_auth_handler_factory.h"
#include "net/http/http_cache.h"
#include "net/http/http_network_session.h"
#include "net/http/http_server_properties_impl.h"
#include "net/proxy/dhcp_proxy_script_fetcher_factory.h"
#include "net/proxy/proxy_config_service.h"
#include "net/proxy/proxy_script_fetcher_impl.h"
#include "net/proxy/proxy_service.h"
#include "net/proxy/proxy_service_v8.h"
#include "net/url_request/static_http_user_agent_settings.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_storage.h"
#include "net/url_request/url_request_job_factory_impl.h"

namespace content {

ShellURLRequestContextGetter::ShellURLRequestContextGetter(
    const FilePath& base_path,
    MessageLoop* io_loop,
    MessageLoop* file_loop)
    : ignore_certificate_errors_(true),
      base_path_(base_path),
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

    FilePath cookie_path = base_path_.Append(FILE_PATH_LITERAL("cookies"));
    scoped_refptr<SQLitePersistentCookieStore> cookie_db =
        new SQLitePersistentCookieStore(cookie_path, true, NULL);
    net::CookieMonster* cookie_store = new net::CookieMonster(cookie_db.get(), NULL);
    cookie_store->GetCookieMonster()->SetPersistSessionCookies(true);
    storage_->set_cookie_store(cookie_store);

    storage_->set_server_bound_cert_service(new net::ServerBoundCertService(
        new net::DefaultServerBoundCertStore(NULL),
        base::WorkerPool::GetTaskRunner(true)));
    storage_->set_http_user_agent_settings(
        new net::StaticHttpUserAgentSettings(
            "en-us,en", "iso-8859-1,*,utf-8", EmptyString()));

    scoped_ptr<net::HostResolver> host_resolver(
        net::HostResolver::CreateDefaultResolver(NULL));

    storage_->set_cert_verifier(net::CertVerifier::CreateDefault());

    net::ProxyService* proxy_service;
    net::DhcpProxyScriptFetcherFactory dhcp_factory;

    proxy_service = net::CreateProxyServiceUsingV8ProxyResolver(
        proxy_config_service_.release(),
        0,
        new net::ProxyScriptFetcherImpl(url_request_context_.get()),
        dhcp_factory.Create(url_request_context_.get()),
        host_resolver.get(),
        NULL,
        url_request_context_->network_delegate());
    storage_->set_proxy_service(proxy_service);

    storage_->set_ssl_config_service(new net::SSLConfigServiceDefaults);
    storage_->set_http_auth_handler_factory(
        net::HttpAuthHandlerFactory::CreateDefault(host_resolver.get()));
    storage_->set_http_server_properties(new net::HttpServerPropertiesImpl);

    FilePath cache_path = base_path_.Append(FILE_PATH_LITERAL("Cache"));
    net::HttpCache::DefaultBackend* main_backend =
        new net::HttpCache::DefaultBackend(
            net::DISK_CACHE,
            cache_path,
            0,
            BrowserThread::GetMessageLoopProxyForThread(
                BrowserThread::CACHE));

    net::HttpNetworkSession::Params network_session_params;
    network_session_params.cert_verifier =
        url_request_context_->cert_verifier();
    network_session_params.server_bound_cert_service =
        url_request_context_->server_bound_cert_service();
    network_session_params.proxy_service =
        url_request_context_->proxy_service();
    network_session_params.ssl_config_service =
        url_request_context_->ssl_config_service();
    network_session_params.http_auth_handler_factory =
        url_request_context_->http_auth_handler_factory();
    network_session_params.network_delegate =
        url_request_context_->network_delegate();
    network_session_params.http_server_properties =
        url_request_context_->http_server_properties();
    network_session_params.ignore_certificate_errors =
        ignore_certificate_errors_;

    // Give |storage_| ownership at the end in case it's |mapped_host_resolver|.
    storage_->set_host_resolver(host_resolver.Pass());
    network_session_params.host_resolver =
        url_request_context_->host_resolver();

    net::HttpCache* main_cache = new net::HttpCache(
        network_session_params, main_backend);
    storage_->set_http_transaction_factory(main_cache);

    net::URLRequestJobFactoryImpl* job_factory = 
      new net::URLRequestJobFactoryImpl();
    job_factory->SetProtocolHandler("nw", new nw::NwProtocolHandler());
    storage_->set_job_factory(job_factory);
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
