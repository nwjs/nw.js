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

#include "content/nw/src/api/shell/shell.h"

#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/bind.h"
#include "base/logging.h"
#include "base/values.h"
#include "base/task/post_task.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_thread.h"
#include "chrome/browser/platform_util.h"
#include "chrome/browser/profiles/profile.h"
#include "url/gurl.h"

using base::FilePath;

namespace nw {

namespace {

  void VerifyItemType(const FilePath &path, platform_util::OpenItemType *item_type) {
    *item_type = base::DirectoryExists(path) ? platform_util::OPEN_FOLDER : platform_util::OPEN_FILE;
  }

  void OnItemTypeVerified(Profile* profile, const FilePath &path, const platform_util::OpenItemType *item_type) {
    platform_util::OpenItem(profile, path,
                            *item_type, platform_util::OpenOperationCallback());
  }

}

// static
void Shell::Call(const std::string& method,
                 const base::ListValue& arguments,
                 content::BrowserContext* context) {
  Profile* profile = Profile::FromBrowserContext(context);
  if (method == "OpenExternal") {
    std::string uri;
    arguments.GetString(0, &uri);
    platform_util::OpenExternal(profile, GURL(uri));
  } else if (method == "OpenItem") {
    std::string full_path;
    arguments.GetString(0, &full_path);
    FilePath path = FilePath::FromUTF8Unsafe(full_path);
    platform_util::OpenItemType *item_type = new platform_util::OpenItemType();
    base::PostTaskWithTraitsAndReply(
      FROM_HERE,
      {base::MayBlock(), base::TaskShutdownBehavior::CONTINUE_ON_SHUTDOWN},
      base::Bind(&VerifyItemType, path, base::Unretained(item_type)),
      base::Bind(&OnItemTypeVerified, profile, path, base::Owned(item_type))
      );
  } else if (method == "ShowItemInFolder") {
    std::string full_path;
    arguments.GetString(0, &full_path);
    platform_util::ShowItemInFolder(profile, FilePath::FromUTF8Unsafe(full_path));
  } else {
    NOTREACHED() << "Calling unknown method " << method << " of Shell";
  }
}

}  // namespace nw
