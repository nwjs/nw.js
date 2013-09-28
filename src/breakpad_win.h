// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_APP_BREAKPAD_WIN_H_
#define CHROME_APP_BREAKPAD_WIN_H_

#include <windows.h>
#include <string>
#include <vector>

namespace google_breakpad {

struct CustomInfoEntry;
}

namespace breakpad_win {

// A pointer to the custom entries that we send in the event of a crash. We need
// this pointer, along with the offsets into it below (and some private ones),
// so that we can keep the data updated as the state of the browser changes.
extern std::vector<google_breakpad::CustomInfoEntry>* g_custom_entries;

// These two are here because they are needed by breakpad_field_trial_win as
// well as breakpad_unittest_win.
extern size_t g_num_of_experiments_offset;
extern size_t g_experiment_chunks_offset;

}  // namespace breakpad_win

// The maximum number of 64-char URL chunks we will report.
static const int kMaxUrlChunks = 8;

void InitCrashReporter();

// Intercepts a crash but does not process it, just ask if we want to restart
// the browser or not.
void InitDefaultCrashCallback(LPTOP_LEVEL_EXCEPTION_FILTER filter);

// If chrome has been restarted because it crashed, this function will display
// a dialog asking for permission to continue execution or to exit now.
bool ShowRestartDialogIfCrashed(bool* exit_now);

// Helper to convert a vector of wstrings to corresponding vector of cstrings.
// Note that |cstrings| will reference memory owned by |wstrings|. Consequently
// |wstrings| must outlive |cstrings|, and |wstrings| should not be mutated.
void StringVectorToCStringVector(const std::vector<std::wstring>& wstrings,
                                 std::vector<const wchar_t*>* cstrings);

namespace testing {

// Testing entry point for calling a function from the unnamed namespace.
void InitCustomInfoEntries();

}

#endif  // CHROME_APP_BREAKPAD_WIN_H_
