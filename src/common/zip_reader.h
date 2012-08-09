// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_COMMON_ZIP_READER_H_
#define CHROME_COMMON_ZIP_READER_H_

#include <string>

#include "base/basictypes.h"
#include "base/file_path.h"
#include "base/file_util.h"
#include "base/memory/scoped_ptr.h"
#include "base/time.h"
#include "third_party/zlib/contrib/minizip/unzip.h"

namespace zip {

// This class is used for reading zip files. A typical use case of this
// class is to scan entries in a zip file and extract them. The code will
// look like:
//
//   ZipReader reader;
//   reader.Open(zip_file_path);
//   while (reader.HasMore()) {
//     reader.OpenCurrentEntryInZip();
//     reader.ExtractCurrentEntryToDirectory(output_directory_path);
//     reader.AdvanceToNextEntry();
//   }
//
// For simplicty, error checking is omitted in the example code above. The
// production code should check return values from all of these functions.
//
// This calls can also be used for random access of contents in a zip file
// using LocateAndOpenEntry().
//
class ZipReader {
 public:
  // This class represents information of an entry (file or directory) in
  // a zip file.
  class EntryInfo {
   public:
    EntryInfo(const std::string& filename_in_zip,
              const unz_file_info& raw_file_info);

    // Returns the file path. The path is usually relative like
    // "foo/bar.txt", but if it's absolute, is_unsafe() returns true.
    const FilePath& file_path() const { return file_path_; }

    // Returns the size of the original file (i.e. after uncompressed).
    // Returns 0 if the entry is a directory.
    int64 original_size() const { return original_size_; }

    // Returns the last modified time.
    base::Time last_modified() const { return last_modified_; }

    // Returns true if the entry is a directory.
    bool is_directory() const { return is_directory_; }

    // Returns true if the entry is unsafe, like having ".." or invalid
    // UTF-8 characters in its file name, or the file path is absolute.
    bool is_unsafe() const { return is_unsafe_; }

   private:
    const FilePath file_path_;
    int64 original_size_;
    base::Time last_modified_;
    bool is_directory_;
    bool is_unsafe_;
    DISALLOW_COPY_AND_ASSIGN(EntryInfo);
  };

  ZipReader();
  ~ZipReader();

  // Opens the zip file specified by |zip_file_path|. Returns true on
  // success.
  bool Open(const FilePath& zip_file_path);

#if defined(OS_POSIX)
  // Opens the zip file referred to by the file descriptor |zip_fd|.
  // Returns true on success.
  bool OpenFromFd(int zip_fd);
#endif

  // Closes the currently opened zip file. This function is called in the
  // destructor of the class, so you usually don't need to call this.
  void Close();

  // Returns true if there is at least one entry to read. This function is
  // used to scan entries with AdvanceToNextEntry(), like:
  //
  // while (reader.HasMore()) {
  //   // Do something with the current file here.
  //   reader.AdvanceToNextEntry();
  // }
  bool HasMore();

  // Advances the next entry. Returns true on success.
  bool AdvanceToNextEntry();

  // Opens the current entry in the zip file. On success, returns true and
  // updates the the current entry state (i.e. current_entry_info() is
  // updated). This function should be called before operations over the
  // current entry like ExtractCurrentEntryToFile().
  //
  // Note that there is no CloseCurrentEntryInZip(). The the current entry
  // state is reset automatically as needed.
  bool OpenCurrentEntryInZip();

  // Locates an entry in the zip file and opens it. Returns true on
  // success. This function internally calls OpenCurrentEntryInZip() on
  // success. On failure, current_entry_info() becomes NULL.
  bool LocateAndOpenEntry(const FilePath& path_in_zip);

  // Extracts the current entry to the given output file path. If the
  // current file is a directory, just creates a directory
  // instead. Returns true on success. OpenCurrentEntryInZip() must be
  // called beforehand.
  //
  // This function does not preserve the timestamp of the original entry.
  bool ExtractCurrentEntryToFilePath(const FilePath& output_file_path);

  // Extracts the current entry to the given output directory path using
  // ExtractCurrentEntryToFilePath(). Sub directories are created as needed
  // based on the file path of the current entry. For example, if the file
  // path in zip is "foo/bar.txt", and the output directory is "output",
  // "output/foo/bar.txt" will be created.
  //
  // Returns true on success. OpenCurrentEntryInZip() must be called
  // beforehand.
  bool ExtractCurrentEntryIntoDirectory(const FilePath& output_directory_path);

#if defined(OS_POSIX)
  // Extracts the current entry by writing directly to a file descriptor.
  // Does not close the file descriptor. Returns true on success.
  bool ExtractCurrentEntryToFd(int fd);
#endif

  // Returns the current entry info. Returns NULL if the current entry is
  // not yet opened. OpenCurrentEntryInZip() must be called beforehand.
  EntryInfo* current_entry_info() const {
    return current_entry_info_.get();
  }

  // Returns the number of entries in the zip file.
  // Open() must be called beforehand.
  int num_entries() const { return num_entries_; }

 private:
  // Common code used both in Open and OpenFromFd.
  bool OpenInternal();

  // Resets the internal state.
  void Reset();

  unzFile zip_file_;
  int num_entries_;
  bool reached_end_;
  scoped_ptr<EntryInfo> current_entry_info_;

  DISALLOW_COPY_AND_ASSIGN(ZipReader);
};

}  // namespace zip

#endif  // CHROME_COMMON_ZIP_READER_H_
