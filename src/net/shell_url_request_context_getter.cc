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
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "base/threading/sequenced_worker_pool.h"
#include "base/threading/worker_pool.h"
#include "chrome/browser/chrome_notification_types.h"
#include "chrome/browser/net/chrome_cookie_notification_details.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/notification_service.h"
#include "content/public/common/url_constants.h"
#include "content/nw/src/net/shell_network_delegate.h"
#include "content/public/browser/cookie_store_factory.h"
#include "content/nw/src/net/app_protocol_handler.h"
#include "content/nw/src/nw_protocol_handler.h"
#include "content/nw/src/nw_shell.h"
#include "content/nw/src/shell_content_browser_client.h"
#include "extensions/browser/info_map.h"
#include "net/cert/cert_verifier.h"
#include "net/cert/cert_verify_proc.h"
#include "net/cert/multi_threaded_cert_verifier.h"
#include "net/dns/host_resolver.h"
#include "net/dns/mapped_host_resolver.h"
#include "net/ssl/ssl_config_service_defaults.h"
#include "net/cookies/cookie_monster.h"
#include "net/http/http_auth_filter.h"
#include "net/http/http_auth_handler_factory.h"
#include "net/http/http_cache.h"
#include "net/http/http_network_session.h"
#include "net/http/http_server_properties_impl.h"
#include "net/proxy/dhcp_proxy_script_fetcher_factory.h"
#include "net/proxy/proxy_config_service.h"
#include "net/proxy/proxy_script_fetcher_impl.h"
#include "net/proxy/proxy_service.h"
#include "net/proxy/proxy_service_v8.h"
#include "net/ssl/channel_id_service.h"
#include "net/ssl/default_channel_id_store.h"
#include "net/url_request/file_protocol_handler.h"
#include "net/url_request/static_http_user_agent_settings.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_storage.h"
#include "net/url_request/url_request_job_factory_impl.h"
#include "ui/base/l10n/l10n_util.h"

using base::MessageLoop;

namespace content {

namespace {

void InstallProtocolHandlers(net::URLRequestJobFactoryImpl* job_factory,
                             ProtocolHandlerMap* protocol_handlers) {
  for (ProtocolHandlerMap::iterator it =
           protocol_handlers->begin();
       it != protocol_handlers->end();
       ++it) {
    bool set_protocol = job_factory->SetProtocolHandler(
        it->first, it->second.release());
    DCHECK(set_protocol);
  }
  protocol_handlers->clear();
}

// ----------------------------------------------------------------------------
// CookieMonster::Delegate implementation
// ----------------------------------------------------------------------------
class NWCookieMonsterDelegate : public net::CookieMonster::Delegate {
 public:
  explicit NWCookieMonsterDelegate(ShellBrowserContext* browser_context)
    : browser_context_(browser_context) {
  }

  // net::CookieMonster::Delegate implementation.
  void OnCookieChanged(
      const net::CanonicalCookie& cookie,
      bool removed,
      net::CookieMonster::Delegate::ChangeCause cause) override {
    BrowserThread::PostTask(
        BrowserThread::UI, FROM_HERE,
        base::Bind(&NWCookieMonsterDelegate::OnCookieChangedAsyncHelper,
                   this, cookie, removed, cause));
  }
  void OnLoaded() override {
  }

 private:
  ~NWCookieMonsterDelegate() final {}

  void OnCookieChangedAsyncHelper(
      const net::CanonicalCookie& cookie,
      bool removed,
      net::CookieMonster::Delegate::ChangeCause cause) {
    ChromeCookieDetails cookie_details(&cookie, removed, cause);
      content::NotificationService::current()->Notify(
          chrome::NOTIFICATION_COOKIE_CHANGED,
          content::Source<ShellBrowserContext>(browser_context_),
          content::Details<ChromeCookieDetails>(&cookie_details));
  }

