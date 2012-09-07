// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/platform_util.h"

#include <commdlg.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <shlobj.h>

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/file_path.h"
#include "base/file_util.h"
#include "base/logging.h"
#include "base/string_util.h"
#include "base/utf_string_conversions.h"
#include "base/win/registry.h"
#include "base/win/scoped_co_mem.h"
#include "base/win/scoped_comptr.h"
#include "content/public/browser/browser_thread.h"
#include "googleurl/src/gurl.h"
#include "ui/base/win/shell.h"
#include "ui/gfx/native_widget_types.h"

using content::BrowserThread;

namespace {

void ShowItemInFolderOnFileThread(const FilePath& full_path) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::FILE));
  FilePath dir = full_path.DirName();
  // ParseDisplayName will fail if the directory is "C:", it must be "C:\\".
  if (dir.value() == L"" || !file_util::EnsureEndsWithSeparator(&dir))
    return;

  typedef HRESULT (WINAPI *SHOpenFolderAndSelectItemsFuncPtr)(
      PCIDLIST_ABSOLUTE pidl_Folder,
      UINT cidl,
      PCUITEMID_CHILD_ARRAY pidls,
      DWORD flags);

  static SHOpenFolderAndSelectItemsFuncPtr open_folder_and_select_itemsPtr =
    NULL;
  static bool initialize_open_folder_proc = true;
  if (initialize_open_folder_proc) {
    initialize_open_folder_proc = false;
    // The SHOpenFolderAndSelectItems API is exposed by shell32 version 6
    // and does not exist in Win2K. We attempt to retrieve this function export
    // from shell32 and if it does not exist, we just invoke ShellExecute to
    // open the folder thus losing the functionality to select the item in
    // the process.
    HMODULE shell32_base = GetModuleHandle(L"shell32.dll");
    if (!shell32_base) {
      NOTREACHED() << " " << __FUNCTION__ << "(): Can't open shell32.dll";
      return;
    }
    open_folder_and_select_itemsPtr =
        reinterpret_cast<SHOpenFolderAndSelectItemsFuncPtr>
            (GetProcAddress(shell32_base, "SHOpenFolderAndSelectItems"));
  }
  if (!open_folder_and_select_itemsPtr) {
    ShellExecute(NULL, L"open", dir.value().c_str(), NULL, NULL, SW_SHOW);
    return;
  }

  base::win::ScopedComPtr<IShellFolder> desktop;
  HRESULT hr = SHGetDesktopFolder(desktop.Receive());
  if (FAILED(hr))
    return;

  base::win::ScopedCoMem<ITEMIDLIST> dir_item;
  hr = desktop->ParseDisplayName(NULL, NULL,
                                 const_cast<wchar_t *>(dir.value().c_str()),
                                 NULL, &dir_item, NULL);
  if (FAILED(hr))
    return;

  base::win::ScopedCoMem<ITEMIDLIST> file_item;
  hr = desktop->ParseDisplayName(NULL, NULL,
      const_cast<wchar_t *>(full_path.value().c_str()),
      NULL, &file_item, NULL);
  if (FAILED(hr))
    return;

  const ITEMIDLIST* highlight[] = {
    {file_item},
  };

  hr = (*open_folder_and_select_itemsPtr)(dir_item, arraysize(highlight),
                                          highlight, NULL);

  if (FAILED(hr)) {
    // On some systems, the above call mysteriously fails with "file not
    // found" even though the file is there.  In these cases, ShellExecute()
    // seems to work as a fallback (although it won't select the file).
    if (hr == ERROR_FILE_NOT_FOUND) {
      ShellExecute(NULL, L"open", dir.value().c_str(), NULL, NULL, SW_SHOW);
    } else {
      LPTSTR message = NULL;
      DWORD message_length = FormatMessage(
          FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
          0, hr, 0, reinterpret_cast<LPTSTR>(&message), 0, NULL);
      LOG(WARNING) << " " << __FUNCTION__
                   << "(): Can't open full_path = \""
                   << full_path.value() << "\""
                   << " hr = " << hr
                   << " " << reinterpret_cast<LPTSTR>(&message);
      if (message)
        LocalFree(message);
    }
  }
}

}  // namespace

namespace platform_util {

void ShowItemInFolder(const FilePath& full_path) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  BrowserThread::PostTask(BrowserThread::FILE, FROM_HERE,
      base::Bind(&ShowItemInFolderOnFileThread, full_path));
}

void OpenItem(const FilePath& full_path) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  BrowserThread::PostTask(
      BrowserThread::FILE, FROM_HERE,
      base::Bind(base::IgnoreResult(&ui::win::OpenItemViaShell), full_path));
}

void OpenExternal(const GURL& url) {
  // Quote the input scheme to be sure that the command does not have
  // parameters unexpected by the external program. This url should already
  // have been escaped.
  std::string escaped_url = url.spec();
  escaped_url.insert(0, "\"");
  escaped_url += "\"";

  // According to Mozilla in uriloader/exthandler/win/nsOSHelperAppService.cpp:
  // "Some versions of windows (Win2k before SP3, Win XP before SP1) crash in
  // ShellExecute on long URLs (bug 161357 on bugzilla.mozilla.org). IE 5 and 6
  // support URLS of 2083 chars in length, 2K is safe."
  const size_t kMaxUrlLength = 2048;
  if (escaped_url.length() > kMaxUrlLength) {
    NOTREACHED();
    return;
  }

  base::win::RegKey key;
  std::wstring registry_path = ASCIIToWide(url.scheme()) +
                               L"\\shell\\open\\command";
  key.Open(HKEY_CLASSES_ROOT, registry_path.c_str(), KEY_READ);
  if (key.Valid()) {
    DWORD size = 0;
    key.ReadValue(NULL, NULL, &size, NULL);
    if (size <= 2) {
      // ShellExecute crashes the process when the command is empty.
      // We check for "2" because it always returns the trailing NULL.
      // TODO(nsylvain): we should also add a dialog to warn on errors. See
      // bug 1136923.
      return;
    }
  }

  if (reinterpret_cast<ULONG_PTR>(ShellExecuteA(NULL, "open",
                                                escaped_url.c_str(), NULL, NULL,
                                                SW_SHOWNORMAL)) <= 32) {
    // We fail to execute the call. We could display a message to the user.
    // TODO(nsylvain): we should also add a dialog to warn on errors. See
    // bug 1136923.
    return;
  }
}

#if !defined(USE_AURA)
gfx::NativeWindow GetTopLevel(gfx::NativeView view) {
  return ::GetAncestor(view, GA_ROOT);
}

gfx::NativeView GetParent(gfx::NativeView view) {
  return ::GetParent(view);
}

bool IsWindowActive(gfx::NativeWindow window) {
  return ::GetForegroundWindow() == window;
}

void ActivateWindow(gfx::NativeWindow window) {
  ::SetForegroundWindow(window);
}

bool IsVisible(gfx::NativeView view) {
  // MSVC complains if we don't include != 0.
  return ::IsWindowVisible(view) != 0;
}
#endif

}  // namespace platform_util
