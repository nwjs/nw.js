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

#include "third_party/zlib/google/zip.h"
#include "third_party/zlib/zlib.h"
#include "content/nw/src/api/app/app.h"

#include "base/command_line.h"
#include "base/logging.h"
#include "base/message_loop.h"
#include "base/values.h"
#include "content/nw/src/api/api_messages.h"
#include "content/nw/src/browser/native_window.h"
#include "content/nw/src/browser/net_disk_cache_remover.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_shell.h"
#include "content/nw/src/shell_browser_context.h"
#include "content/common/view_messages.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/render_process_host.h"
#if defined(OS_WIN)
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#endif

using base::MessageLoop;
using content::Shell;
using content::ShellBrowserContext;
using content::RenderProcessHost;

namespace api {

namespace {

// Get render process host.
RenderProcessHost* GetRenderProcessHost() {
  RenderProcessHost* render_process_host = NULL;
  std::vector<Shell*> windows = Shell::windows();
  for (size_t i = 0; i < windows.size(); ++i) {
    if (!windows[i]->is_devtools()) {
      render_process_host = windows[i]->web_contents()->GetRenderProcessHost();
      break;
    }
  }

  return render_process_host;
}

void GetRenderProcessHosts(std::set<RenderProcessHost*>& rphs) {
  RenderProcessHost* render_process_host = NULL;
  std::vector<Shell*> windows = Shell::windows();
  for (size_t i = 0; i < windows.size(); ++i) {
    if (!windows[i]->is_devtools()) {
      render_process_host = windows[i]->web_contents()->GetRenderProcessHost();
      rphs.insert(render_process_host);
    }
  }
}

}  // namespace

// static
void App::Call(const std::string& method,
               const base::ListValue& arguments) {
  if (method == "Quit") {
    Quit();
    return;
  } else if (method == "CloseAllWindows") {
    CloseAllWindows();
    return;
  }
  NOTREACHED() << "Calling unknown method " << method << " of App";
}


// static
void App::Call(Shell* shell,
               const std::string& method,
               const base::ListValue& arguments,
               base::ListValue* result) {
  if (method == "GetDataPath") {
    ShellBrowserContext* browser_context =
      static_cast<ShellBrowserContext*>(shell->web_contents()->GetBrowserContext());
    result->AppendString(browser_context->GetPath().value());
    return;
  }else if (method == "GetArgv") {
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
  } else if (method == "Gzip") {
	  std::string ssrc;
	  std::string sdst;
	  unsigned char buffer[0x1000];
	  int bytes_read = 1, bytes_written = 1;
	  
	  arguments.GetString(0,&ssrc);
	  arguments.GetString(1,&sdst);
#if defined(OS_WIN)
	  int src = _open(ssrc.c_str(), _O_RDONLY);
	  int dst = _open(sdst.c_str(), _O_WRONLY|_O_CREAT|_O_TRUNC);
#else
	  int src = open(ssrc.c_str(), O_RDONLY);
	  int dst = open(sdst.c_str(), O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
#endif
	  gzFile zDst = gzdopen(dst, "w");
	  if(gzbuffer(zDst, 0x1000)==-1) {
		  result->AppendBoolean(false);
		  return;
	  }
	  if(gzsetparams(zDst, Z_BEST_COMPRESSION, Z_DEFAULT_STRATEGY) != Z_OK) {
		  result->AppendBoolean(false);
		  return;
	  }
	  if(zDst == NULL || src == -1) {
		  result->AppendBoolean(false);
		  return;
	  }
	  
	  while(bytes_read > 0 && bytes_written > 0) {
#if defined(OS_WIN)
		  bytes_read = _read(src,&buffer,0x1000);
#else
		  bytes_read = read(src,&buffer,0x1000);
#endif
		  bytes_written = gzwrite(zDst,&buffer,bytes_read);
	  }
#if defined(OS_WIN)
	  _close(src);
#else
	  close(src);
#endif
	  //gzflush(zDst, Z_FINISH);
	  gzclose(zDst);
	  result->AppendBoolean(bytes_read != -1 && bytes_written != -1);
	  return;
  } else if (method == "Ungzip") {
	  std::string ssrc;
	  std::string sdst;
	  unsigned char buffer[0x1000];
	  int bytes_read = 1, bytes_written = 1, eof = 0;
	  
	  arguments.GetString(0,&ssrc);
	  arguments.GetString(1,&sdst);
	
#if defined(OS_WIN)
    int src = _open(ssrc.c_str(), _O_RDONLY);
	  int dst = _open(sdst.c_str(), _O_WRONLY|_O_CREAT|_O_TRUNC);
#else
	  int src = open(ssrc.c_str(), O_RDONLY);
	  int dst = open(sdst.c_str(), O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
#endif
	  gzFile zSrc = gzdopen(src, "r");
	  if(gzbuffer(zSrc, 0x1000)==-1) {
		  result->AppendBoolean(false);
		  return;
	  }
	  
	  if(zSrc == NULL || dst == -1) {
		  result->AppendBoolean(false);
		  return;
	  }
	  
	  while(eof == 0 &&
			bytes_read > 0 &&
			bytes_written > 0) {
		  bytes_read = gzread(zSrc,&buffer,0x1000);
#if defined(OS_WIN)
		  bytes_written = _write(dst,&buffer,bytes_read);
#else
		  bytes_written = write(dst,&buffer,bytes_read);
#endif
		  eof = gzeof(zSrc);
	  }
#if defined(OS_WIN)
	  _close(dst);
#else
	  close(dst);
#endif
	  gzclose(zSrc);
	  result->AppendBoolean(eof==1);
	  return;
  } else if (method == "ClearCache") {
    ClearCache(GetRenderProcessHost());
  } else if (method == "GetPackage") {
    result->AppendString(shell->GetPackage()->package_string());
    return;
  }

  NOTREACHED() << "Calling unknown sync method " << method << " of App";
}

// static
void App::CloseAllWindows(bool force) {
  std::vector<Shell*> windows = Shell::windows();

  for (size_t i = 0; i < windows.size(); ++i) {
    // Only send close event to browser windows, since devtools windows will
    // be automatically closed.
    if (!windows[i]->is_devtools()) {
      // If there is no js object bound to the window, then just close.
      if (force || windows[i]->ShouldCloseWindow())
        // we used to delete the Shell object here
        // but it should be deleted on native window destruction
        windows[i]->window()->Close();
    }
  }
  if (force) {
    // in a special force close case, since we're going to exit the
    // main loop soon, we should delete the shell object asap so the
    // render widget can be closed on the renderer side
    windows = Shell::windows();
    for (size_t i = 0; i < windows.size(); ++i) {
      if (!windows[i]->is_devtools())
        delete windows[i];
    }
  }
}

// static
void App::Quit(RenderProcessHost* render_process_host) {
  // Send the quit message.
  int no_use;
  if (render_process_host) {
    render_process_host->Send(new ViewMsg_WillQuit(&no_use));
  }else{
    std::set<RenderProcessHost*> rphs;
    std::set<RenderProcessHost*>::iterator it;

    GetRenderProcessHosts(rphs);
    for (it = rphs.begin(); it != rphs.end(); it++) {
      RenderProcessHost* rph = *it;
      DCHECK(rph != NULL);

      rph->Send(new ViewMsg_WillQuit(&no_use));
    }
    CloseAllWindows(true);
  }
  // Then quit.
  MessageLoop::current()->PostTask(FROM_HERE, MessageLoop::QuitClosure());
}

// static
void App::EmitOpenEvent(const std::string& path) {
  std::set<RenderProcessHost*> rphs;
  std::set<RenderProcessHost*>::iterator it;

  GetRenderProcessHosts(rphs);
  for (it = rphs.begin(); it != rphs.end(); it++) {
    RenderProcessHost* rph = *it;
    DCHECK(rph != NULL);

    rph->Send(new ShellViewMsg_Open(path));
  }
}

void App::ClearCache(content::RenderProcessHost* render_process_host) {
  render_process_host->Send(new ShellViewMsg_ClearCache());
  nw::RemoveHttpDiskCache(render_process_host->GetBrowserContext(),
                          render_process_host->GetID());
}

}  // namespace api
