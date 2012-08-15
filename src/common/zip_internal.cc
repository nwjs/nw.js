// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "zip.h"

#include "base/utf_string_conversions.h"
#include "third_party/zlib/contrib/minizip/unzip.h"
#include "third_party/zlib/contrib/minizip/zip.h"
#if defined(OS_WIN)
#include "third_party/zlib/contrib/minizip/iowin32.h"
#elif defined(OS_POSIX)
#include "third_party/zlib/contrib/minizip/ioapi.h"
#endif

namespace {

#if defined(OS_WIN)
typedef struct {
  HANDLE hf;
  int error;
} WIN32FILE_IOWIN;

// This function is derived from third_party/minizip/iowin32.c.
// Its only difference is that it treats the char* as UTF8 and
// uses the Unicode version of CreateFile.
void* ZipOpenFunc(void *opaque, const char* filename, int mode) {
  DWORD desired_access, creation_disposition;
  DWORD share_mode, flags_and_attributes;
  HANDLE file = 0;
  void* ret = NULL;

  desired_access = share_mode = flags_and_attributes = 0;

  if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER) == ZLIB_FILEFUNC_MODE_READ) {
    desired_access = GENERIC_READ;
    creation_disposition = OPEN_EXISTING;
    share_mode = FILE_SHARE_READ;
  } else if (mode & ZLIB_FILEFUNC_MODE_EXISTING) {
    desired_access = GENERIC_WRITE | GENERIC_READ;
    creation_disposition = OPEN_EXISTING;
  } else if (mode & ZLIB_FILEFUNC_MODE_CREATE) {
    desired_access = GENERIC_WRITE | GENERIC_READ;
    creation_disposition = CREATE_ALWAYS;
  }

  string16 filename16 = UTF8ToUTF16(filename);
  if ((filename != NULL) && (desired_access != 0)) {
    file = CreateFile(filename16.c_str(), desired_access, share_mode,
        NULL, creation_disposition, flags_and_attributes, NULL);
  }

  if (file == INVALID_HANDLE_VALUE)
    file = NULL;

  if (file != NULL) {
    WIN32FILE_IOWIN file_ret;
    file_ret.hf = file;
    file_ret.error = 0;
    ret = malloc(sizeof(WIN32FILE_IOWIN));
    if (ret == NULL)
      CloseHandle(file);
    else
      *(static_cast<WIN32FILE_IOWIN*>(ret)) = file_ret;
  }
  return ret;
}
#endif

#if defined(OS_POSIX)
// Callback function for zlib that opens a file stream from a file descriptor.
void* FdOpenFileFunc(void* opaque, const char* filename, int mode) {
  FILE* file = NULL;
  const char* mode_fopen = NULL;

  if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER) == ZLIB_FILEFUNC_MODE_READ)
    mode_fopen = "rb";
  else if (mode & ZLIB_FILEFUNC_MODE_EXISTING)
    mode_fopen = "r+b";
  else if (mode & ZLIB_FILEFUNC_MODE_CREATE)
    mode_fopen = "wb";

  if ((filename != NULL) && (mode_fopen != NULL))
    file = fdopen(*static_cast<int*>(opaque), mode_fopen);

  return file;
}

// We don't actually close the file stream since that would close
// the underlying file descriptor, and we don't own it. We do free
// |opaque| since we malloc'ed it in FillFdOpenFileFunc.
int CloseFileFunc(void* opaque, void* stream) {
  free(opaque);
  return 0;
}

// Fills |pzlib_filecunc_def| appropriately to handle the zip file
// referred to by |fd|.
void FillFdOpenFileFunc(zlib_filefunc_def* pzlib_filefunc_def, int fd) {
  fill_fopen_filefunc(pzlib_filefunc_def);
  pzlib_filefunc_def->zopen_file = FdOpenFileFunc;
  pzlib_filefunc_def->zclose_file = CloseFileFunc;
  int* ptr_fd = static_cast<int*>(malloc(sizeof(fd)));
  *ptr_fd = fd;
  pzlib_filefunc_def->opaque = ptr_fd;
}
#endif  // defined(OS_POSIX)

}  // namespace

namespace zip {
namespace internal {

unzFile OpenForUnzipping(const std::string& file_name_utf8) {
  zlib_filefunc_def* zip_func_ptrs = NULL;
#if defined(OS_WIN)
  zlib_filefunc_def zip_funcs;
  fill_win32_filefunc(&zip_funcs);
  zip_funcs.zopen_file = ZipOpenFunc;
  zip_func_ptrs = &zip_funcs;
#endif
  return unzOpen2(file_name_utf8.c_str(), zip_func_ptrs);
}

#if defined(OS_POSIX)
unzFile OpenFdForUnzipping(int zip_fd) {
  zlib_filefunc_def zip_funcs;
  FillFdOpenFileFunc(&zip_funcs, zip_fd);
  // Passing dummy "fd" filename to zlib.
  return unzOpen2("fd", &zip_funcs);
}
#endif

zipFile OpenForZipping(const std::string& file_name_utf8, int append_flag) {
  zlib_filefunc_def* zip_func_ptrs = NULL;
#if defined(OS_WIN)
  zlib_filefunc_def zip_funcs;
  fill_win32_filefunc(&zip_funcs);
  zip_funcs.zopen_file = ZipOpenFunc;
  zip_func_ptrs = &zip_funcs;
#endif
  return zipOpen2(file_name_utf8.c_str(),
                  append_flag,
                  NULL,  // global comment
                  zip_func_ptrs);
}

}  // namespace internal
}  // namespace zip
