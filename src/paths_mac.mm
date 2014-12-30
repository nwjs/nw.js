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

#include "paths_mac.h"

#include "base/mac/bundle_locations.h"
#include "base/mac/foundation_util.h"
#include "base/path_service.h"
#include "base/strings/sys_string_conversions.h"
#include "content/public/common/content_paths.h"

using base::FilePath;

FilePath GetFrameworksPath() {
  // Start out with the path to the running executable.
  FilePath path;
  PathService::Get(base::FILE_EXE, &path);

  // Up to Contents.
  if (base::mac::IsBackgroundOnlyProcess()) {
    // The running executable is the helper. Go up five steps:
    // Contents/Frameworks/Helper.app/Contents/MacOS/Helper
    // ^ to here                                     ^ from here
    path = path.DirName().DirName().DirName().DirName().DirName();
  } else {
    // One step up to MacOS, another to Contents.
    path = path.DirName().DirName();
  }
  DCHECK_EQ(path.BaseName().value(), "Contents");

  // Go into the frameworks directory.
  return path.Append("Frameworks");
}

void OverrideFrameworkBundlePath() {
  FilePath helper_path =
      GetFrameworksPath().Append("nwjs Framework.framework");

  base::mac::SetOverrideFrameworkBundlePath(helper_path);
}

void OverrideChildProcessPath() {
  FilePath helper_path = GetFrameworksPath().Append("nwjs Helper.app")
                                            .Append("Contents")
                                            .Append("MacOS")
                                            .Append("nwjs Helper");

  PathService::Override(content::CHILD_PROCESS_EXE, helper_path);
}

bool GetResourcesPakFilePath(FilePath& output) {
  NSString* pak_path =
      [base::mac::FrameworkBundle() pathForResource:@"nw"
                                             ofType:@"pak"];

  if (!pak_path)
    return false;
  output = FilePath([pak_path fileSystemRepresentation]);
  return true;
}

bool GetLocalePakFilePath(const std::string& locale, FilePath& output) {
  NSString* pak_path =
      [base::mac::FrameworkBundle() pathForResource:base::SysUTF8ToNSString(locale)
                                             ofType:@"pak"];

  if (!pak_path)
    return false;
  output = FilePath([pak_path fileSystemRepresentation]);
  return true;
}
