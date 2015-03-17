// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_SHELL_MAIN_DELEGATE_H_
#define CONTENT_SHELL_SHELL_MAIN_DELEGATE_H_

#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "content/nw/src/shell_content_client.h"
#include "content/public/app/content_main_delegate.h"

namespace content {
class ShellContentBrowserClient;
class ShellContentRendererClient;

class ShellMainDelegate : public ContentMainDelegate {
 public:
  ShellMainDelegate();
  ~ShellMainDelegate() final;

  // ContentMainDelegate implementation:
   bool BasicStartupComplete(int* exit_code) override;
   void PreSandboxStartup() override;
   int RunProcess(
      const std::string& process_type,
      const MainFunctionParams& main_function_params) override;
   ContentBrowserClient* CreateContentBrowserClient() override;
   ContentRendererClient* CreateContentRendererClient() override;
   content::ContentUtilityClient* CreateContentUtilityClient() override;

  static void InitializeResourceBundle(const std::string& pref_locale);
#if defined(OS_POSIX) && !defined(OS_MACOSX)
   void ZygoteForked() override;
#endif

 private:
  scoped_ptr<ShellContentBrowserClient> browser_client_;
  scoped_ptr<ShellContentRendererClient> renderer_client_;
  ShellContentClient content_client_;

  DISALLOW_COPY_AND_ASSIGN(ShellMainDelegate);
};

}  // namespace content

#endif  // CONTENT_SHELL_SHELL_MAIN_DELEGATE_H_
