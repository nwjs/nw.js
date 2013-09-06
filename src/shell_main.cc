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

#include "content/public/app/content_main.h"
#include "base/command_line.h"
#include "content/nw/src/shell_main_delegate.h"
#include "sandbox/win/src/sandbox_types.h"

#if defined(OS_WIN)
#include "base/command_line.h"
#include "base/file_util.h"
#include "base/files/scoped_temp_dir.h"
#include "third_party/zlib/google/zip.h"
#include "content/public/app/startup_helper_win.h"
#include "sandbox/win/src/sandbox_types.h"
#include "base/path_service.h"
#include "content/nw/src/nw_version.h"
#include "content/nw/src/net/util/embed_utils.h"
#endif

#if defined(OS_MACOSX)
#include "shell_content_main.h"
#endif

#if defined(OS_WIN)

#define VINFO "TI_" NW_VERSION_STRING

void Bootstrap() {
  CommandLine *cmd = CommandLine::ForCurrentProcess();
  CommandLine::StringVector args = cmd->GetArgs();

  base::FilePath path = (args.size() > 0) ? base::FilePath(args[0]) : base::FilePath(cmd->GetProgram());
  path = base::MakeAbsoluteFilePath(path);
  if(file_util::DirectoryExists(path)) {
    cmd->AppendSwitchNative("resources",path.value());
    return;
  } else if(file_util::PathExists(path)) {
    PathService::Get(base::DIR_LOCAL_APP_DATA, &path);
    std::string version = VINFO;
    std::wstring resources[] = {L"d3dcompiler_46.dll",L"ffmpegsumo.dll",L"icudt.dll",L"libEGL.dll",L"libGLESv2.dll",L"nw.pak"};
    int resource_count = 7;
    path = path.Append(std::wstring(version.begin(),version.end()));
    cmd->AppendSwitchNative("resources", path.value());
    if(!file_util::PathExists(path)) file_util::CreateDirectory(path);
    for(int i=0; i < resource_count; i++) {
      if(!file_util::PathExists(path.Append(resources[i]))) {
        embed_util::FileMetaInfo meta;
        if(embed_util::Utility::GetFileInfo(std::string(resources[i].begin(),resources[i].end()), &meta) &&
          embed_util::Utility::GetFileData(&meta))
          file_util::WriteFile(path.Append(resources[i]),reinterpret_cast<const char *>(meta.data),meta.data_size);
      }
    }
    return;
  } else {
    cmd->AppendSwitchNative("resources", base::MakeAbsoluteFilePath(cmd->GetProgram().DirName()).value());
    return;
  }
}

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, wchar_t*, int) {
  CommandLine::Init(__argc, __argv);
  if(!CommandLine::ForCurrentProcess()->HasSwitch("resources")) Bootstrap();
  AddDllDirectory(CommandLine::ForCurrentProcess()->GetSwitchValueNative("resources").c_str());
  sandbox::SandboxInterfaceInfo sandbox_info = {0};
  content::InitializeSandboxInfo(&sandbox_info);
  content::ShellMainDelegate delegate;
  return content::ContentMain(instance, &sandbox_info, &delegate);
}

#else

int main(int argc, const char** argv) {
#if defined(OS_MACOSX)
  // Do the delegate work in shell_content_main to avoid having to export the
  // delegate types.
  return ::ContentMain(argc, argv);
#else
  content::ShellMainDelegate delegate;
  return content::ContentMain(argc, argv, &delegate);
#endif  // OS_MACOSX
}

#endif  // OS_POSIX
