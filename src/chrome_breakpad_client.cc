// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/app/chrome_breakpad_client.h"

#include "base/atomicops.h"
#include "base/command_line.h"
#include "base/environment.h"
#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/path_service.h"
#include "base/strings/safe_sprintf.h"
#include "base/strings/string_split.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/common/chrome_constants.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/common/chrome_result_codes.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/common/crash_keys.h"
#include "chrome/common/env_vars.h"

#include "id/commit.h"

#if defined(OS_WIN)
#include <windows.h>

#include "base/file_version_info.h"
#include "base/win/registry.h"
#include "chrome/installer/util/google_chrome_sxs_distribution.h"
#include "chrome/installer/util/install_util.h"
#endif

#if defined(OS_POSIX) && !defined(OS_MACOSX) && !defined(OS_IOS)
#include "chrome/browser/crash_upload_list.h"
//#include "chrome/common/chrome_version_info_posix.h"
#include "content/nw/src/nw_version.h"
#endif

#if defined(OS_POSIX)
#include "base/debug/dump_without_crashing.h"
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

// The value name prefix will be of the form {chrome-version}-{pid}-{timestamp}
// (i.e., "#####.#####.#####.#####-########-########") which easily fits into a
// 63 character buffer.
const char kBrowserCrashDumpPrefixTemplate[] = "%s-%08x-%08x";
const size_t kBrowserCrashDumpPrefixLength = 63;
char g_browser_crash_dump_prefix[kBrowserCrashDumpPrefixLength + 1] = {};

// These registry key to which we'll write a value for each crash dump attempt.
HKEY g_browser_crash_dump_regkey = NULL;

// A atomic counter to make each crash dump value name unique.
base::subtle::Atomic32 g_browser_crash_dump_count = 0;
#endif

}  // namespace

ChromeBreakpadClient::ChromeBreakpadClient() {}

ChromeBreakpadClient::~ChromeBreakpadClient() {}

void ChromeBreakpadClient::SetBreakpadClientIdFromGUID(
    const std::string& client_guid) {
  crash_keys::SetCrashClientIdFromGUID(client_guid);
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

    *product_name = version_info->product_short_name();
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

void ChromeBreakpadClient::InitBrowserCrashDumpsRegKey() {
  DCHECK(g_browser_crash_dump_regkey == NULL);

  base::win::RegKey regkey;
  if (regkey.Create(HKEY_CURRENT_USER,
                    chrome::kBrowserCrashDumpAttemptsRegistryPath,
                    KEY_ALL_ACCESS) != ERROR_SUCCESS) {
    return;
  }

  // We use the current process id and the current tick count as a (hopefully)
  // unique combination for the crash dump value. There's a small chance that
  // across a reboot we might have a crash dump signal written, and the next
  // browser process might have the same process id and tick count, but crash
  // before consuming the signal (overwriting the signal with an identical one).
  // For now, we're willing to live with that risk.
  int length = base::strings::SafeSPrintf(g_browser_crash_dump_prefix,
                                          kBrowserCrashDumpPrefixTemplate,
                                          "chrome-version",
                                          ::GetCurrentProcessId(),
                                          ::GetTickCount());
  if (length <= 0) {
    NOTREACHED();
    g_browser_crash_dump_prefix[0] = '\0';
    return;
  }

  // Hold the registry key in a global for update on crash dump.
  g_browser_crash_dump_regkey = regkey.Take();
}

void ChromeBreakpadClient::RecordCrashDumpAttempt(bool is_real_crash) {
  // If we're not a browser (or the registry is unavailable to us for some
  // reason) then there's nothing to do.
  if (g_browser_crash_dump_regkey == NULL)
    return;

  // Generate the final value name we'll use (appends the crash number to the
  // base value name).
  const size_t kMaxValueSize = 2 * kBrowserCrashDumpPrefixLength;
  char value_name[kMaxValueSize + 1] = {};
  int length = base::strings::SafeSPrintf(
      value_name,
      "%s-%x",
      g_browser_crash_dump_prefix,
      base::subtle::NoBarrier_AtomicIncrement(&g_browser_crash_dump_count, 1));

  if (length > 0) {
    DWORD value_dword = is_real_crash ? 1 : 0;
    ::RegSetValueExA(g_browser_crash_dump_regkey, value_name, 0, REG_DWORD,
                     reinterpret_cast<BYTE*>(&value_dword),
                     sizeof(value_dword));
  }
}

bool ChromeBreakpadClient::ReportingIsEnforcedByPolicy(bool* breakpad_enabled) {
#if 0
// Determine whether configuration management allows loading the crash reporter.
// Since the configuration management infrastructure is not initialized at this
// point, we read the corresponding registry key directly. The return status
// indicates whether policy data was successfully read. If it is true,
// |breakpad_enabled| contains the value set by policy.
  string16 key_name = UTF8ToUTF16(policy::key::kMetricsReportingEnabled);
  DWORD value = 0;
  base::win::RegKey hklm_policy_key(HKEY_LOCAL_MACHINE,
                                    policy::kRegistryChromePolicyKey, KEY_READ);
  if (hklm_policy_key.ReadValueDW(key_name.c_str(), &value) == ERROR_SUCCESS) {
    *breakpad_enabled = value != 0;
    return true;
  }

  base::win::RegKey hkcu_policy_key(HKEY_CURRENT_USER,
                                    policy::kRegistryChromePolicyKey, KEY_READ);
  if (hkcu_policy_key.ReadValueDW(key_name.c_str(), &value) == ERROR_SUCCESS) {
    *breakpad_enabled = value != 0;
    return true;
  }

  return false;
#endif
  return true;
}
#endif

#if defined(OS_POSIX) && !defined(OS_MACOSX) && !defined(OS_IOS)
void ChromeBreakpadClient::GetProductNameAndVersion(std::string* product_name,
                                                    std::string* version) {
  DCHECK(product_name);
  DCHECK(version);

  *product_name = "node-webkit";

  *version = NW_VERSION_STRING " " NW_COMMIT_HASH;
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

bool ChromeBreakpadClient::EnableBreakpadForProcess(
    const std::string& process_type) {
  return process_type == switches::kRendererProcess ||
         process_type == switches::kPluginProcess ||
         process_type == switches::kPpapiPluginProcess ||
         process_type == switches::kZygoteProcess ||
         process_type == switches::kGpuProcess;
}

}  // namespace chrome
