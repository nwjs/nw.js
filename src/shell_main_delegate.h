// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_SHELL_MAIN_DELEGATE_H_
#define CONTENT_SHELL_SHELL_MAIN_DELEGATE_H_

#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "content/shell/shell_content_client.h"
#include "content/public/app/content_main_delegate.h"

namespace content {
class ShellContentBrowserClient;
class ShellContentRendererClient;

#if defined(OS_ANDROID)
class BrowserMainRunner;
#endif

class ShellMainDelegate : public ContentMainDelegate {
 public:
  ShellMainDelegate();
  virtual ~ShellMainDelegate();

  // ContentMainDelegate implementation:
  virtual bool BasicStartupComplete(int* exit_code) OVERRIDE;
  virtual void PreSandboxStartup() OVERRIDE;
  virtual int RunProcess(
      const std::string& process_type,
      const MainFunctionParams& main_function_params) OVERRIDE;
  virtual ContentBrowserClient* CreateContentBrowserClient() OVERRIDE;
  virtual ContentRendererClient* CreateContentRendererClient() OVERRIDE;

  static void InitializeResourceBundle();

 private:
  scoped_ptr<ShellContentBrowserClient> browser_client_;
  scoped_ptr<ShellContentRendererClient> renderer_client_;
  ShellContentClient content_client_;

#if defined(OS_ANDROID)
  scoped_ptr<BrowserMainRunner> browser_runner_;
#endif

  DISALLOW_COPY_AND_ASSIGN(ShellMainDelegate);
};

}  // namespace content

#endif  // CONTENT_SHELL_SHELL_MAIN_DELEGATE_H_
