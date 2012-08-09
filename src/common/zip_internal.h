// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_COMMON_ZIP_INTERNAL_H_
#define CHROME_COMMON_ZIP_INTERNAL_H_

#include <string>

#include "third_party/zlib/contrib/minizip/unzip.h"
#include "third_party/zlib/contrib/minizip/zip.h"

// Utility functions and constants used internally for the zip file
// library in the directory. Don't use them outside of the library.
namespace zip {
namespace internal {

// Opens the given file name in UTF-8 for unzipping, with some setup for
// Windows.
unzFile OpenForUnzipping(const std::string& file_name_utf8);

#if defined(OS_POSIX)
// Opens the file referred to by |zip_fd| for unzipping.
unzFile OpenFdForUnzipping(int zip_fd);
#endif

// Opens the given file name in UTF-8 for zipping, with some setup for
// Windows. |append_flag| will be passed to zipOpen2().
zipFile OpenForZipping(const std::string& file_name_utf8, int append_flag);

const int kZipMaxPath = 256;
const int kZipBufSize = 8192;

}  // namespace internal
}  // namespace zip

#endif  // CHROME_COMMON_ZIP_INTERNAL_H_
