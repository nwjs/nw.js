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

#if defined(OS_WIN)
#include <windows.h>
#include <commdlg.h>
#endif

#include "base/bind.h"
#include "base/file_util.h"
#include "base/logging.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/platform_util.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/download_manager.h"
#include "content/public/browser/web_contents.h"
#include "net/base/net_util.h"

#include "ui/aura/window.h"
#include "ui/aura/window_tree_host.h"

namespace content {

void ShellDownloadManagerDelegate::ChooseDownloadPath(
    int32 download_id,
    const DownloadTargetCallback& callback,
    const base::FilePath& suggested_path) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DownloadItem* item = download_manager_->GetDownload(download_id);
  if (!item || (item->GetState() != DownloadItem::IN_PROGRESS))
    return;

  WebContents* web_contents = item->GetWebContents();
  select_file_dialog_ = ui::SelectFileDialog::Create(this, NULL);
  ui::SelectFileDialog::FileTypeInfo file_type_info;
  // Platform file pickers, notably on Mac and Windows, tend to break
  // with double extensions like .tar.gz, so only pass in normal ones.
  base::FilePath::StringType extension = suggested_path.FinalExtension();
  if (!extension.empty()) {
    extension.erase(extension.begin());  // drop the .
    file_type_info.extensions.resize(1);
    file_type_info.extensions[0].push_back(extension);
  }
  file_type_info.include_all_files = true;
  file_type_info.support_drive = true;
  gfx::NativeWindow owning_window = web_contents ?
      platform_util::GetTopLevel(web_contents->GetNativeView()) : NULL;

  callback_ = callback;
  base::FilePath working_path;
  select_file_dialog_->SelectFile(ui::SelectFileDialog::SELECT_SAVEAS_FILE,
                                  base::string16(),
                                  suggested_path,
                                  &file_type_info,
                                  0,
                                  base::FilePath::StringType(),
                                  owning_window,
                                  NULL, working_path);
}

void ShellDownloadManagerDelegate::OnFileSelected(const base::FilePath& path) {
  callback_.Run(path, DownloadItem::TARGET_DISPOSITION_PROMPT,
                DOWNLOAD_DANGER_TYPE_NOT_DANGEROUS, path);
}

void ShellDownloadManagerDelegate::FileSelected(const base::FilePath& path,
                                      int index,
                                      void* params) {
  OnFileSelected(path);
}

void ShellDownloadManagerDelegate::FileSelectionCanceled(void* params) {
  OnFileSelected(base::FilePath());
}

}  // namespace content
