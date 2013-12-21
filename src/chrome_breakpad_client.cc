// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/app/chrome_breakpad_client.h"

#include "base/command_line.h"
#include "base/environment.h"
#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/path_service.h"
#include "base/strings/string_split.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/common/chrome_result_codes.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/common/crash_keys.h"
#include "chrome/common/env_vars.h"

#if defined(OS_WIN)
#include <windows.h>

#include "base/file_version_info.h"
#include "chrome/installer/util/google_chrome_sxs_distribution.h"
#include "chrome/installer/util/install_util.h"
#endif

#if defined(OS_POSIX) && !defined(OS_MACOSX) && !defined(OS_IOS)
#include "chrome/browser/crash_upload_list.h"
//#include "chrome/common/chrome_version_info_posix.h"
#include "content/nw/src/nw_version.h"
#endif

#if defined(OS_POSIX)
#include "chrome/common/dump_without_crashing.h"
#endif

#if defined(OS_WIN) || defined(OS_MACOSX)
#include "chrome/installer/util/google_update_settings.h"
#endif

#if defined(OS_ANDROID)
#include "chrome/common/descriptors_android.h"
#endif

namespace chrome {

namespace {

#if defined(OS_WIN)
// This is the minimum version of google update that is required for deferred
// crash uploads to work.
const char kMinUpdateVersion[] = "1.3.21.115";
#endif

}  // namespace

ChromeBreakpadClient::ChromeBreakpadClient() {}

ChromeBreakpadClient::~ChromeBreakpadClient() {}

void ChromeBreakpadClient::SetClientID(const std::string& client_id) {
  crash_keys::SetClientID(client_id);
}

#if defined(OS_WIN)
bool ChromeBreakpadClient::GetAlternativeCrashDumpLocation(
    base::FilePath* crash_dir) {
  // By setting the BREAKPAD_DUMP_LOCATION environment variable, an alternate
  // location to write breakpad crash dumps can be set.
  scoped_ptr<base::Environment> env(base::Environment::Create());
  std::string alternate_crash_dump_location;
  if (env->GetVar("BREAKPAD_DUMP_LOCATION", &alternate_crash_dump_location)) {
    *crash_dir = base::FilePath::FromUTF8Unsafe(alternate_crash_dump_location);
    return true;
  }

  return false;
}

void ChromeBreakpadClient::GetProductNameAndVersion(
    const base::FilePath& exe_path,
    base::string16* product_name,
    base::string16* version,
    base::string16* special_build,
    base::string16* channel_name) {
  DCHECK(product_name);
  DCHECK(version);
  DCHECK(special_build);
  DCHECK(channel_name);

  scoped_ptr<FileVersionInfo> version_info(
      FileVersionInfo::CreateFileVersionInfo(exe_path));

  if (version_info.get()) {
    // Get the information from the file.
    *version = version_info->product_version();
    if (!version_info->is_official_build())
      version->append(base::ASCIIToUTF16("-devel"));

    const CommandLine& command = *CommandLine::ForCurrentProcess();
    if (command.HasSwitch(switches::kChromeFrame)) {
      *product_name = base::ASCIIToUTF16("ChromeFrame");
    } else {
      *product_name = version_info->product_short_name();
    }

    *special_build = version_info->special_build();
  } else {
    // No version info found. Make up the values.
    *product_name = base::ASCIIToUTF16("Chrome");
    *version = base::ASCIIToUTF16("0.0.0.0-devel");
  }
}

bool ChromeBreakpadClient::ShouldShowRestartDialog(base::string16* title,
                                                   base::string16* message,
                                                   bool* is_rtl_locale) {
  scoped_ptr<base::Environment> env(base::Environment::Create());
  if (!env->HasVar(env_vars::kShowRestart) ||
      !env->HasVar(env_vars::kRestartInfo)) {
    return false;
  }

  std::string restart_info;
  env->GetVar(env_vars::kRestartInfo, &restart_info);

  // The CHROME_RESTART var contains the dialog strings separated by '|'.
  // See ChromeBrowserMainPartsWin::PrepareRestartOnCrashEnviroment()
  // for details.
  std::vector<std::string> dlg_strings;
  base::SplitString(restart_info, '|', &dlg_strings);

  if (dlg_strings.size() < 3)
    return false;

  *title = base::ASCIIToUTF16(dlg_strings[0]);
  *message = base::ASCIIToUTF16(dlg_strings[1]);
  *is_rtl_locale = dlg_strings[2] == env_vars::kRtlLocale;
  return true;
}

bool ChromeBreakpadClient::AboutToRestart() {
  scoped_ptr<base::Environment> env(base::Environment::Create());
  if (!env->HasVar(env_vars::kRestartInfo))
    return false;

  env->SetVar(env_vars::kShowRestart, "1");
  return true;
}

base::string16 ChromeBreakpadClient::GetCrashGUID() {
  std::wstring guid;
  // GoogleUpdateSettings::GetMetricsId(&guid);
  return base::WideToUTF16(guid);
}

bool ChromeBreakpadClient::GetDeferredUploadsSupported(bool) {
  return false;
}

bool ChromeBreakpadClient::GetIsPerUserInstall(const base::FilePath& exe_path) {
  return false;
}

bool ChromeBreakpadClient::GetShouldDumpLargerDumps(bool is_per_user_install) {
  return true;
}

int ChromeBreakpadClient::GetResultCodeRespawnFailed() {
  return chrome::RESULT_CODE_RESPAWN_FAILED;
}
#endif

#if defined(OS_POSIX) && !defined(OS_MACOSX) && !defined(OS_IOS)
void ChromeBreakpadClient::GetProductNameAndVersion(std::string* product_name,
                                                    std::string* version) {
  DCHECK(product_name);
  DCHECK(version);
#if defined(OS_ANDROID)
  *product_name = "Chrome_Android";
#elif defined(OS_CHROMEOS)
  *product_name = "Chrome_ChromeOS";
#else  // OS_LINUX
#if !defined(ADDRESS_SANITIZER)
  *product_name = "Chrome_Linux";
#else
  *product_name = "Chrome_Linux_ASan";
#endif
#endif

  *version = NW_VERSION_STRING;
}

base::FilePath ChromeBreakpadClient::GetReporterLogFilename() {
  return base::FilePath(CrashUploadList::kReporterLogFilename);
}
#endif

bool ChromeBreakpadClient::GetCrashDumpLocation(base::FilePath* crash_dir) {
  // By setting the BREAKPAD_DUMP_LOCATION environment variable, an alternate
  // location to write breakpad crash dumps can be set.
  scoped_ptr<base::Environment> env(base::Environment::Create());
  std::string alternate_crash_dump_location;
  if (env->GetVar("BREAKPAD_DUMP_LOCATION", &alternate_crash_dump_location)) {
    base::FilePath crash_dumps_dir_path =
        base::FilePath::FromUTF8Unsafe(alternate_crash_dump_location);
    PathService::Override(chrome::DIR_CRASH_DUMPS, crash_dumps_dir_path);
  }

  return PathService::Get(chrome::DIR_CRASH_DUMPS, crash_dir);
}

#if defined(OS_POSIX)
void ChromeBreakpadClient::SetDumpWithoutCrashingFunction(void (*function)()) {
  logging::SetDumpWithoutCrashingFunction(function);
}
#endif

size_t ChromeBreakpadClient::RegisterCrashKeys() {
  return crash_keys::RegisterChromeCrashKeys();
}

bool ChromeBreakpadClient::IsRunningUnattended() {
  // scoped_ptr<base::Environment> env(base::Environment::Create());
  // return env->HasVar(env_vars::kHeadless);
  return true;
}

bool ChromeBreakpadClient::GetCollectStatsConsent() {
  return false;
}

#if defined(OS_ANDROID)
int ChromeBreakpadClient::GetAndroidMinidumpDescriptor() {
  return kAndroidMinidumpDescriptor;
}
#endif

}  // namespace chrome
