// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/shell_main_delegate.h"

#include "base/command_line.h"
#include "base/file_path.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "content/public/browser/browser_main_runner.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/url_constants.h"
#include "content/shell/shell_browser_main.h"
#include "content/shell/shell_content_browser_client.h"
#include "content/shell/shell_content_renderer_client.h"
#include "content/shell/shell_switches.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/ui_base_paths.h"

#if defined(OS_ANDROID)
#include "base/global_descriptors_posix.h"
#include "content/shell/android/shell_descriptors.h"
#endif

#if defined(OS_MACOSX)
#include "content/shell/paths_mac.h"
#endif  // OS_MACOSX

#if defined(OS_WIN)
#include "base/logging_win.h"
#include <initguid.h>
#endif

namespace {

#if defined(OS_WIN)
// If "Content Shell" doesn't show up in your list of trace providers in
// Sawbuck, add these registry entries to your machine (NOTE the optional
// Wow6432Node key for x64 machines):
// 1. Find:  HKLM\SOFTWARE\[Wow6432Node\]Google\Sawbuck\Providers
// 2. Add a subkey with the name "{6A3E50A4-7E15-4099-8413-EC94D8C2A4B6}"
// 3. Add these values:
//    "default_flags"=dword:00000001
//    "default_level"=dword:00000004
//    @="Content Shell"

// {6A3E50A4-7E15-4099-8413-EC94D8C2A4B6}
const GUID kContentShellProviderName = {
    0x6a3e50a4, 0x7e15, 0x4099,
        { 0x84, 0x13, 0xec, 0x94, 0xd8, 0xc2, 0xa4, 0xb6 } };
#endif

void InitLogging() {
  FilePath log_filename;
  PathService::Get(base::DIR_EXE, &log_filename);
  log_filename = log_filename.AppendASCII("content_shell.log");
  logging::InitLogging(
      log_filename.value().c_str(),
      logging::LOG_ONLY_TO_FILE,
      logging::LOCK_LOG_FILE,
      logging::DELETE_OLD_LOG_FILE,
      logging::DISABLE_DCHECK_FOR_NON_OFFICIAL_RELEASE_BUILDS);
  logging::SetLogItems(true, true, true, true);
}

}  // namespace

namespace content {

ShellMainDelegate::ShellMainDelegate() {
}

ShellMainDelegate::~ShellMainDelegate() {
#if defined(OS_ANDROID)
  NOTREACHED();
#endif
}

bool ShellMainDelegate::BasicStartupComplete(int* exit_code) {
#if defined(OS_WIN)
  // Enable trace control and transport through event tracing for Windows.
  logging::LogEventProvider::Initialize(kContentShellProviderName);
#endif

  if (CommandLine::ForCurrentProcess()->HasSwitch(switches::kDumpRenderTree)) {
    InitLogging();
    CommandLine::ForCurrentProcess()->AppendSwitch(
        switches::kAllowFileAccessFromFiles);
  }
  SetContentClient(&content_client_);
  return false;
}

void ShellMainDelegate::PreSandboxStartup() {
#if defined(OS_MACOSX)
  OverrideFrameworkBundlePath();
  OverrideChildProcessPath();
#endif  // OS_MACOSX
  InitializeResourceBundle();
}

int ShellMainDelegate::RunProcess(
    const std::string& process_type,
    const MainFunctionParams& main_function_params) {
  if (!process_type.empty())
    return -1;

#if !defined(OS_ANDROID)
  return ShellBrowserMain(main_function_params);
#else
  // If no process type is specified, we are creating the main browser process.
  browser_runner_.reset(BrowserMainRunner::Create());
  int exit_code = browser_runner_->Initialize(main_function_params);
  DCHECK(exit_code < 0)
      << "BrowserRunner::Initialize failed in ShellMainDelegate";

  return exit_code;
#endif
}

void ShellMainDelegate::InitializeResourceBundle() {
#if defined(OS_ANDROID)
  // In the Android case, the renderer runs with a different UID and can never
  // access the file system.  So we are passed a file descriptor to the
  // ResourceBundle pak at launch time.
  int pak_fd =
      base::GlobalDescriptors::GetInstance()->MaybeGet(kShellPakDescriptor);
  if (pak_fd != base::kInvalidPlatformFileValue) {
    ui::ResourceBundle::InitSharedInstanceWithPakFile(pak_fd, false);
    ResourceBundle::GetSharedInstance().AddDataPackFromFile(
        pak_fd, ui::SCALE_FACTOR_100P);
    return;
  }
#endif

  FilePath pak_file;
#if defined(OS_MACOSX)
  pak_file = GetResourcesPakFilePath();
#else
  FilePath pak_dir;

#if defined(OS_ANDROID)
  bool got_path = PathService::Get(base::DIR_ANDROID_APP_DATA, &pak_dir);
  DCHECK(got_path);
  pak_dir = pak_dir.Append(FILE_PATH_LITERAL("paks"));
#else
  PathService::Get(base::DIR_MODULE, &pak_dir);
#endif

  pak_file = pak_dir.Append(FILE_PATH_LITERAL("content_shell.pak"));
#endif
  ui::ResourceBundle::InitSharedInstanceWithPakPath(pak_file);
}

ContentBrowserClient* ShellMainDelegate::CreateContentBrowserClient() {
  browser_client_.reset(new ShellContentBrowserClient);
  return browser_client_.get();
}

ContentRendererClient* ShellMainDelegate::CreateContentRendererClient() {
  renderer_client_.reset(new ShellContentRendererClient);
  return renderer_client_.get();
}

}  // namespace content
