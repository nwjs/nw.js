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

#include <AppKit/NSSavePanel.h>
#include <Foundation/NSURL.h>

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

void ShellDownloadManagerDelegate::ChooseDownloadPath(
    int32 download_id,
    const DownloadTargetCallback& callback,
    const FilePath& suggested_path) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DownloadItem* item = download_manager_->GetDownload(download_id);
  if (!item || (item->GetState() != DownloadItem::IN_PROGRESS))
    return;

  FilePath result;
  std::string base_name = FilePath(suggested_path).BaseName().value();

  NSSavePanel *savePanel = [NSSavePanel savePanel];

  [savePanel setNameFieldStringValue:[NSString stringWithUTF8String:base_name.c_str()]];

  if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
    char *filename = (char *)[[[savePanel URL] path] UTF8String];

    result = FilePath(filename);
  }

  callback.Run(result, DownloadItem::TARGET_DISPOSITION_PROMPT,
               DOWNLOAD_DANGER_TYPE_NOT_DANGEROUS, result);
}
}  // namespace content
