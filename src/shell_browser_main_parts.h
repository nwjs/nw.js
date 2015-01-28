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

namespace base {
class CommandLine;
class FilePath;
}

namespace printing {
class PrintJobManager;
}

namespace extensions {

class ExtensionsBrowserClient;
class ExtensionsClient;
class ShellExtensionSystem;

}

namespace content {

class BrowserContext;
class ShellBrowserContext;
class DevToolsHttpHandler;
struct MainFunctionParams;

class ShellBrowserMainParts : public BrowserMainParts {
 public:
  explicit ShellBrowserMainParts(const MainFunctionParams& parameters);
  ~ShellBrowserMainParts() final;

  extensions::ShellExtensionSystem* extension_system() { return extension_system_; }

  // BrowserMainParts overrides.
   void PreEarlyInitialization() override;
   void PreMainMessageLoopStart() override;
   void PreMainMessageLoopRun() override;
   void PostMainMessageLoopStart() override;
   bool MainMessageLoopRun(int* result_code) override;
   void PostMainMessageLoopRun() override;
   int  PreCreateThreads() override;
   void PostDestroyThreads() override;
   void ToolkitInitialized() override;

  // Init browser context and every thing
  void Init();

  DevToolsHttpHandler* devtools_handler() { return devtools_http_handler_.get(); }

  ShellBrowserContext* browser_context() { return browser_context_.get(); }
  ShellBrowserContext* off_the_record_browser_context() {
    return off_the_record_browser_context_.get();
  }
  nw::Package* package() { return package_.get(); }
   printing::PrintJobManager* print_job_manager();

 protected:
   extensions::ExtensionsClient* CreateExtensionsClient();
   extensions::ExtensionsBrowserClient* CreateExtensionsBrowserClient(content::BrowserContext* context);

 private:
  // Creates and initializes the ExtensionSystem.
  void CreateExtensionSystem();
  bool ProcessSingletonNotificationCallback(const base::CommandLine& command_line,
                                            const base::FilePath& current_directory);

  scoped_ptr<ShellBrowserContext> browser_context_;
  scoped_ptr<ShellBrowserContext> off_the_record_browser_context_;
  scoped_ptr<nw::Package> package_;

  scoped_ptr<ProcessSingleton> process_singleton_;
  scoped_ptr<extensions::ExtensionsClient> extensions_client_;
  scoped_ptr<extensions::ExtensionsBrowserClient> extensions_browser_client_;

  // Ensures that all the print jobs are finished before closing the browser.
  scoped_ptr<printing::PrintJobManager> print_job_manager_;

  // For running content_browsertests.
  const MainFunctionParams& parameters_;
  bool run_message_loop_;

  scoped_ptr<DevToolsHttpHandler> devtools_http_handler_;
  ProcessSingleton::NotifyResult notify_result_;

  // Owned by the KeyedService system.
  extensions::ShellExtensionSystem* extension_system_;

  //base::WeakPtrFactory<ShellBrowserMainParts> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(ShellBrowserMainParts);
};

}  // namespace content

#endif  // CONTENT_SHELL_SHELL_BROWSER_MAIN_PARTS_H_
