// Copyright (c) 2012 Intel Corp
// Copyright (c) 2012 The Chromium Authors
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell co
// pies of the Software, and to permit persons to whom the Software is furnished
//  to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in al
// l copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IM
// PLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNES
// S FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
//  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WH
// ETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "content/nw/src/browser/shell_download_manager_delegate.h"

#if defined(TOOLKIT_GTK)
#include <gtk/gtk.h>
#endif

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
#include "content/public/browser/web_contents_view.h"
#include "net/base/net_util.h"

namespace content {

ShellDownloadManagerDelegate::ShellDownloadManagerDelegate()
    : download_manager_(NULL),
      suppress_prompting_(false),
      last_download_db_handle_(DownloadItem::kUninitializedHandle) {
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
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  // This assignment needs to be here because even at the call to
  // SetDownloadManager, the system is not fully initialized.
  if (default_download_path_.empty()) {
    default_download_path_ = download_manager_->GetBrowserContext()->GetPath().
        Append(FILE_PATH_LITERAL("Downloads"));
  }

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
          this, download->GetId(), callback, generated_name,
          default_download_path_));
  return true;
}

void ShellDownloadManagerDelegate::GenerateFilename(
    int32 download_id,
    const DownloadTargetCallback& callback,
    const FilePath& generated_name,
    const FilePath& suggested_directory) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::FILE));
  if (!file_util::PathExists(suggested_directory))
    file_util::CreateDirectory(suggested_directory);

  FilePath suggested_path(suggested_directory.Append(generated_name));
  BrowserThread::PostTask(
      BrowserThread::UI,
      FROM_HERE,
      base::Bind(
          &ShellDownloadManagerDelegate::OnDownloadPathGenerated,
          this, download_id, callback, suggested_path));
}

void ShellDownloadManagerDelegate::OnDownloadPathGenerated(
    int32 download_id,
    const DownloadTargetCallback& callback,
    const FilePath& suggested_path) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  if (suppress_prompting_) {
    // Testing exit.
    callback.Run(suggested_path, DownloadItem::TARGET_DISPOSITION_OVERWRITE,
                 DOWNLOAD_DANGER_TYPE_NOT_DANGEROUS,
                 suggested_path.AddExtension(FILE_PATH_LITERAL(".crdownload")));
    return;
  }

  ChooseDownloadPath(download_id, callback, suggested_path);
}

void ShellDownloadManagerDelegate::ChooseDownloadPath(
    int32 download_id,
    const DownloadTargetCallback& callback,
    const FilePath& suggested_path) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DownloadItem* item = download_manager_->GetDownload(download_id);
  if (!item || (item->GetState() != DownloadItem::IN_PROGRESS))
    return;

  FilePath result;
#if defined(OS_WIN)
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
#elif defined(TOOLKIT_GTK)
  GtkWidget *dialog;
  gfx::NativeWindow parent_window;
  std::string base_name = FilePath(suggested_path).BaseName().value();

  parent_window = item->GetWebContents()->GetView()->GetTopLevelNativeWindow();
  dialog = gtk_file_chooser_dialog_new("Save File",
                                       parent_window,
                                       GTK_FILE_CHOOSER_ACTION_SAVE,
                                       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                       GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                       NULL);
  gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog),
                                                 TRUE);
  gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog),
                                    base_name.c_str());

  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
    char *filename;
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    result = FilePath(filename);
  }
  gtk_widget_destroy(dialog);
#else
  NOTIMPLEMENTED();
#endif

  callback.Run(result, DownloadItem::TARGET_DISPOSITION_PROMPT,
               DOWNLOAD_DANGER_TYPE_NOT_DANGEROUS, result);
}

void ShellDownloadManagerDelegate::AddItemToPersistentStore(
    DownloadItem* item) {
  download_manager_->OnItemAddedToPersistentStore(
      item->GetId(), --last_download_db_handle_);
}

void ShellDownloadManagerDelegate::SetDownloadBehaviorForTesting(
    const FilePath& default_download_path) {
  default_download_path_ = default_download_path;
  suppress_prompting_ = true;
}

}  // namespace content
