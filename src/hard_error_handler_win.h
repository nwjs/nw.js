// Copyright (c) 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_APP_HARD_ERROR_HANDLER_WIN_H_
#define CHROME_APP_HARD_ERROR_HANDLER_WIN_H_

#include <windows.h>

// This function is in charge of displaying a dialog box that informs the
// user of a fatal condition in chrome. It is meant to be called from
// breakpad's unhandled exception handler after the crash dump has been
// created. The return value will be true if we are to retry launching
// chrome (and show the 'chrome has crashed' dialog) or to silently exit.
//
// This function only handles a few known exceptions, currently:
// - Failure to load a delayload dll.
// - Failure to bind to a delayloaded import.
// - Fatal Graphics card failure  (aura build only).
//
// If any of these conditions are encountered, a message box shown by
// the operating system CSRSS process via NtRaiseHardError is invoked.
// The wording and localization is up to the operating system.
//
// Do not call this function for memory related errors like heap corruption
// or stack exahustion. This function assumes that memory allocations are
// possible.
bool HardErrorHandler(EXCEPTION_POINTERS* ex_info);

#endif  // CHROME_APP_HARD_ERROR_HANDLER_WIN_H_

