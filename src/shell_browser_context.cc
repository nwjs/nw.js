// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "shell_browser_context.h"

#include "base/bind.h"
#include "base/command_line.h"
#include "base/environment.h"
#include "base/file_util.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "base/threading/thread.h"
#include "base/values.h"
#include "content/public/browser/browser_thread.h"
#include "nw_package.h"
#include "shell_download_manager_delegate.h"
#include "shell_resource_context.h"
#include "shell_switches.h"
#include "shell_url_request_context_getter.h"

#if defined(OS_WIN)
#include "base/base_paths_win.h"
#elif defined(OS_LINUX)
#include "base/nix/xdg_util.h"
#elif defined(OS_MACOSX)
#include "base/base_paths_mac.h"
#endif

namespace content {

ShellBrowserContext::ShellBrowserContext(bool off_the_record)
    : off_the_record_(off_the_record) {
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
  if (cmd_line->HasSwitch(switches::kContentBrowserTest) ||
      cmd_line->HasSwitch(switches::kDumpRenderTree)) {
    CHECK(testing_path_.CreateUniqueTempDir());
    path_ = testing_path_.path();
    return;
  }
  base::DictionaryValue *manifest = nw::GetManifest();
  FilePath::StringType name(
#if defined(OS_WIN)
      L"node-webkit"
#else
      "node-webkit"
#endif
      );
  manifest->GetString(switches::kmName, &name);
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
#elif defined(OS_ANDROID)
  DCHECK(PathService::Get(base::DIR_ANDROID_APP_DATA, &path_));
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
  download_manager_delegate_ = new ShellDownloadManagerDelegate();
  return download_manager_delegate_.get();
}

net::URLRequestContextGetter* ShellBrowserContext::GetRequestContext()  {
  if (!url_request_getter_) {
    url_request_getter_ = new ShellURLRequestContextGetter(
        GetPath(),
        BrowserThread::UnsafeGetMessageLoopForThread(BrowserThread::IO),
        BrowserThread::UnsafeGetMessageLoopForThread(BrowserThread::FILE));
  }
  return url_request_getter_;
}

net::URLRequestContextGetter*
    ShellBrowserContext::GetRequestContextForRenderProcess(
        int renderer_child_id)  {
  return GetRequestContext();
}

net::URLRequestContextGetter*
    ShellBrowserContext::GetRequestContextForMedia()  {
  return GetRequestContext();
}

ResourceContext* ShellBrowserContext::GetResourceContext()  {
  if (!resource_context_.get()) {
    resource_context_.reset(new ShellResourceContext(
        static_cast<ShellURLRequestContextGetter*>(GetRequestContext())));
  }
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

bool ShellBrowserContext::DidLastSessionExitCleanly()  {
  return true;
}

quota::SpecialStoragePolicy* ShellBrowserContext::GetSpecialStoragePolicy() {
  return NULL;
}

}  // namespace content
