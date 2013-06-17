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

#include "content/nw/src/shell_browser_context.h"

#include "base/command_line.h"
#include "base/environment.h"
#include "base/file_util.h"
#include "base/path_service.h"
#include "base/values.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/resource_context.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/common/content_switches.h"
#include "content/nw/src/browser/shell_download_manager_delegate.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/net/shell_url_request_context_getter.h"
#include "content/nw/src/nw_package.h"

#if defined(OS_WIN)
#include "base/base_paths_win.h"
#elif defined(OS_LINUX)
#include "base/nix/xdg_util.h"
#elif defined(OS_MACOSX)
#include "base/base_paths_mac.h"
#endif

namespace content {
class ShellBrowserContext::ShellResourceContext : public ResourceContext {
 public:
  ShellResourceContext() : getter_(NULL) {}
  virtual ~ShellResourceContext() {}

  // ResourceContext implementation:
  virtual net::HostResolver* GetHostResolver() OVERRIDE {
    CHECK(getter_);
    return getter_->host_resolver();
  }
  virtual net::URLRequestContext* GetRequestContext() OVERRIDE {
    CHECK(getter_);
    return getter_->GetURLRequestContext();
  }

  void set_url_request_context_getter(ShellURLRequestContextGetter* getter) {
    getter_ = getter;
  }

 private:
  ShellURLRequestContextGetter* getter_;

  DISALLOW_COPY_AND_ASSIGN(ShellResourceContext);
};

ShellBrowserContext::ShellBrowserContext(bool off_the_record,
                                         nw::Package* package)
  : disable_pinning_renderer_(false),
    off_the_record_(off_the_record),
    ignore_certificate_errors_(false),
    package_(package),
    resource_context_(new ShellResourceContext) {
  InitWhileIOAllowed();
}

ShellBrowserContext::~ShellBrowserContext() {
  if (resource_context_.get()) {
    BrowserThread::DeleteSoon(
      BrowserThread::IO, FROM_HERE, resource_context_.release());
  }
}

void ShellBrowserContext::InitWhileIOAllowed() {
  CommandLine* cmd_line = CommandLine::ForCurrentProcess();
  if (cmd_line->HasSwitch(switches::kIgnoreCertificateErrors)) {
    ignore_certificate_errors_ = true;
  }
  if (cmd_line->HasSwitch(switches::kContentShellDataPath)) {
    path_ = cmd_line->GetSwitchValuePath(switches::kContentShellDataPath);
    return;
  }
  FilePath::StringType name(
#if defined(OS_WIN)
      L"node-webkit"
#else
      "node-webkit"
#endif
      );
  package_->root()->GetString(switches::kmName, &name);
#if defined(OS_WIN)
  CHECK(PathService::Get(base::DIR_LOCAL_APP_DATA, &path_));
  path_ = path_.Append(name);
#elif defined(OS_LINUX)
  scoped_ptr<base::Environment> env(base::Environment::Create());
  FilePath config_dir(
      base::nix::GetXDGDirectory(env.get(),
                                 base::nix::kXdgConfigHomeEnvVar,
                                 base::nix::kDotConfigDir));
  path_ = config_dir.Append(name);
#elif defined(OS_MACOSX)
  CHECK(PathService::Get(base::DIR_APP_DATA, &path_));
  path_ = path_.Append(name);
#else
  NOTIMPLEMENTED();
#endif

  if (!file_util::PathExists(path_))
    file_util::CreateDirectory(path_);
}

FilePath ShellBrowserContext::GetPath() {
  return path_;
}

bool ShellBrowserContext::IsOffTheRecord() const {
  return off_the_record_;
}

DownloadManagerDelegate* ShellBrowserContext::GetDownloadManagerDelegate()  {
  DownloadManager* manager = BrowserContext::GetDownloadManager(this);

  if (!download_manager_delegate_.get()) {
    download_manager_delegate_ = new ShellDownloadManagerDelegate();
    download_manager_delegate_->SetDownloadManager(manager);
  }

  return download_manager_delegate_.get();
}

net::URLRequestContextGetter* ShellBrowserContext::GetRequestContext()  {
  return GetDefaultStoragePartition(this)->GetURLRequestContext();
}

net::URLRequestContextGetter* ShellBrowserContext::CreateRequestContext(
    ProtocolHandlerMap* protocol_handlers) {
  DCHECK(!url_request_getter_);
  url_request_getter_ = new ShellURLRequestContextGetter(
      ignore_certificate_errors_,
      GetPath(),
      BrowserThread::UnsafeGetMessageLoopForThread(BrowserThread::IO),
      BrowserThread::UnsafeGetMessageLoopForThread(BrowserThread::FILE),
      protocol_handlers);
  resource_context_->set_url_request_context_getter(url_request_getter_.get());
  return url_request_getter_.get();
}

net::URLRequestContextGetter*
    ShellBrowserContext::GetRequestContextForRenderProcess(
        int renderer_child_id)  {
  return GetRequestContext();
}

net::URLRequestContextGetter*
    ShellBrowserContext::GetMediaRequestContext()  {
  return GetRequestContext();
}

net::URLRequestContextGetter*
    ShellBrowserContext::GetMediaRequestContextForRenderProcess(
        int renderer_child_id)  {
  return GetRequestContext();
}

net::URLRequestContextGetter*
    ShellBrowserContext::GetMediaRequestContextForStoragePartition(
        const FilePath& partition_path,
        bool in_memory) {
  return GetRequestContext();
}

net::URLRequestContextGetter*
    ShellBrowserContext::CreateRequestContextForStoragePartition(
        const base::FilePath& partition_path,
        bool in_memory,
        ProtocolHandlerMap* protocol_handlers) {
  return NULL;
}

ResourceContext* ShellBrowserContext::GetResourceContext()  {
  return resource_context_.get();
}

GeolocationPermissionContext*
    ShellBrowserContext::GetGeolocationPermissionContext()  {
  return NULL;
}

SpeechRecognitionPreferences*
    ShellBrowserContext::GetSpeechRecognitionPreferences() {
  return NULL;
}

quota::SpecialStoragePolicy* ShellBrowserContext::GetSpecialStoragePolicy() {
  return NULL;
}

}  // namespace content
