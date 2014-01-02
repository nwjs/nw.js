// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/breakpad/app/breakpad_win.h"

#include <windows.h>
#include <shellapi.h>
#include <tchar.h>
#include <userenv.h>
#include <winnt.h>

#include <algorithm>
#include <vector>

#include "base/base_switches.h"
#include "base/basictypes.h"
#include "base/command_line.h"
#include "base/debug/crash_logging.h"
#include "base/environment.h"
#include "base/memory/scoped_ptr.h"
#include "base/strings/string16.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/win/metro.h"
#include "base/win/pe_image.h"
#include "base/win/registry.h"
#include "base/win/win_util.h"
#include "breakpad/src/client/windows/handler/exception_handler.h"
#include "components/breakpad/app/breakpad_client.h"
#include "components/breakpad/app/hard_error_handler_win.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/result_codes.h"
#include "sandbox/win/src/nt_internals.h"
#include "sandbox/win/src/sidestep/preamble_patcher.h"

// userenv.dll is required for GetProfileType().
#pragma comment(lib, "userenv.lib")

#pragma intrinsic(_AddressOfReturnAddress)
#pragma intrinsic(_ReturnAddress)

namespace breakpad {

// TODO(raymes): Modify the way custom crash info is stored. g_custom_entries
// is way too too fragile. See
// https://code.google.com/p/chromium/issues/detail?id=137062.
std::vector<google_breakpad::CustomInfoEntry>* g_custom_entries = NULL;
bool g_deferred_crash_uploads = false;

namespace {

// Minidump with stacks, PEB, TEB, and unloaded module list.
const MINIDUMP_TYPE kSmallDumpType = static_cast<MINIDUMP_TYPE>(
    MiniDumpWithProcessThreadData |  // Get PEB and TEB.
    MiniDumpWithUnloadedModules);  // Get unloaded modules when available.

// Minidump with all of the above, plus memory referenced from stack.
const MINIDUMP_TYPE kLargerDumpType = static_cast<MINIDUMP_TYPE>(
    MiniDumpWithProcessThreadData |  // Get PEB and TEB.
    MiniDumpWithUnloadedModules |  // Get unloaded modules when available.
    MiniDumpWithIndirectlyReferencedMemory);  // Get memory referenced by stack.

// Large dump with all process memory.
const MINIDUMP_TYPE kFullDumpType = static_cast<MINIDUMP_TYPE>(
    MiniDumpWithFullMemory |  // Full memory from process.
    MiniDumpWithProcessThreadData |  // Get PEB and TEB.
    MiniDumpWithHandleData |  // Get all handle information.
    MiniDumpWithUnloadedModules);  // Get unloaded modules when available.

const char kPipeNameVar[] = "CHROME_BREAKPAD_PIPE_NAME";

const wchar_t kGoogleUpdatePipeName[] = L"\\\\.\\pipe\\GoogleCrashServices\\";
const wchar_t kChromePipeName[] = L"\\\\.\\pipe\\ChromeCrashServices";

// This is the well known SID for the system principal.
const wchar_t kSystemPrincipalSid[] =L"S-1-5-18";

google_breakpad::ExceptionHandler* g_breakpad = NULL;
google_breakpad::ExceptionHandler* g_dumphandler_no_crash = NULL;

EXCEPTION_POINTERS g_surrogate_exception_pointers = {0};
EXCEPTION_RECORD g_surrogate_exception_record = {0};
CONTEXT g_surrogate_context = {0};

typedef NTSTATUS (WINAPI* NtTerminateProcessPtr)(HANDLE ProcessHandle,
                                                 NTSTATUS ExitStatus);
char* g_real_terminate_process_stub = NULL;

static size_t g_dynamic_keys_offset = 0;
typedef std::map<std::wstring, google_breakpad::CustomInfoEntry*>
    DynamicEntriesMap;
DynamicEntriesMap* g_dynamic_entries = NULL;
// Allow for 128 entries. POSIX uses 64 entries of 256 bytes, so Windows needs
// 256 entries of 64 bytes to match. See CustomInfoEntry::kValueMaxLength in
// Breakpad.
const size_t kMaxDynamicEntries = 256;

// Maximum length for plugin path to include in plugin crash reports.
const size_t kMaxPluginPathLength = 256;

// Dumps the current process memory.
extern "C" void __declspec(dllexport) __cdecl DumpProcess() {
  if (g_breakpad) {
    g_breakpad->WriteMinidump();
  }
}

// Used for dumping a process state when there is no crash.
extern "C" void __declspec(dllexport) __cdecl DumpProcessWithoutCrash() {
  if (g_dumphandler_no_crash) {
    g_dumphandler_no_crash->WriteMinidump();
  }
}

// We need to prevent ICF from folding DumpForHangDebuggingThread() and
// DumpProcessWithoutCrashThread() together, since that makes them
// indistinguishable in crash dumps. We do this by making the function
// bodies unique, and prevent optimization from shuffling things around.
MSVC_DISABLE_OPTIMIZE()
MSVC_PUSH_DISABLE_WARNING(4748)

DWORD WINAPI DumpProcessWithoutCrashThread(void*) {
  DumpProcessWithoutCrash();
  return 0;
}

// The following two functions do exactly the same thing as the two above. But
// we want the signatures to be different so that we can easily track them in
// crash reports.
// TODO(yzshen): Remove when enough information is collected and the hang rate
// of pepper/renderer processes is reduced.
DWORD WINAPI DumpForHangDebuggingThread(void*) {
  DumpProcessWithoutCrash();
  LOG(INFO) << "dumped for hang debugging";
  return 0;
}

MSVC_POP_WARNING()
MSVC_ENABLE_OPTIMIZE()

// Injects a thread into a remote process to dump state when there is no crash.
extern "C" HANDLE __declspec(dllexport) __cdecl
InjectDumpProcessWithoutCrash(HANDLE process) {
  return CreateRemoteThread(process, NULL, 0, DumpProcessWithoutCrashThread,
                            0, 0, NULL);
}

extern "C" HANDLE __declspec(dllexport) __cdecl
InjectDumpForHangDebugging(HANDLE process) {
  return CreateRemoteThread(process, NULL, 0, DumpForHangDebuggingThread,
                            0, 0, NULL);
}

extern "C" void DumpProcessAbnormalSignature() {
  if (!g_breakpad)
    return;
  g_custom_entries->push_back(
      google_breakpad::CustomInfoEntry(L"unusual-crash-signature", L""));
  g_breakpad->WriteMinidump();
}

// Reduces the size of the string |str| to a max of 64 chars. Required because
// breakpad's CustomInfoEntry raises an invalid_parameter error if the string
// we want to set is longer.
std::wstring TrimToBreakpadMax(const std::wstring& str) {
  std::wstring shorter(str);
  return shorter.substr(0,
      google_breakpad::CustomInfoEntry::kValueMaxLength - 1);
}

static void SetIntegerValue(size_t offset, int value) {
  if (!g_custom_entries)
    return;

  base::wcslcpy((*g_custom_entries)[offset].value,
                base::StringPrintf(L"%d", value).c_str(),
                google_breakpad::CustomInfoEntry::kValueMaxLength);
}

// Appends the plugin path to |g_custom_entries|.
void SetPluginPath(const std::wstring& path) {
  DCHECK(g_custom_entries);

  if (path.size() > kMaxPluginPathLength) {
    // If the path is too long, truncate from the start rather than the end,
    // since we want to be able to recover the DLL name.
    SetPluginPath(path.substr(path.size() - kMaxPluginPathLength));
    return;
  }

  // The chunk size without terminator.
  const size_t kChunkSize = static_cast<size_t>(
      google_breakpad::CustomInfoEntry::kValueMaxLength - 1);

  int chunk_index = 0;
  size_t chunk_start = 0;  // Current position inside |path|

  for (chunk_start = 0; chunk_start < path.size(); chunk_index++) {
    size_t chunk_length = std::min(kChunkSize, path.size() - chunk_start);

    g_custom_entries->push_back(google_breakpad::CustomInfoEntry(
        base::StringPrintf(L"plugin-path-chunk-%i", chunk_index + 1).c_str(),
        path.substr(chunk_start, chunk_length).c_str()));

    chunk_start += chunk_length;
  }
}

// Appends the breakpad dump path to |g_custom_entries|.
void SetBreakpadDumpPath() {
  DCHECK(g_custom_entries);
  base::FilePath crash_dumps_dir_path;
  if (GetBreakpadClient()->GetAlternativeCrashDumpLocation(
          &crash_dumps_dir_path)) {
    g_custom_entries->push_back(google_breakpad::CustomInfoEntry(
        L"breakpad-dump-location", crash_dumps_dir_path.value().c_str()));
  }
}

// Returns a string containing a list of all modifiers for the loaded profile.
std::wstring GetProfileType() {
  std::wstring profile_type;
  DWORD profile_bits = 0;
  if (::GetProfileType(&profile_bits)) {
    static const struct {
      DWORD bit;
      const wchar_t* name;
    } kBitNames[] = {
      { PT_MANDATORY, L"mandatory" },
      { PT_ROAMING, L"roaming" },
      { PT_TEMPORARY, L"temporary" },
    };
    for (size_t i = 0; i < arraysize(kBitNames); ++i) {
      const DWORD this_bit = kBitNames[i].bit;
      if ((profile_bits & this_bit) != 0) {
        profile_type.append(kBitNames[i].name);
        profile_bits &= ~this_bit;
        if (profile_bits != 0)
          profile_type.append(L", ");
      }
    }
  } else {
    DWORD last_error = ::GetLastError();
    base::SStringPrintf(&profile_type, L"error %u", last_error);
  }
  return profile_type;
}

// Returns the custom info structure based on the dll in parameter and the
// process type.
google_breakpad::CustomClientInfo* GetCustomInfo(const std::wstring& exe_path,
                                                 const std::wstring& type) {
  base::string16 version, product;
  base::string16 special_build;
  base::string16 channel_name;
  GetBreakpadClient()->GetProductNameAndVersion(
      base::FilePath(exe_path),
      &product,
      &version,
      &special_build,
      &channel_name);

  // We only expect this method to be called once per process.
  DCHECK(!g_custom_entries);
  g_custom_entries = new std::vector<google_breakpad::CustomInfoEntry>;

  // Common g_custom_entries.
  g_custom_entries->push_back(
      google_breakpad::CustomInfoEntry(L"ver", UTF16ToWide(version).c_str()));
  g_custom_entries->push_back(
      google_breakpad::CustomInfoEntry(L"prod", UTF16ToWide(product).c_str()));
  g_custom_entries->push_back(
      google_breakpad::CustomInfoEntry(L"plat", L"Win32"));
  g_custom_entries->push_back(
      google_breakpad::CustomInfoEntry(L"ptype", type.c_str()));
  g_custom_entries->push_back(google_breakpad::CustomInfoEntry(
      L"channel", base::UTF16ToWide(channel_name).c_str()));
  g_custom_entries->push_back(google_breakpad::CustomInfoEntry(
      L"profile-type", GetProfileType().c_str()));

  if (g_deferred_crash_uploads)
    g_custom_entries->push_back(
        google_breakpad::CustomInfoEntry(L"deferred-upload", L"true"));

  if (!special_build.empty())
    g_custom_entries->push_back(google_breakpad::CustomInfoEntry(
        L"special", UTF16ToWide(special_build).c_str()));

  if (type == L"plugin" || type == L"ppapi") {
    std::wstring plugin_path =
        CommandLine::ForCurrentProcess()->GetSwitchValueNative("plugin-path");
    if (!plugin_path.empty())
      SetPluginPath(plugin_path);
  }

  // Check whether configuration management controls crash reporting.
  bool crash_reporting_enabled = true;
  bool controlled_by_policy = GetBreakpadClient()->ReportingIsEnforcedByPolicy(
      &crash_reporting_enabled);
  const CommandLine& command = *CommandLine::ForCurrentProcess();
  bool use_crash_service =
      !controlled_by_policy && (command.HasSwitch(switches::kNoErrorDialogs) ||
                                GetBreakpadClient()->IsRunningUnattended());
  if (use_crash_service)
    SetBreakpadDumpPath();

  // Create space for dynamic ad-hoc keys. The names and values are set using
  // the API defined in base/debug/crash_logging.h.
  g_dynamic_keys_offset = g_custom_entries->size();
  for (size_t i = 0; i < kMaxDynamicEntries; ++i) {
    // The names will be mutated as they are set. Un-numbered since these are
    // merely placeholders. The name cannot be empty because Breakpad's
    // HTTPUpload will interpret that as an invalid parameter.
    g_custom_entries->push_back(
        google_breakpad::CustomInfoEntry(L"unspecified-crash-key", L""));
  }
  g_dynamic_entries = new DynamicEntriesMap;

  static google_breakpad::CustomClientInfo custom_client_info;
  custom_client_info.entries = &g_custom_entries->front();
  custom_client_info.count = g_custom_entries->size();

  return &custom_client_info;
}

// This callback is used when we want to get a dump without crashing the
// process.
bool DumpDoneCallbackWhenNoCrash(const wchar_t*, const wchar_t*, void*,
                                 EXCEPTION_POINTERS* ex_info,
                                 MDRawAssertionInfo*, bool) {
  return true;
}

// This callback is executed when the browser process has crashed, after
// the crash dump has been created. We need to minimize the amount of work
// done here since we have potentially corrupted process. Our job is to
// spawn another instance of chrome which will show a 'chrome has crashed'
// dialog. This code needs to live in the exe and thus has no access to
// facilities such as the i18n helpers.
bool DumpDoneCallback(const wchar_t*, const wchar_t*, void*,
                      EXCEPTION_POINTERS* ex_info,
                      MDRawAssertionInfo*, bool) {
  // Check if the exception is one of the kind which would not be solved
  // by simply restarting chrome. In this case we show a message box with
  // and exit silently. Remember that chrome is in a crashed state so we
  // can't show our own UI from this process.
  if (HardErrorHandler(ex_info))
    return true;

  if (!GetBreakpadClient()->AboutToRestart())
    return true;

  // Now we just start chrome browser with the same command line.
  STARTUPINFOW si = {sizeof(si)};
  PROCESS_INFORMATION pi;
  if (::CreateProcessW(NULL, ::GetCommandLineW(), NULL, NULL, FALSE,
                       CREATE_UNICODE_ENVIRONMENT, NULL, NULL, &si, &pi)) {
    ::CloseHandle(pi.hProcess);
    ::CloseHandle(pi.hThread);
  }
  // After this return we will be terminated. The actual return value is
  // not used at all.
  return true;
}

// flag to indicate that we are already handling an exception.
volatile LONG handling_exception = 0;

// This callback is used when there is no crash. Note: Unlike the
// |FilterCallback| below this does not do dupe detection. It is upto the caller
// to implement it.
bool FilterCallbackWhenNoCrash(
    void*, EXCEPTION_POINTERS*, MDRawAssertionInfo*) {
  GetBreakpadClient()->RecordCrashDumpAttempt(false);
  return true;
}

// This callback is executed when the Chrome process has crashed and *before*
// the crash dump is created. To prevent duplicate crash reports we
// make every thread calling this method, except the very first one,
// go to sleep.
bool FilterCallback(void*, EXCEPTION_POINTERS*, MDRawAssertionInfo*) {
  // Capture every thread except the first one in the sleep. We don't
  // want multiple threads to concurrently report exceptions.
  if (::InterlockedCompareExchange(&handling_exception, 1, 0) == 1) {
    ::Sleep(INFINITE);
  }
  GetBreakpadClient()->RecordCrashDumpAttempt(true);
  return true;
}

// Previous unhandled filter. Will be called if not null when we
// intercept a crash.
LPTOP_LEVEL_EXCEPTION_FILTER previous_filter = NULL;

// Exception filter used when breakpad is not enabled. We just display
// the "Do you want to restart" message and then we call the previous filter.
long WINAPI ChromeExceptionFilter(EXCEPTION_POINTERS* info) {
  DumpDoneCallback(NULL, NULL, NULL, info, NULL, false);

  if (previous_filter)
    return previous_filter(info);

  return EXCEPTION_EXECUTE_HANDLER;
}

// Exception filter for the service process used when breakpad is not enabled.
// We just display the "Do you want to restart" message and then die
// (without calling the previous filter).
long WINAPI ServiceExceptionFilter(EXCEPTION_POINTERS* info) {
  DumpDoneCallback(NULL, NULL, NULL, info, NULL, false);
  return EXCEPTION_EXECUTE_HANDLER;
}

// NOTE: This function is used by SyzyASAN to annotate crash reports. If you
// change the name or signature of this function you will break SyzyASAN
// instrumented releases of Chrome. Please contact syzygy-team@chromium.org
// before doing so!
extern "C" void __declspec(dllexport) __cdecl SetCrashKeyValueImpl(
    const wchar_t* key, const wchar_t* value) {
  if (!g_dynamic_entries)
    return;

  // CustomInfoEntry limits the length of key and value. If they exceed
  // their maximum length the underlying string handling functions raise
  // an exception and prematurely trigger a crash. Truncate here.
  std::wstring safe_key(std::wstring(key).substr(
      0, google_breakpad::CustomInfoEntry::kNameMaxLength  - 1));
  std::wstring safe_value(std::wstring(value).substr(
      0, google_breakpad::CustomInfoEntry::kValueMaxLength - 1));

  // If we already have a value for this key, update it; otherwise, insert
  // the new value if we have not exhausted the pre-allocated slots for dynamic
  // entries.
  DynamicEntriesMap::iterator it = g_dynamic_entries->find(safe_key);
  google_breakpad::CustomInfoEntry* entry = NULL;
  if (it == g_dynamic_entries->end()) {
    if (g_dynamic_entries->size() >= kMaxDynamicEntries)
      return;
    entry = &(*g_custom_entries)[g_dynamic_keys_offset++];
    g_dynamic_entries->insert(std::make_pair(safe_key, entry));
  } else {
    entry = it->second;
  }

  entry->set(safe_key.data(), safe_value.data());
}

extern "C" void __declspec(dllexport) __cdecl ClearCrashKeyValueImpl(
    const wchar_t* key) {
  if (!g_dynamic_entries)
    return;

  std::wstring key_string(key);
  DynamicEntriesMap::iterator it = g_dynamic_entries->find(key_string);
  if (it == g_dynamic_entries->end())
    return;

  it->second->set_value(NULL);
}

}  // namespace

bool WrapMessageBoxWithSEH(const wchar_t* text, const wchar_t* caption,
                           UINT flags, bool* exit_now) {
  // We wrap the call to MessageBoxW with a SEH handler because it some
  // machines with CursorXP, PeaDict or with FontExplorer installed it crashes
  // uncontrollably here. Being this a best effort deal we better go away.
  __try {
    *exit_now = (IDOK != ::MessageBoxW(NULL, text, caption, flags));
  } __except(EXCEPTION_EXECUTE_HANDLER) {
    // Its not safe to continue executing, exit silently here.
    ::TerminateProcess(::GetCurrentProcess(),
                       GetBreakpadClient()->GetResultCodeRespawnFailed());
  }

  return true;
}

// This function is executed by the child process that DumpDoneCallback()
// spawned and basically just shows the 'chrome has crashed' dialog if
// the CHROME_CRASHED environment variable is present.
bool ShowRestartDialogIfCrashed(bool* exit_now) {
  // If we are being launched in metro mode don't try to show the dialog.
  if (base::win::IsMetroProcess())
    return false;

  // Only show this for the browser process. See crbug.com/132119.
  const CommandLine& command_line = *CommandLine::ForCurrentProcess();
  std::string process_type =
      command_line.GetSwitchValueASCII(switches::kProcessType);
  if (!process_type.empty())
    return false;

  base::string16 message;
  base::string16 title;
  bool is_rtl_locale;
  if (!GetBreakpadClient()->ShouldShowRestartDialog(
           &title, &message, &is_rtl_locale)) {
    return false;
  }

  // If the UI layout is right-to-left, we need to pass the appropriate MB_XXX
  // flags so that an RTL message box is displayed.
  UINT flags = MB_OKCANCEL | MB_ICONWARNING;
  if (is_rtl_locale)
    flags |= MB_RIGHT | MB_RTLREADING;

  return WrapMessageBoxWithSEH(base::UTF16ToWide(message).c_str(),
                               base::UTF16ToWide(title).c_str(),
                               flags,
                               exit_now);
}

// Crashes the process after generating a dump for the provided exception. Note
// that the crash reporter should be initialized before calling this function
// for it to do anything.
// NOTE: This function is used by SyzyASAN to invoke a crash. If you change the
// the name or signature of this function you will break SyzyASAN instrumented
// releases of Chrome. Please contact syzygy-team@chromium.org before doing so!
extern "C" int __declspec(dllexport) CrashForException(
    EXCEPTION_POINTERS* info) {
  if (g_breakpad) {
    g_breakpad->WriteMinidumpForException(info);
    // Patched stub exists based on conditions (See InitCrashReporter).
    // As a side note this function also gets called from
    // WindowProcExceptionFilter.
    if (g_real_terminate_process_stub == NULL) {
      ::TerminateProcess(::GetCurrentProcess(), content::RESULT_CODE_KILLED);
    } else {
      NtTerminateProcessPtr real_terminate_proc =
          reinterpret_cast<NtTerminateProcessPtr>(
              static_cast<char*>(g_real_terminate_process_stub));
      real_terminate_proc(::GetCurrentProcess(), content::RESULT_CODE_KILLED);
    }
  }
  return EXCEPTION_CONTINUE_SEARCH;
}

NTSTATUS WINAPI HookNtTerminateProcess(HANDLE ProcessHandle,
                                       NTSTATUS ExitStatus) {
  if (g_breakpad &&
      (ProcessHandle == ::GetCurrentProcess() || ProcessHandle == NULL)) {
    NT_TIB* tib = reinterpret_cast<NT_TIB*>(NtCurrentTeb());
    void* address_on_stack = _AddressOfReturnAddress();
    if (address_on_stack < tib->StackLimit ||
        address_on_stack > tib->StackBase) {
      g_surrogate_exception_record.ExceptionAddress = _ReturnAddress();
      g_surrogate_exception_record.ExceptionCode = DBG_TERMINATE_PROCESS;
      g_surrogate_exception_record.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
      CrashForException(&g_surrogate_exception_pointers);
    }
  }

  NtTerminateProcessPtr real_proc =
      reinterpret_cast<NtTerminateProcessPtr>(
          static_cast<char*>(g_real_terminate_process_stub));
  return real_proc(ProcessHandle, ExitStatus);
}

static void InitTerminateProcessHooks() {
  NtTerminateProcessPtr terminate_process_func_address =
      reinterpret_cast<NtTerminateProcessPtr>(::GetProcAddress(
          ::GetModuleHandle(L"ntdll.dll"), "NtTerminateProcess"));
  if (terminate_process_func_address == NULL)
    return;

  DWORD old_protect = 0;
  if (!::VirtualProtect(terminate_process_func_address, 5,
                        PAGE_EXECUTE_READWRITE, &old_protect))
    return;

  g_real_terminate_process_stub = reinterpret_cast<char*>(VirtualAllocEx(
      ::GetCurrentProcess(), NULL, sidestep::kMaxPreambleStubSize,
      MEM_COMMIT, PAGE_EXECUTE_READWRITE));
  if (g_real_terminate_process_stub == NULL)
    return;

  g_surrogate_exception_pointers.ContextRecord = &g_surrogate_context;
  g_surrogate_exception_pointers.ExceptionRecord =
      &g_surrogate_exception_record;

  sidestep::SideStepError patch_result =
      sidestep::PreamblePatcher::Patch(
          terminate_process_func_address, HookNtTerminateProcess,
          g_real_terminate_process_stub, sidestep::kMaxPreambleStubSize);
  if (patch_result != sidestep::SIDESTEP_SUCCESS) {
    CHECK(::VirtualFreeEx(::GetCurrentProcess(), g_real_terminate_process_stub,
                    0, MEM_RELEASE));
    CHECK(::VirtualProtect(terminate_process_func_address, 5, old_protect,
                           &old_protect));
    return;
  }

  DWORD dummy = 0;
  CHECK(::VirtualProtect(terminate_process_func_address,
                         5,
                         old_protect,
                         &dummy));
  CHECK(::VirtualProtect(g_real_terminate_process_stub,
                         sidestep::kMaxPreambleStubSize,
                         old_protect,
                         &old_protect));
}

static void InitPipeNameEnvVar(bool is_per_user_install) {
  scoped_ptr<base::Environment> env(base::Environment::Create());
  if (env->HasVar(kPipeNameVar)) {
    // The Breakpad pipe name is already configured: nothing to do.
    return;
  }

  // Check whether configuration management controls crash reporting.
  bool crash_reporting_enabled = true;
  bool controlled_by_policy = GetBreakpadClient()->ReportingIsEnforcedByPolicy(
      &crash_reporting_enabled);

  const CommandLine& command = *CommandLine::ForCurrentProcess();
  bool use_crash_service =
      !controlled_by_policy && (command.HasSwitch(switches::kNoErrorDialogs) ||
                                GetBreakpadClient()->IsRunningUnattended());

  std::wstring pipe_name;
  if (use_crash_service) {
    // Crash reporting is done by crash_service.exe.
    pipe_name = kChromePipeName;
  } else {
    // We want to use the Google Update crash reporting. We need to check if the
    // user allows it first (in case the administrator didn't already decide
    // via policy).
    if (!controlled_by_policy)
      crash_reporting_enabled = GetBreakpadClient()->GetCollectStatsConsent();

    if (!crash_reporting_enabled) {
      if (!controlled_by_policy &&
          GetBreakpadClient()->GetDeferredUploadsSupported(
              is_per_user_install)) {
        g_deferred_crash_uploads = true;
      } else {
        return;
      }
    }

    // Build the pipe name. It can be either:
    // System-wide install: "NamedPipe\GoogleCrashServices\S-1-5-18"
    // Per-user install: "NamedPipe\GoogleCrashServices\<user SID>"
    std::wstring user_sid;
    if (is_per_user_install) {
      if (!base::win::GetUserSidString(&user_sid)) {
        return;
      }
    } else {
      user_sid = kSystemPrincipalSid;
    }

    pipe_name = kGoogleUpdatePipeName;
    pipe_name += user_sid;
  }
  env->SetVar(kPipeNameVar, WideToASCII(pipe_name));
}

void InitDefaultCrashCallback(LPTOP_LEVEL_EXCEPTION_FILTER filter) {
  previous_filter = SetUnhandledExceptionFilter(filter);
}

void InitCrashReporter() {
  const CommandLine& command = *CommandLine::ForCurrentProcess();
  if (command.HasSwitch(switches::kDisableBreakpad))
    return;

  // Disable the message box for assertions.
  _CrtSetReportMode(_CRT_ASSERT, 0);

  std::wstring process_type =
    command.GetSwitchValueNative(switches::kProcessType);
  if (process_type.empty())
    process_type = L"browser";

  wchar_t exe_path[MAX_PATH];
  exe_path[0] = 0;
  GetModuleFileNameW(NULL, exe_path, MAX_PATH);

  bool is_per_user_install =
      GetBreakpadClient()->GetIsPerUserInstall(base::FilePath(exe_path));

  google_breakpad::CustomClientInfo* custom_info =
      GetCustomInfo(exe_path, process_type);

  google_breakpad::ExceptionHandler::MinidumpCallback callback = NULL;
  LPTOP_LEVEL_EXCEPTION_FILTER default_filter = NULL;
  // We install the post-dump callback only for the browser and service
  // processes. It spawns a new browser/service process.
  if (process_type == L"browser") {
    callback = &DumpDoneCallback;
    default_filter = &ChromeExceptionFilter;
  } else if (process_type == L"service") {
    callback = &DumpDoneCallback;
    default_filter = &ServiceExceptionFilter;
  }

  if (process_type == L"browser") {
    InitPipeNameEnvVar(is_per_user_install);
    GetBreakpadClient()->InitBrowserCrashDumpsRegKey();
  }

  scoped_ptr<base::Environment> env(base::Environment::Create());
  std::string pipe_name_ascii;
  if (!env->GetVar(kPipeNameVar, &pipe_name_ascii)) {
    // Breakpad is not enabled.  Configuration is managed or the user
    // did not allow Google Update to send crashes.  We need to use
    // our default crash handler instead, but only for the
    // browser/service processes.
    if (default_filter)
      InitDefaultCrashCallback(default_filter);
    return;
  }
  std::wstring pipe_name = ASCIIToWide(pipe_name_ascii);

#ifdef _WIN64
  // The protocol for connecting to the out-of-process Breakpad crash
  // reporter is different for x86-32 and x86-64: the message sizes
  // are different because the message struct contains a pointer.  As
  // a result, there are two different named pipes to connect to.  The
  // 64-bit one is distinguished with an "-x64" suffix.
  pipe_name += L"-x64";
#endif

  // Get the alternate dump directory. We use the temp path.
  wchar_t temp_dir[MAX_PATH] = {0};
  ::GetTempPathW(MAX_PATH, temp_dir);

  MINIDUMP_TYPE dump_type = kSmallDumpType;
  // Capture full memory if explicitly instructed to.
  if (command.HasSwitch(switches::kFullMemoryCrashReport))
    dump_type = kFullDumpType;
  else if (GetBreakpadClient()->GetShouldDumpLargerDumps(is_per_user_install))
    dump_type = kLargerDumpType;

  g_breakpad = new google_breakpad::ExceptionHandler(temp_dir, &FilterCallback,
                   callback, NULL,
                   google_breakpad::ExceptionHandler::HANDLER_ALL,
                   dump_type, pipe_name.c_str(), custom_info);

  // Now initialize the non crash dump handler.
  g_dumphandler_no_crash = new google_breakpad::ExceptionHandler(temp_dir,
      &FilterCallbackWhenNoCrash,
      &DumpDoneCallbackWhenNoCrash,
      NULL,
      // Set the handler to none so this handler would not be added to
      // |handler_stack_| in |ExceptionHandler| which is a list of exception
      // handlers.
      google_breakpad::ExceptionHandler::HANDLER_NONE,
      dump_type, pipe_name.c_str(), custom_info);

  if (g_breakpad->IsOutOfProcess()) {
    // Tells breakpad to handle breakpoint and single step exceptions.
    // This might break JIT debuggers, but at least it will always
    // generate a crashdump for these exceptions.
    g_breakpad->set_handle_debug_exceptions(true);

#ifndef _WIN64
    if (process_type != L"browser" &&
        !GetBreakpadClient()->IsRunningUnattended()) {
      // Initialize the hook TerminateProcess to catch unexpected exits.
      InitTerminateProcessHooks();
    }
#endif
  }
}

}  // namespace breakpad