  ShellBrowserContext* browser_context_;
};

}  // namespace


ShellURLRequestContextGetter::ShellURLRequestContextGetter(
    bool ignore_certificate_errors,
    const FilePath& data_path,
    const FilePath& root_path,
    MessageLoop* io_loop,
    MessageLoop* file_loop,
    ProtocolHandlerMap* protocol_handlers,
    ShellBrowserContext* browser_context,
    const std::string& auth_schemes,
    const std::string& auth_server_whitelist,
    const std::string& auth_delegate_whitelist,
    const std::string& gssapi_library_name,
    extensions::InfoMap* extension_info_map)
    : ignore_certificate_errors_(ignore_certificate_errors),
      data_path_(data_path),
      root_path_(root_path),
      auth_schemes_(auth_schemes),
      negotiate_disable_cname_lookup_(false),
      negotiate_enable_port_(false),
      auth_server_whitelist_(auth_server_whitelist),
      auth_delegate_whitelist_(auth_delegate_whitelist),
      gssapi_library_name_(gssapi_library_name),
      io_loop_(io_loop),
      file_loop_(file_loop),
      browser_context_(browser_context),
      extension_info_map_(extension_info_map){
  // Must first be created on the UI thread.
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  std::swap(protocol_handlers_, *protocol_handlers);

  // We must create the proxy config service on the UI loop on Linux because it
  // must synchronously run on the glib message loop. This will be passed to
  // the URLRequestContextStorage on the IO thread in GetURLRequestContext().
  proxy_config_service_.reset(
      net::ProxyService::CreateSystemProxyConfigService(
                                                        io_loop_->message_loop_proxy(), file_loop_->message_loop_proxy()));
}

ShellURLRequestContextGetter::~ShellURLRequestContextGetter() {
}

net::URLRequestContext* ShellURLRequestContextGetter::GetURLRequestContext() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));

  if (!url_request_context_.get()) {
    ShellContentBrowserClient* browser_client =
      static_cast<ShellContentBrowserClient*>(
          GetContentClient()->browser());

  url_request_context_.reset(new net::URLRequestContext());
  network_delegate_.reset(new ShellNetworkDelegate(browser_context_, extension_info_map_));
  url_request_context_->set_network_delegate(network_delegate_.get());
  storage_.reset(
        new net::URLRequestContextStorage(url_request_context_.get()));

    FilePath cookie_path = data_path_.Append(FILE_PATH_LITERAL("cookies"));
    scoped_refptr<net::CookieStore> cookie_store = NULL;

    content::CookieStoreConfig cookie_config(
                                             cookie_path, content::CookieStoreConfig::PERSISTANT_SESSION_COOKIES,
                                             NULL, new NWCookieMonsterDelegate(browser_context_));
    cookie_store = content::CreateCookieStore(cookie_config);
    cookie_store->GetCookieMonster()->SetPersistSessionCookies(true);
    storage_->set_cookie_store(cookie_store.get());

    const char* schemes[] = {"http", "https", "ws", "wss", "app", "file"};
    cookie_store->GetCookieMonster()->SetCookieableSchemes(schemes, 6);

    storage_->set_channel_id_service(new net::ChannelIDService(
        new net::DefaultChannelIDStore(NULL),
        base::WorkerPool::GetTaskRunner(true)));

    std::string accept_lang = browser_client->GetApplicationLocale();
    if (accept_lang.empty())
      accept_lang = "en-us,en";
    else
      accept_lang.append(",en-us,en");
    storage_->set_http_user_agent_settings(
         new net::StaticHttpUserAgentSettings(
                net::HttpUtil::GenerateAcceptLanguageHeader(accept_lang),
                base::EmptyString()));

    scoped_ptr<net::HostResolver> host_resolver(
        net::HostResolver::CreateDefaultResolver(NULL));

    net::CertVerifyProc *verify_proc = net::CertVerifyProc::CreateDefault();
    if (!verify_proc->SupportsAdditionalTrustAnchors()) {
      LOG(WARNING)
        << "Additional trust anchors not supported on the current platform!";
    }
    net::MultiThreadedCertVerifier *verifier = new net::MultiThreadedCertVerifier(verify_proc);
    verifier->SetCertTrustAnchorProvider(this);
    storage_->set_cert_verifier(verifier);
    storage_->set_transport_security_state(new net::TransportSecurityState);

    net::ProxyService* proxy_service;
    net::DhcpProxyScriptFetcherFactory dhcp_factory;

    proxy_service = net::CreateProxyServiceUsingV8ProxyResolver(
        proxy_config_service_.release(),
        new net::ProxyScriptFetcherImpl(url_request_context_.get()),
        dhcp_factory.Create(url_request_context_.get()),
        host_resolver.get(),
        NULL,
        url_request_context_->network_delegate());
    storage_->set_proxy_service(proxy_service);

    storage_->set_ssl_config_service(new net::SSLConfigServiceDefaults);
    storage_->set_http_auth_handler_factory(
            CreateDefaultAuthHandlerFactory(host_resolver.get()));

    storage_->set_http_server_properties(
        scoped_ptr<net::HttpServerProperties>(
            new net::HttpServerPropertiesImpl()));

    FilePath cache_path = data_path_.Append(FILE_PATH_LITERAL("Cache"));
    net::HttpCache::DefaultBackend* main_backend =
        new net::HttpCache::DefaultBackend(
            net::DISK_CACHE,
            net::CACHE_BACKEND_SIMPLE,
            cache_path,
            10 * 1024 * 1024,  // 10M
            BrowserThread::GetMessageLoopProxyForThread(
                BrowserThread::CACHE));

    net::HttpNetworkSession::Params network_session_params;
    network_session_params.cert_verifier =
        url_request_context_->cert_verifier();
    network_session_params.transport_security_state =
        url_request_context_->transport_security_state();
    network_session_params.channel_id_service =
        url_request_context_->channel_id_service();
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

    scoped_ptr<net::URLRequestJobFactoryImpl> job_factory(
        new net::URLRequestJobFactoryImpl());
    InstallProtocolHandlers(job_factory.get(), &protocol_handlers_);
    job_factory->SetProtocolHandler(
         url::kFileScheme,
         new net::FileProtocolHandler(
               content::BrowserThread::GetBlockingPool()->
               GetTaskRunnerWithShutdownBehavior(
                    base::SequencedWorkerPool::SKIP_ON_SHUTDOWN)));
    job_factory->SetProtocolHandler("app",
                                    new net::AppProtocolHandler(root_path_));
    job_factory->SetProtocolHandler("nw", new nw::NwProtocolHandler());

    storage_->set_job_factory(job_factory.release());

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

void ShellURLRequestContextGetter::SetAdditionalTrustAnchors(const net::CertificateList& trust_anchors)
{
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::IO));
  trust_anchors_ = trust_anchors;
}

