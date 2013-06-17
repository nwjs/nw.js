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

#include "chrome/common/zip.h"

#include "content/nw/src/api/app/app.h"

#include "base/command_line.h"
#include "base/logging.h"
#include "base/message_loop.h"
#include "base/values.h"
#include "content/nw/src/api/api_messages.h"
#include "content/nw/src/browser/net_disk_cache_remover.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_shell.h"
#include "content/common/view_messages.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/render_process_host.h"

namespace api {

namespace {

// Get render process host.
content::RenderProcessHost* GetRenderProcessHost() {
  content::RenderProcessHost* render_process_host = NULL;
  std::vector<content::Shell*> windows = content::Shell::windows();
  for (size_t i = 0; i < windows.size(); ++i) {
    if (!windows[i]->is_devtools()) {
      render_process_host = windows[i]->web_contents()->GetRenderProcessHost();
      break;
    }
  }

  return render_process_host;
}

}  // namespace
  
// static
void App::Call(const std::string& method,
               const base::ListValue& arguments) {
  if (method == "Quit") {
    Quit(GetRenderProcessHost());
    return;
  } else if (method == "CloseAllWindows") {
    CloseAllWindows();
    return;
  }
  NOTREACHED() << "Calling unknown method " << method << " of App";
}


// static
void App::Call(content::Shell* shell,
               const std::string& method,
               const base::ListValue& arguments,
               base::ListValue* result) {
  if (method == "GetArgv") {
    nw::Package* package = shell->GetPackage();
    CommandLine* command_line = CommandLine::ForCurrentProcess();
    CommandLine::StringVector args = command_line->GetArgs();
    CommandLine::StringVector argv = command_line->original_argv();

    // Ignore first non-switch arg if it's not a standalone package.
    bool ignore_arg = !package->self_extract();
    for (unsigned i = 1; i < argv.size(); ++i) {
      if (ignore_arg && argv[i] == args[0]) {
        ignore_arg = false;
        continue;
      }

      result->AppendString(argv[i]);
    }

    return;
  } else if (method == "Zip") { 
    std::string zipdir;
    std::string zipfile;
    arguments.GetString(0,&zipdir);
    arguments.GetString(1,&zipfile);
    result->AppendBoolean(zip::Zip(base::FilePath::FromUTF8Unsafe(zipdir), base::FilePath::FromUTF8Unsafe(zipfile), true));
    return;
  } else if (method == "Unzip") {
    std::string zipfile;
    std::string zipdir;
    arguments.GetString(0,&zipfile);
    arguments.GetString(1,&zipdir);
    result->AppendBoolean(zip::Unzip(base::FilePath::FromUTF8Unsafe(zipfile), base::FilePath::FromUTF8Unsafe(zipdir)));
    return;
  } else if (method == "ClearCache") {
    ClearCache(GetRenderProcessHost());
  }

  NOTREACHED() << "Calling unknown sync method " << method << " of App";
}

// static
void App::CloseAllWindows() {
  std::vector<content::Shell*> windows = content::Shell::windows();

  for (size_t i = 0; i < windows.size(); ++i) {
    // Only send close event to browser windows, since devtools windows will
    // be automatically closed.
    if (!windows[i]->is_devtools()) {
      // If there is no js object bound to the window, then just close.
      if (windows[i]->ShouldCloseWindow())
        delete windows[i];
    }
  }
}

// static
void App::Quit(content::RenderProcessHost* render_process_host) {
  // Send the quit message.
  int no_use;
  render_process_host->Send(new ViewMsg_WillQuit(&no_use));

  // Then quit.
  MessageLoop::current()->PostTask(FROM_HERE, MessageLoop::QuitClosure());
}

// static
void App::EmitOpenEvent(const std::string& path) {
  // Get the app's renderer process.
  content::RenderProcessHost* render_process_host = GetRenderProcessHost();
  DCHECK(render_process_host != NULL);

  render_process_host->Send(new ShellViewMsg_Open(path));
}

void App::ClearCache(content::RenderProcessHost* render_process_host) {
  render_process_host->Send(new ShellViewMsg_ClearCache());
  nw::RemoveHttpDiskCache(render_process_host->GetBrowserContext(),
                          render_process_host->GetID());
}

}  // namespace api
