// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "nw/common/zip.h"

#include "base/bind.h"
#include "base/file_util.h"
#include "base/logging.h"
#include "base/string16.h"
#include "base/string_util.h"
#include "nw/common/zip_internal.h"
#include "nw/common/zip_reader.h"
#include "net/base/file_stream.h"
#include "third_party/zlib/contrib/minizip/unzip.h"
#include "third_party/zlib/contrib/minizip/zip.h"

namespace {

bool AddFileToZip(zipFile zip_file, const FilePath& src_dir) {
  net::FileStream stream(NULL);
  int flags = base::PLATFORM_FILE_OPEN | base::PLATFORM_FILE_READ;
  if (stream.OpenSync(src_dir, flags) != 0) {
    DLOG(ERROR) << "Could not open stream for path "
                << src_dir.value();
    return false;
  }

  int num_bytes;
  char buf[zip::internal::kZipBufSize];
  do {
    num_bytes = stream.ReadSync(buf, zip::internal::kZipBufSize);
    if (num_bytes > 0) {
      if (ZIP_OK != zipWriteInFileInZip(zip_file, buf, num_bytes)) {
        DLOG(ERROR) << "Could not write data to zip for path "
                    << src_dir.value();
        return false;
      }
    }
  } while (num_bytes > 0);

  return true;
}

bool AddEntryToZip(zipFile zip_file, const FilePath& path,
                   const FilePath& root_path) {
  std::string str_path =
      path.AsUTF8Unsafe().substr(root_path.value().length() + 1);
#if defined(OS_WIN)
  ReplaceSubstringsAfterOffset(&str_path, 0u, "\\", "/");
#endif

  bool is_directory = file_util::DirectoryExists(path);
  if (is_directory)
    str_path += "/";

  if (ZIP_OK != zipOpenNewFileInZip(
      zip_file, str_path.c_str(),
      NULL, NULL, 0u, NULL, 0u, NULL,  // file info, extrafield local, length,
                                       // extrafield global, length, comment
      Z_DEFLATED, Z_DEFAULT_COMPRESSION)) {
    DLOG(ERROR) << "Could not open zip file entry " << str_path;
    return false;
  }

  bool success = true;
  if (!is_directory) {
    success = AddFileToZip(zip_file, path);
  }

  if (ZIP_OK != zipCloseFileInZip(zip_file)) {
    DLOG(ERROR) << "Could not close zip file entry " << str_path;
    return false;
  }

  return success;
}

bool ExcludeNoFilesFilter(const FilePath& file_path) {
  return true;
}

bool ExcludeHiddenFilesFilter(const FilePath& file_path) {
  return file_path.BaseName().value()[0] != '.';
}

}  // namespace

namespace zip {

bool Unzip(const FilePath& src_file, const FilePath& dest_dir) {
  ZipReader reader;
  if (!reader.Open(src_file)) {
    DLOG(WARNING) << "Failed to open " << src_file.value();
    return false;
  }
  while (reader.HasMore()) {
    if (!reader.OpenCurrentEntryInZip()) {
      DLOG(WARNING) << "Failed to open the current file in zip";
      return false;
    }
    if (reader.current_entry_info()->is_unsafe()) {
      DLOG(WARNING) << "Found an unsafe file in zip "
                    << reader.current_entry_info()->file_path().value();
      return false;
    }
    if (!reader.ExtractCurrentEntryIntoDirectory(dest_dir)) {
      DLOG(WARNING) << "Failed to extract "
                    << reader.current_entry_info()->file_path().value();
      return false;
    }
    if (!reader.AdvanceToNextEntry()) {
      DLOG(WARNING) << "Failed to advance to the next file";
      return false;
    }
  }
  return true;
}

bool ZipWithFilterCallback(const FilePath& src_dir, const FilePath& dest_file,
                           const FilterCallback& filter_cb) {
  DCHECK(file_util::DirectoryExists(src_dir));

  zipFile zip_file = internal::OpenForZipping(dest_file.AsUTF8Unsafe(),
                                              APPEND_STATUS_CREATE);

  if (!zip_file) {
    DLOG(WARNING) << "couldn't create file " << dest_file.value();
    return false;
  }

  bool success = true;
  file_util::FileEnumerator file_enumerator(
      src_dir, true,  // recursive
      static_cast<file_util::FileEnumerator::FileType>(
          file_util::FileEnumerator::FILES |
          file_util::FileEnumerator::DIRECTORIES));
  for (FilePath path = file_enumerator.Next(); !path.value().empty();
       path = file_enumerator.Next()) {
    if (!filter_cb.Run(path)) {
      continue;
    }

    if (!AddEntryToZip(zip_file, path, src_dir)) {
      success = false;
      return false;
    }
  }

  if (ZIP_OK != zipClose(zip_file, NULL)) {  // global comment
    DLOG(ERROR) << "Error closing zip file " << dest_file.value();
    return false;
  }

  return success;
}

bool Zip(const FilePath& src_dir, const FilePath& dest_file,
         bool include_hidden_files) {
  if (include_hidden_files) {
    return ZipWithFilterCallback(
        src_dir, dest_file, base::Bind(&ExcludeNoFilesFilter));
  } else {
    return ZipWithFilterCallback(
        src_dir, dest_file, base::Bind(&ExcludeHiddenFilesFilter));
  }
}

}  // namespace zip
