// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_SHELL_BROWSER_CONTEXT_H_
#define CONTENT_SHELL_SHELL_BROWSER_CONTEXT_H_

#include "base/compiler_specific.h"
#include "base/file_path.h"
#include "base/files/scoped_temp_dir.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "content/public/browser/browser_context.h"

namespace nw {
class Package;
}

namespace content {

class DownloadManagerDelegate;
class ResourceContext;
class ShellDownloadManagerDelegate;

class ShellBrowserContext : public BrowserContext {
 public:
  explicit ShellBrowserContext(bool off_the_record,
                               nw::Package* package);
  virtual ~ShellBrowserContext();

  // BrowserContext implementation.
  virtual FilePath GetPath() OVERRIDE;
  virtual bool IsOffTheRecord() const OVERRIDE;
  virtual DownloadManagerDelegate* GetDownloadManagerDelegate() OVERRIDE;
  virtual net::URLRequestContextGetter* GetRequestContext() OVERRIDE;
  virtual net::URLRequestContextGetter* GetRequestContextForRenderProcess(
      int renderer_child_id) OVERRIDE;
  virtual net::URLRequestContextGetter* GetMediaRequestContext() OVERRIDE;
  virtual net::URLRequestContextGetter* GetMediaRequestContextForRenderProcess(
      int renderer_child_id) OVERRIDE;
  virtual net::URLRequestContextGetter*
      GetMediaRequestContextForStoragePartition(
          const FilePath& partition_path,
          bool in_memory) OVERRIDE;
  virtual net::URLRequestContextGetter* GetRequestContextForStoragePartition(
      const FilePath& partition_path,
      bool in_memory) OVERRIDE;
  virtual ResourceContext* GetResourceContext() OVERRIDE;
  virtual GeolocationPermissionContext*
      GetGeolocationPermissionContext() OVERRIDE;
  virtual SpeechRecognitionPreferences*
      GetSpeechRecognitionPreferences() OVERRIDE;
  virtual quota::SpecialStoragePolicy* GetSpecialStoragePolicy() OVERRIDE;

  bool pinning_renderer() { return !disable_pinning_renderer_; }
  void set_pinning_renderer(bool val) { disable_pinning_renderer_ = !val; }
 private:
  // Performs initialization of the ShellBrowserContext while IO is still
  // allowed on the current thread.
  void InitWhileIOAllowed();

  bool disable_pinning_renderer_;  // whether dev reload is in process
                                   // or we want to disable pinning
                                   // temporarily
  bool off_the_record_;
  nw::Package* package_;
  base::ScopedTempDir testing_path_;
  FilePath path_;
  scoped_ptr<ResourceContext> resource_context_;
  scoped_refptr<ShellDownloadManagerDelegate> download_manager_delegate_;
  scoped_refptr<net::URLRequestContextGetter> url_request_getter_;

  DISALLOW_COPY_AND_ASSIGN(ShellBrowserContext);
};

}  // namespace content

#endif  // CONTENT_SHELL_SHELL_BROWSER_CONTEXT_H_
