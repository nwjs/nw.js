// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/platform_util.h"

#include "base/bind.h"
#include "base/file_util.h"
#include "base/process_util.h"
#include "base/utf_string_conversions.h"
#include "content/public/browser/browser_thread.h"
#include "googleurl/src/gurl.h"

using content::BrowserThread;

namespace {

void XDGUtil(const std::string& util, const std::string& arg) {
  std::vector<std::string> argv;
  argv.push_back(util);
  argv.push_back(arg);

  base::EnvironmentVector env;
  // xdg-open can fall back on mailcap which eventually might plumb through
  // to a command that needs a terminal.  Set the environment variable telling
  // it that we definitely don't have a terminal available and that it should
  // bring up a new terminal if necessary.  See "man mailcap".
  env.push_back(std::make_pair("MM_NOTTTY", "1"));

  // In Google Chrome, we do not let GNOME's bug-buddy intercept our crashes.
  // However, we do not want this environment variable to propagate to external
  // applications. See http://crbug.com/24120
  char* disable_gnome_bug_buddy = getenv("GNOME_DISABLE_CRASH_DIALOG");
  if (disable_gnome_bug_buddy &&
      disable_gnome_bug_buddy == std::string("SET_BY_GOOGLE_CHROME")) {
    env.push_back(std::make_pair("GNOME_DISABLE_CRASH_DIALOG", ""));
  }

  base::ProcessHandle handle;
  base::LaunchOptions options;
  options.environ = &env;
  if (base::LaunchProcess(argv, options, &handle))
    base::EnsureProcessGetsReaped(handle);
}

void XDGOpen(const std::string& path) {
  XDGUtil("xdg-open", path);
}

void XDGEmail(const std::string& email) {
  XDGUtil("xdg-email", email);
}

// TODO(estade): It would be nice to be able to select the file in the file
// manager, but that probably requires extending xdg-open. For now just
// show the folder.
void ShowItemInFolderOnFileThread(const FilePath& full_path) {
  FilePath dir = full_path.DirName();
  if (!file_util::DirectoryExists(dir))
    return;

  XDGOpen(dir.value());
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
  BrowserThread::PostTask(BrowserThread::FILE, FROM_HERE,
      base::Bind(&XDGOpen, full_path.value()));
}

void OpenExternal(const GURL& url) {
  if (url.SchemeIs("mailto"))
    XDGEmail(url.spec());
  else
    XDGOpen(url.spec());
}

}  // namespace platform_util
