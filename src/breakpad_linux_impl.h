// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Internal header file for the Linux breakpad implementation. This file is
// shared between crash_handler_host_linux.cc and breakpad_linux.cc. It should
// only be used in files compiled with linux_breakpad=1.

#ifndef CHROME_APP_BREAKPAD_LINUX_IMPL_H_
#define CHROME_APP_BREAKPAD_LINUX_IMPL_H_

#include <sys/types.h>

#include "base/basictypes.h"
#include "breakpad/src/common/simple_string_dictionary.h"
#include "content/nw/src/breakpad_linux.h"

typedef google_breakpad::NonAllocatingMap<256, 256, 64> CrashKeyStorage;

static const size_t kMaxActiveURLSize = 1024;
static const size_t kGuidSize = 32;  // 128 bits = 32 chars in hex.
static const size_t kDistroSize = 128;
#if defined(ADDRESS_SANITIZER)
static const size_t kMaxAsanReportSize = 1 << 16;
#endif
// Define a preferred limit on minidump sizes, because Crash Server currently
// throws away any larger than 1.2MB (1.2 * 1024 * 1024).  A value of -1 means
// no limit.
static const off_t kMaxMinidumpFileSize = 1258291;

// The size of the iovec used to transfer crash data from a child back to the
// browser.
#if !defined(ADDRESS_SANITIZER)
const size_t kCrashIovSize = 9;
#else
// Additional field to pass the AddressSanitizer log to the crash handler.
const size_t kCrashIovSize = 10;
#endif

// BreakpadInfo describes a crash report.
// The minidump information can either be contained in a file descriptor (fd) or
// in a file (whose path is in filename).
struct BreakpadInfo {
  int fd;                          // File descriptor to the Breakpad dump data.
  const char* filename;            // Path to the Breakpad dump data.
#if defined(ADDRESS_SANITIZER)
  const char* log_filename;        // Path to the ASan log file.
  const char* asan_report_str;     // ASan report.
  unsigned asan_report_length;     // Length of |asan_report_length|.
#endif
  const char* process_type;        // Process type, e.g. "renderer".
  unsigned process_type_length;    // Length of |process_type|.
  const char* crash_url;           // Active URL in the crashing process.
  unsigned crash_url_length;       // Length of |crash_url|.
  const char* guid;                // Client ID.
  unsigned guid_length;            // Length of |guid|.
  const char* distro;              // Linux distro string.
  unsigned distro_length;          // Length of |distro|.
  bool upload;                     // Whether to upload or save crash dump.
  uint64_t process_start_time;     // Uptime of the crashing process.
  size_t oom_size;                 // Amount of memory requested if OOM.
  uint64_t pid;                    // PID where applicable.
  CrashKeyStorage* crash_keys;
};

extern void HandleCrashDump(const BreakpadInfo& info);

#endif  // CHROME_APP_BREAKPAD_LINUX_IMPL_H_
