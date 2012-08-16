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

#include "content/shell/shell_browser_main.h"

#include "base/command_line.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/threading/thread_restrictions.h"
#include "content/public/browser/browser_main_runner.h"
#include "layout_test_controller_host.h"
#include "nw_package.h"
#include "shell.h"
#include "shell_browser_context.h"
#include "shell_content_browser_client.h"
#include "shell_switches.h"
#include "webkit/support/webkit_support.h"

namespace {

GURL GetURLForLayoutTest(const char* test_name,
                         std::string* expected_pixel_hash) {
#if defined(OS_ANDROID)
  // DumpRenderTree is not currently supported for Android using the content
  // shell.
  NOTIMPLEMENTED();
  return GURL::EmptyGURL();
#else
  std::string path_or_url = test_name;
  std::string pixel_hash;
  std::string::size_type separator_position = path_or_url.find('\'');
  if (separator_position != std::string::npos) {
    pixel_hash = path_or_url.substr(separator_position + 1);
    path_or_url.erase(separator_position);
  }
  if (expected_pixel_hash)
    *expected_pixel_hash = pixel_hash;
  GURL test_url = webkit_support::CreateURLForPathOrURL(path_or_url);
  {
    // We're outside of the message loop here, and this is a test.
    base::ThreadRestrictions::ScopedAllowIO allow_io;
    webkit_support::SetCurrentDirectoryForFileURL(test_url);
  }
  return test_url;
#endif
}

}  // namespace

// Main routine for running as the Browser process.
int ShellBrowserMain(const content::MainFunctionParams& parameters) {
  scoped_ptr<content::BrowserMainRunner> main_runner_(
      content::BrowserMainRunner::Create());

  nw::InitPackageForceNoEmpty();

  int exit_code = main_runner_->Initialize(parameters);

  if (exit_code >= 0)
    return exit_code;

  if (CommandLine::ForCurrentProcess()->HasSwitch(
        switches::kCheckLayoutTestSysDeps)) {
    return 0;
  }

  bool layout_test_mode =
      CommandLine::ForCurrentProcess()->HasSwitch(switches::kDumpRenderTree);

  if (layout_test_mode) {
    char test_string[2048];
    content::ShellBrowserContext* browser_context =
        static_cast<content::ShellContentBrowserClient*>(
            content::GetContentClient()->browser())->browser_context();

    while (fgets(test_string, sizeof(test_string), stdin)) {
      char *new_line_position = strchr(test_string, '\n');
      if (new_line_position)
        *new_line_position = '\0';
      if (test_string[0] == '\0')
        continue;

      // Test header.
      printf("Content-Type: text/plain\n");

      std::string pixel_hash;
      content::Shell::CreateNewWindow(
          browser_context,
          GetURLForLayoutTest(test_string, &pixel_hash),
          NULL,
          MSG_ROUTING_NONE,
          NULL);
      content::LayoutTestControllerHost::Init(pixel_hash);

      main_runner_->Run();

      content::Shell::CloseAllWindows();

      // Test footer.
      printf("#EOF\n");
      fflush(stdout);
      fflush(stderr);
    }
    exit_code = 0;
  } else {
    exit_code = main_runner_->Run();
  }

  main_runner_->Shutdown();

  return exit_code;
}
