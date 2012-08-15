// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "zip_reader.h"

#include "base/file_util.h"
#include "base/logging.h"
#include "base/string_util.h"
#include "base/utf_string_conversions.h"
#include "net/base/file_stream.h"
#include "third_party/zlib/contrib/minizip/unzip.h"
#if defined(OS_WIN)
#include "third_party/zlib/contrib/minizip/iowin32.h"
#endif
#include "zip_internal.h"

namespace zip {

// TODO(satorux): The implementation assumes that file names in zip files
// are encoded in UTF-8. This is true for zip files created by Zip()
// function in zip.h, but not true for user-supplied random zip files.
ZipReader::EntryInfo::EntryInfo(const std::string& file_name_in_zip,
                                const unz_file_info& raw_file_info)
    : file_path_(FilePath::FromUTF8Unsafe(file_name_in_zip)),
      is_directory_(false) {
  original_size_ = raw_file_info.uncompressed_size;

  // Directory entries in zip files end with "/".
  is_directory_ = EndsWith(file_name_in_zip, "/", false);

  // Check the file name here for directory traversal issues. In the name of
  // simplicity and security, we might reject a valid file name such as "a..b".
  is_unsafe_ = file_name_in_zip.find("..") != std::string::npos;

  // We also consider that the file name is unsafe, if it's invalid UTF-8.
  string16 file_name_utf16;
  if (!UTF8ToUTF16(file_name_in_zip.data(), file_name_in_zip.size(),
                   &file_name_utf16)) {
    is_unsafe_ = true;
  }

  // We also consider that the file name is unsafe, if it's absolute.
  // On Windows, IsAbsolute() returns false for paths starting with "/".
  if (file_path_.IsAbsolute() || StartsWithASCII(file_name_in_zip, "/", false))
    is_unsafe_ = true;

  // Construct the last modified time. The timezone info is not present in
  // zip files, so we construct the time as local time.
  base::Time::Exploded exploded_time = {};  // Zero-clear.
  exploded_time.year = raw_file_info.tmu_date.tm_year;
  // The month in zip file is 0-based, whereas ours is 1-based.
  exploded_time.month = raw_file_info.tmu_date.tm_mon + 1;
  exploded_time.day_of_month = raw_file_info.tmu_date.tm_mday;
  exploded_time.hour = raw_file_info.tmu_date.tm_hour;
  exploded_time.minute = raw_file_info.tmu_date.tm_min;
  exploded_time.second = raw_file_info.tmu_date.tm_sec;
  exploded_time.millisecond = 0;
  if (exploded_time.HasValidValues()) {
    last_modified_ = base::Time::FromLocalExploded(exploded_time);
  } else {
    // Use Unix time epoch if the time stamp data is invalid.
    last_modified_ = base::Time::UnixEpoch();
  }
}

ZipReader::ZipReader() {
  Reset();
}

ZipReader::~ZipReader() {
  Close();
}

bool ZipReader::Open(const FilePath& zip_file_path) {
  DCHECK(!zip_file_);

  // Use of "Unsafe" function does not look good, but there is no way to do
  // this safely on Linux. See file_util.h for details.
  zip_file_ = internal::OpenForUnzipping(zip_file_path.AsUTF8Unsafe());
  if (!zip_file_) {
    return false;
  }

  return OpenInternal();
}

#if defined(OS_POSIX)
bool ZipReader::OpenFromFd(const int zip_fd) {
  DCHECK(!zip_file_);

  zip_file_ = internal::OpenFdForUnzipping(zip_fd);
  if (!zip_file_) {
    return false;
  }

  return OpenInternal();
}
#endif

void ZipReader::Close() {
  if (zip_file_) {
    unzClose(zip_file_);
  }
  Reset();
}

bool ZipReader::HasMore() {
  return !reached_end_;
}

bool ZipReader::AdvanceToNextEntry() {
  DCHECK(zip_file_);

  // Should not go further if we already reached the end.
  if (reached_end_)
    return false;

  unz_file_pos position = {};
  if (unzGetFilePos(zip_file_, &position) != UNZ_OK)
    return false;
  const int current_entry_index = position.num_of_file;
  // If we are currently at the last entry, then the next position is the
  // end of the zip file, so mark that we reached the end.
  if (current_entry_index + 1 == num_entries_) {
    reached_end_ = true;
  } else {
    DCHECK_LT(current_entry_index + 1, num_entries_);
    if (unzGoToNextFile(zip_file_) != UNZ_OK) {
      return false;
    }
  }
  current_entry_info_.reset();
  return true;
}

bool ZipReader::OpenCurrentEntryInZip() {
  DCHECK(zip_file_);

  unz_file_info raw_file_info = {};
  char raw_file_name_in_zip[internal::kZipMaxPath] = {};
  const int result = unzGetCurrentFileInfo(zip_file_,
                                           &raw_file_info,
                                           raw_file_name_in_zip,
                                           sizeof(raw_file_name_in_zip) - 1,
                                           NULL,  // extraField.
                                           0,  // extraFieldBufferSize.
                                           NULL,  // szComment.
                                           0);  // commentBufferSize.
  if (result != UNZ_OK)
    return false;
  if (raw_file_name_in_zip[0] == '\0')
    return false;
  current_entry_info_.reset(
      new EntryInfo(raw_file_name_in_zip, raw_file_info));
  return true;
}

bool ZipReader::LocateAndOpenEntry(const FilePath& path_in_zip) {
  DCHECK(zip_file_);

  current_entry_info_.reset();
  reached_end_ = false;
  const int kDefaultCaseSensivityOfOS = 0;
  const int result = unzLocateFile(zip_file_,
                                   path_in_zip.AsUTF8Unsafe().c_str(),
                                   kDefaultCaseSensivityOfOS);
  if (result != UNZ_OK)
    return false;

  // Then Open the entry.
  return OpenCurrentEntryInZip();
}

bool ZipReader::ExtractCurrentEntryToFilePath(
    const FilePath& output_file_path) {
  DCHECK(zip_file_);

  // If this is a directory, just create it and return.
  if (current_entry_info()->is_directory())
    return file_util::CreateDirectory(output_file_path);

  const int open_result = unzOpenCurrentFile(zip_file_);
  if (open_result != UNZ_OK)
    return false;

  // We can't rely on parent directory entries being specified in the
  // zip, so we make sure they are created.
  FilePath output_dir_path = output_file_path.DirName();
  if (!file_util::CreateDirectory(output_dir_path))
    return false;

  net::FileStream stream(NULL);
  const int flags = (base::PLATFORM_FILE_CREATE_ALWAYS |
                     base::PLATFORM_FILE_WRITE);
  if (stream.OpenSync(output_file_path, flags) != 0)
    return false;

  bool success = true;  // This becomes false when something bad happens.
  while (true) {
    char buf[internal::kZipBufSize];
    const int num_bytes_read = unzReadCurrentFile(zip_file_, buf,
                                                  internal::kZipBufSize);
    if (num_bytes_read == 0) {
      // Reached the end of the file.
      break;
    } else if (num_bytes_read < 0) {
      // If num_bytes_read < 0, then it's a specific UNZ_* error code.
      success = false;
      break;
    } else if (num_bytes_read > 0) {
      // Some data is read. Write it to the output file.
      if (num_bytes_read != stream.WriteSync(buf, num_bytes_read)) {
        success = false;
        break;
      }
    }
  }

  stream.CloseSync();
  unzCloseCurrentFile(zip_file_);
  return success;
}

bool ZipReader::ExtractCurrentEntryIntoDirectory(
    const FilePath& output_directory_path) {
  DCHECK(current_entry_info_.get());

  FilePath output_file_path = output_directory_path.Append(
      current_entry_info()->file_path());
  return ExtractCurrentEntryToFilePath(output_file_path);
}

#if defined(OS_POSIX)
bool ZipReader::ExtractCurrentEntryToFd(const int fd) {
  DCHECK(zip_file_);

  // If this is a directory, there's nothing to extract to the file descriptor,
  // so return false.
  if (current_entry_info()->is_directory())
    return false;

  const int open_result = unzOpenCurrentFile(zip_file_);
  if (open_result != UNZ_OK)
    return false;

  bool success = true;  // This becomes false when something bad happens.
  while (true) {
    char buf[internal::kZipBufSize];
    const int num_bytes_read = unzReadCurrentFile(zip_file_, buf,
                                                  internal::kZipBufSize);
    if (num_bytes_read == 0) {
      // Reached the end of the file.
      break;
    } else if (num_bytes_read < 0) {
      // If num_bytes_read < 0, then it's a specific UNZ_* error code.
      success = false;
      break;
    } else if (num_bytes_read > 0) {
      // Some data is read. Write it to the output file descriptor.
      if (num_bytes_read !=
          file_util::WriteFileDescriptor(fd, buf, num_bytes_read)) {
        success = false;
        break;
      }
    }
  }

  unzCloseCurrentFile(zip_file_);
  return success;
}
#endif  // defined(OS_POSIX)

bool ZipReader::OpenInternal() {
  DCHECK(zip_file_);

  unz_global_info zip_info = {};  // Zero-clear.
  if (unzGetGlobalInfo(zip_file_, &zip_info) != UNZ_OK) {
    return false;
  }
  num_entries_ = zip_info.number_entry;
  if (num_entries_ < 0)
    return false;

  // We are already at the end if the zip file is empty.
  reached_end_ = (num_entries_ == 0);
  return true;
}

void ZipReader::Reset() {
  zip_file_ = NULL;
  num_entries_ = 0;
  reached_end_ = false;
  current_entry_info_.reset();
}

}  // namespace zip
