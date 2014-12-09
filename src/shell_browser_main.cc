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

#include "content/nw/src/shell_browser_main.h"

#include "base/command_line.h"
#include "base/compiler_specific.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/threading/thread_restrictions.h"
#include "content/public/common/content_switches.h"
#include "content/public/browser/browser_main_runner.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/nw_shell.h"
#include "content/nw/src/shell_browser_context.h"
#include "content/nw/src/shell_content_browser_client.h"

// Main routine for running as the Browser process.
int ShellBrowserMain(const content::MainFunctionParams& parameters) {
  scoped_ptr<content::BrowserMainRunner> main_runner_(
      content::BrowserMainRunner::Create());

  int exit_code = main_runner_->Initialize(parameters);

  if (exit_code >= 0)
    return exit_code;

  main_runner_->Run();

  content::Shell::Cleanup();

  main_runner_->Shutdown();

  return content::Shell::exit_code();
}
