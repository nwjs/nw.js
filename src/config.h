// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_CONFIG_H_
#define CONTENT_SHELL_CONFIG_H_

// Required to build WebTestingSupport.

// To avoid confict of LOG in wtf/Assertions.h and LOG in base/logging.h,
// skip base/logging.h by defining BASE_LOGGING_H_ and define some macros
// provided by base/logging.h.
// FIXME: Remove this hack!
#include <ostream>
#define BASE_LOGGING_H_
#define CHECK(condition) while (false && (condition)) std::cerr
#define DCHECK(condition) while (false && (condition)) std::cerr
#define DCHECK_EQ(a, b) while (false && (a) == (b)) std::cerr
#define DCHECK_NE(a, b) while (false && (a) != (b)) std::cerr

#include <wtf/Platform.h>
#include <wtf/ExportMacros.h>

#if OS(WINDOWS) && !COMPILER(GCC)
// Allow 'this' to be used in base member initializer list.
#pragma warning(disable : 4355)
#endif

#endif  // CONTENT_SHELL_CONFIG_H_
