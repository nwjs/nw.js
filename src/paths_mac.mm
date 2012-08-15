// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/paths_mac.h"

#include "base/mac/bundle_locations.h"
#include "base/mac/foundation_util.h"
#include "base/path_service.h"
#include "content/public/common/content_paths.h"

namespace {

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

}  // namespace

void OverrideFrameworkBundlePath() {
  FilePath helper_path =
      GetFrameworksPath().Append("Content Shell Framework.framework");

  base::mac::SetOverrideFrameworkBundlePath(helper_path);
}

void OverrideChildProcessPath() {
  FilePath helper_path = GetFrameworksPath().Append("Content Shell Helper.app")
                                            .Append("Contents")
                                            .Append("MacOS")
                                            .Append("Content Shell Helper");

  PathService::Override(content::CHILD_PROCESS_EXE, helper_path);
}

FilePath GetResourcesPakFilePath() {
  NSString* pak_path =
      [base::mac::FrameworkBundle() pathForResource:@"content_shell"
                                             ofType:@"pak"];

  return FilePath([pak_path fileSystemRepresentation]);
}
