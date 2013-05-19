// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_SHELL_BROWSER_MAIN_PARTS_H_
#define CONTENT_SHELL_SHELL_BROWSER_MAIN_PARTS_H_

#include "base/memory/ref_counted_memory.h"
#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "chrome/browser/process_singleton.h"
#include "content/public/browser/browser_main_parts.h"
#include "content/public/browser/render_view_host.h"

namespace base {
class Thread;
}

namespace nw {
class Package;
}

class CommandLine;
class FilePath;

namespace printing {
class PrintJobManager;
}

namespace content {

class ShellBrowserContext;
class ShellDevToolsDelegate;
struct MainFunctionParams;

class ShellBrowserMainParts : public BrowserMainParts {
 public:
  explicit ShellBrowserMainParts(const MainFunctionParams& parameters);
  virtual ~ShellBrowserMainParts();

  // BrowserMainParts overrides.
  virtual void PreEarlyInitialization() OVERRIDE;
  virtual void PreMainMessageLoopStart() OVERRIDE;
  virtual void PreMainMessageLoopRun() OVERRIDE;
  virtual void PostMainMessageLoopStart() OVERRIDE;
  virtual bool MainMessageLoopRun(int* result_code) OVERRIDE;
  virtual void PostMainMessageLoopRun() OVERRIDE;
  virtual int PreCreateThreads() OVERRIDE;

  // Init browser context and every thing
  void Init();

  ShellDevToolsDelegate* devtools_delegate() { return devtools_delegate_; }

  ShellBrowserContext* browser_context() { return browser_context_.get(); }
  ShellBrowserContext* off_the_record_browser_context() {
    return off_the_record_browser_context_.get();
  }
  nw::Package* package() { return package_.get(); }
  virtual printing::PrintJobManager* print_job_manager();

 private:
  bool ProcessSingletonNotificationCallback(const CommandLine& command_line,
                                            const base::FilePath& current_directory);

  scoped_ptr<ShellBrowserContext> browser_context_;
  scoped_ptr<ShellBrowserContext> off_the_record_browser_context_;
  scoped_ptr<nw::Package> package_;

  scoped_ptr<ProcessSingleton> process_singleton_;

  // Ensures that all the print jobs are finished before closing the browser.
  scoped_ptr<printing::PrintJobManager> print_job_manager_;

  // For running content_browsertests.
  const MainFunctionParams& parameters_;
  bool run_message_loop_;

  ShellDevToolsDelegate* devtools_delegate_;
  content::RenderViewHost::CreatedCallback rvh_callback_;

  //base::WeakPtrFactory<ShellBrowserMainParts> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(ShellBrowserMainParts);
};

}  // namespace content

#endif  // CONTENT_SHELL_SHELL_BROWSER_MAIN_PARTS_H_
