// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/shell_download_manager_delegate.h"

#if defined(OS_WIN)
#include <windows.h>
#include <commdlg.h>
#endif

#include "base/bind.h"
#include "base/file_util.h"
#include "base/logging.h"
#include "base/string_util.h"
#include "base/utf_string_conversions.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/download_manager.h"
#include "content/public/browser/web_contents.h"
#include "net/base/net_util.h"

namespace content {

ShellDownloadManagerDelegate::ShellDownloadManagerDelegate()
    : download_manager_(NULL) {
  // Balanced in Shutdown();
  AddRef();
}

ShellDownloadManagerDelegate::~ShellDownloadManagerDelegate(){
}


void ShellDownloadManagerDelegate::SetDownloadManager(
    DownloadManager* download_manager) {
  download_manager_ = download_manager;
}

void ShellDownloadManagerDelegate::Shutdown() {
  Release();
}

bool ShellDownloadManagerDelegate::DetermineDownloadTarget(
    DownloadItem* download,
    const DownloadTargetCallback& callback) {
  if (!download->GetForcedFilePath().empty()) {
    callback.Run(download->GetForcedFilePath(),
                 DownloadItem::TARGET_DISPOSITION_OVERWRITE,
                 DOWNLOAD_DANGER_TYPE_NOT_DANGEROUS,
                 download->GetForcedFilePath());
    return true;
  }

  FilePath generated_name = net::GenerateFileName(
      download->GetURL(),
      download->GetContentDisposition(),
      download->GetReferrerCharset(),
      download->GetSuggestedFilename(),
      download->GetMimeType(),
      "download");

  BrowserThread::PostTask(
      BrowserThread::FILE,
      FROM_HERE,
      base::Bind(
          &ShellDownloadManagerDelegate::GenerateFilename,
          this, download->GetId(), callback, generated_name));
  return false;
}

void ShellDownloadManagerDelegate::GenerateFilename(
    int32 download_id,
    const DownloadTargetCallback& callback,
    const FilePath& generated_name) {
  FilePath suggested_path = download_manager_->GetBrowserContext()->GetPath().
      Append(FILE_PATH_LITERAL("Downloads"));
  if (!file_util::PathExists(suggested_path))
    file_util::CreateDirectory(suggested_path);

  suggested_path = suggested_path.Append(generated_name);
  BrowserThread::PostTask(
      BrowserThread::UI,
      FROM_HERE,
      base::Bind(
          &ShellDownloadManagerDelegate::ChooseDownloadPath,
          this, download_id, callback, suggested_path));
}

void ShellDownloadManagerDelegate::ChooseDownloadPath(
    int32 download_id,
    const DownloadTargetCallback& callback,
    const FilePath& suggested_path) {
  DownloadItem* item =
      download_manager_->GetActiveDownloadItem(download_id);
  if (!item)
    return;

  FilePath result;
#if defined(OS_WIN) && !defined(USE_AURA)
  std::wstring file_part = FilePath(suggested_path).BaseName().value();
  wchar_t file_name[MAX_PATH];
  base::wcslcpy(file_name, file_part.c_str(), arraysize(file_name));
  OPENFILENAME save_as;
  ZeroMemory(&save_as, sizeof(save_as));
  save_as.lStructSize = sizeof(OPENFILENAME);
  save_as.hwndOwner = item->GetWebContents()->GetNativeView();
  save_as.lpstrFile = file_name;
  save_as.nMaxFile = arraysize(file_name);

  std::wstring directory;
  if (!suggested_path.empty())
    directory = suggested_path.DirName().value();

  save_as.lpstrInitialDir = directory.c_str();
  save_as.Flags = OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING |
                  OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;

  if (GetSaveFileName(&save_as))
    result = FilePath(std::wstring(save_as.lpstrFile));
#else
  NOTIMPLEMENTED();
#endif

  callback.Run(result, DownloadItem::TARGET_DISPOSITION_PROMPT,
               DOWNLOAD_DANGER_TYPE_NOT_DANGEROUS, result);
}

}  // namespace content