const net::CertificateList& ShellURLRequestContextGetter::GetAdditionalTrustAnchors() {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::IO));
  return trust_anchors_;
}

net::HttpAuthHandlerFactory* ShellURLRequestContextGetter::CreateDefaultAuthHandlerFactory(
    net::HostResolver* resolver) {
  net::HttpAuthFilterWhitelist* auth_filter_default_credentials = NULL;
  if (!auth_server_whitelist_.empty()) {
    auth_filter_default_credentials =
        new net::HttpAuthFilterWhitelist(auth_server_whitelist_);
  }
  net::HttpAuthFilterWhitelist* auth_filter_delegate = NULL;
  if (!auth_delegate_whitelist_.empty()) {
    auth_filter_delegate =
        new net::HttpAuthFilterWhitelist(auth_delegate_whitelist_);
  }
  url_security_manager_.reset(
      net::URLSecurityManager::Create(auth_filter_default_credentials,
                                      auth_filter_delegate));
  std::vector<std::string> supported_schemes;
  base::SplitString(auth_schemes_, ',', &supported_schemes);

  scoped_ptr<net::HttpAuthHandlerRegistryFactory> registry_factory(
      net::HttpAuthHandlerRegistryFactory::Create(
          supported_schemes, url_security_manager_.get(),
          resolver, gssapi_library_name_, negotiate_disable_cname_lookup_,
          negotiate_enable_port_));

  return registry_factory.release();
}

}  // namespace content
