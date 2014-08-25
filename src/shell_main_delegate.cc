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

#include "content/nw/src/shell_main_delegate.h"

#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/file_util.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/browser_main_runner.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/url_constants.h"
#include "content/nw/src/breakpad_linux.h"
#include "content/nw/src/chrome_breakpad_client.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/nw_version.h"
#include "content/nw/src/renderer/shell_content_renderer_client.h"
#include "content/nw/src/shell_browser_main.h"
#include "content/nw/src/shell_content_browser_client.h"
#include "net/cookies/cookie_monster.h"
#include "third_party/node/src/node_webkit.h"
#include "third_party/node/src/node_version.h"
#include "third_party/zlib/google/zip_reader.h"
#include "ui/base/l10n/l10n_util.h"
#if defined(OS_MACOSX)
#include "ui/base/l10n/l10n_util_mac.h"
#endif
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/ui_base_paths.h"
#include "ui/base/ui_base_switches.h"

#include <stdio.h>

using base::FilePath;

#if defined(OS_MACOSX)
#include "content/nw/src/paths_mac.h"
#endif  // OS_MACOSX

#if defined(OS_WIN)
#include "base/logging_win.h"
#include <initguid.h>
#include "content/nw/src/breakpad_win.h"
#endif

#include "ipc/ipc_message.h"  // For IPC_MESSAGE_LOG_ENABLED.

#if defined(IPC_MESSAGE_LOG_ENABLED)
#define IPC_MESSAGE_MACROS_LOG_ENABLED
#include "content/public/common/content_ipc_logging.h"
#define IPC_LOG_TABLE_ADD_ENTRY(msg_id, logger) \
    content::RegisterIPCLogger(msg_id, logger)
#include "content/nw/src/common/common_message_generator.h"
#include "components/autofill/content/common/autofill_messages.h"
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

base::LazyInstance<chrome::ChromeBreakpadClient>::Leaky
    g_chrome_breakpad_client = LAZY_INSTANCE_INITIALIZER;

void InitLogging() {
  base::FilePath log_filename;
  PathService::Get(base::DIR_EXE, &log_filename);
  log_filename = log_filename.AppendASCII("debug.log");
  logging::LoggingSettings settings;
  if (CommandLine::ForCurrentProcess()->HasSwitch(switches::kEnableLogging)) {
    settings.logging_dest = logging::LOG_TO_ALL;
    settings.log_file = log_filename.value().c_str();
    settings.delete_old = logging::DELETE_OLD_LOG_FILE;
  }else{
#if defined(OS_WIN)
    settings.logging_dest = logging::LOG_NONE;
#endif
  }
  logging::InitLogging(settings);
  logging::SetLogItems(true, false, true, false);
}

}  // namespace

namespace content {

ShellMainDelegate::ShellMainDelegate() {
}

ShellMainDelegate::~ShellMainDelegate() {
}

bool ShellMainDelegate::BasicStartupComplete(int* exit_code) {
  CommandLine* command_line = CommandLine::ForCurrentProcess();
  const CommandLine::StringVector& args = command_line->GetArgs();
  if (args.size() > 0) {
    zip::ZipReader reader;
    FilePath fp(args[0]);
    if (!command_line->HasSwitch(switches::kProcessType) &&
        PathExists(fp) && !DirectoryExists(fp) && !reader.Open(fp)) {
      *exit_code = node::Start(command_line->argc0(), command_line->argv0());
      return true;
    }
  }

#if defined(OS_WIN)
  // Enable trace control and transport through event tracing for Windows.
  logging::LogEventProvider::Initialize(kContentShellProviderName);
#endif

#if defined(OS_MACOSX)
  // Needs to happen before InitializeResourceBundle() and before
  // WebKitTestPlatformInitialize() are called.
  OverrideFrameworkBundlePath();
  OverrideChildProcessPath();
  // FIXME: EnsureCorrectResolutionSettings();
  l10n_util::OverrideLocaleWithUserDefault();
#endif  // OS_MACOSX

  InitLogging();
  // FIXME: net::CookieMonster::EnableFileScheme();

  SetContentClient(&content_client_);
  return false;
}

void ShellMainDelegate::PreSandboxStartup() {
  breakpad::SetBreakpadClient(g_chrome_breakpad_client.Pointer());
  CommandLine* command_line = CommandLine::ForCurrentProcess();
  std::string pref_locale;
  if (command_line->HasSwitch(switches::kLang)) {
    pref_locale = command_line->GetSwitchValueASCII(switches::kLang);
  }

  InitializeResourceBundle(pref_locale);

  std::string process_type =
      command_line->GetSwitchValueASCII(switches::kProcessType);

  if (process_type != switches::kZygoteProcess)
    breakpad::InitCrashReporter();

  // Just prevent sandbox.
  command_line->AppendSwitch(switches::kNoSandbox);

  // Make sure we keep using only one render process for one app, by using the
  // process-per-tab mode, we can make Chrome only create new site instance
  // when we require to, the default mode is creating different site instances
  // for different domains.
  // This is needed because we want our Window API to have full control of new 
  // windows created, which require all windows to be in one render process
  // host.
  command_line->AppendSwitch(switches::kProcessPerTab);

  // Allow file:// URIs can read other file:// URIs by default.
  command_line->AppendSwitch(switches::kAllowFileAccessFromFiles);
}

int ShellMainDelegate::RunProcess(
    const std::string& process_type,
    const MainFunctionParams& main_function_params) {
  if (!process_type.empty())
    return -1;

  return ShellBrowserMain(main_function_params);
}

void ShellMainDelegate::InitializeResourceBundle(const std::string& pref_locale) {
  FilePath pak_file;
#if defined(OS_MACOSX)
  FilePath locale_file;
  if (!GetResourcesPakFilePath(pak_file))
    LOG(FATAL) << "nw.pak file not found.";
  std::string locale = l10n_util::GetApplicationLocale(pref_locale);
  if (!GetLocalePakFilePath(locale, locale_file)) {
    LOG(WARNING) << locale << ".pak file not found.";
    locale = "en-US";
    if (!GetLocalePakFilePath(locale, locale_file))
      LOG(ERROR) << locale << ".pak file not found.";
  }
  ui::ResourceBundle::InitSharedInstanceWithPakPath2(pak_file, locale_file);
#else
  FilePath pak_dir;
  PathService::Get(base::DIR_MODULE, &pak_dir);
  pak_file = pak_dir.Append(FILE_PATH_LITERAL("nw.pak"));
  CHECK(base::PathExists(pak_file)) << "nw.pak is missing";
  ui::ResourceBundle::InitSharedInstanceWithPakPath(pak_file);
#endif
}

ContentBrowserClient* ShellMainDelegate::CreateContentBrowserClient() {
  browser_client_.reset(new ShellContentBrowserClient);
  return browser_client_.get();
}

ContentRendererClient* ShellMainDelegate::CreateContentRendererClient() {
  renderer_client_.reset(new ShellContentRendererClient);
  return renderer_client_.get();
}

#if defined(OS_POSIX) && !defined(OS_MACOSX)
void ShellMainDelegate::ZygoteForked() {
  // Needs to be called after we have chrome::DIR_USER_DATA.  BrowserMain sets
  // this up for the browser process in a different manner.
  breakpad::InitCrashReporter();
}
#endif

}  // namespace content
