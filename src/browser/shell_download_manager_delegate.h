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

#ifndef CONTENT_NW_SRC_BROWSER_SHELL_DOWNLOAD_MANAGER_DELEGATE_H_
#define CONTENT_NW_SRC_BROWSER_SHELL_DOWNLOAD_MANAGER_DELEGATE_H_

#include "base/compiler_specific.h"
#include "base/memory/ref_counted.h"
#include "content/public/browser/download_manager_delegate.h"

namespace content {

class DownloadManager;

class ShellDownloadManagerDelegate
    : public DownloadManagerDelegate,
      public base::RefCountedThreadSafe<ShellDownloadManagerDelegate> {
 public:
  ShellDownloadManagerDelegate();

  void SetDownloadManager(DownloadManager* manager);

  virtual void Shutdown() OVERRIDE;
  virtual bool DetermineDownloadTarget(
      DownloadItem* download,
      const DownloadTargetCallback& callback) OVERRIDE;

  // Inhibits prompting and sets the default download path.
  void SetDownloadBehaviorForTesting(
      const FilePath& default_download_path);

 protected:
  // To allow subclasses for testing.
  virtual ~ShellDownloadManagerDelegate();

 private:
  friend class base::RefCountedThreadSafe<ShellDownloadManagerDelegate>;


  void GenerateFilename(int32 download_id,
                        const DownloadTargetCallback& callback,
                        const FilePath& generated_name,
                        const FilePath& suggested_directory);
  void OnDownloadPathGenerated(int32 download_id,
                               const DownloadTargetCallback& callback,
                               const FilePath& suggested_path);
  void ChooseDownloadPath(int32 download_id,
                          const DownloadTargetCallback& callback,
                          const FilePath& suggested_path);

  DownloadManager* download_manager_;
  FilePath default_download_path_;
  bool suppress_prompting_;

  DISALLOW_COPY_AND_ASSIGN(ShellDownloadManagerDelegate);
};

}  // namespace content

#endif  // CONTENT_NW_SRC_BROWSER_SHELL_DOWNLOAD_MANAGER_DELEGATE_H_
