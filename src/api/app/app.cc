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

#include "content/nw/src/api/app/app.h"

#include "base/command_line.h"
#include "base/logging.h"
#include "base/message_loop/message_loop.h"
#include "base/values.h"

#if defined(OS_WIN)
#include "base/strings/utf_string_conversions.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/win/shortcut.h"
#include "base/path_service.h"
#include "content/nw/src/common/shell_switches.h"
#endif

#include "content/nw/src/api/api_messages.h"
#include "content/nw/src/api/dispatcher_host.h"
#include "content/nw/src/api/shortcut/global_shortcut_listener.h"
#include "content/nw/src/api/shortcut/shortcut.h"
#include "content/nw/src/breakpad_linux.h"
#include "content/nw/src/browser/native_window.h"
#include "content/nw/src/browser/net_disk_cache_remover.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_shell.h"
#include "content/nw/src/shell_browser_context.h"
#include "content/common/view_messages.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/render_process_host.h"
#include "net/proxy/proxy_config.h"
#include "net/proxy/proxy_config_service_fixed.h"
#include "net/proxy/proxy_service.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_getter.h"

using base::MessageLoop;
using content::BrowserThread;
using content::Shell;
using content::ShellBrowserContext;
using content::RenderProcessHost;

namespace nwapi {

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

void SetProxyConfigCallback(
    base::WaitableEvent* done,
    net::URLRequestContextGetter* url_request_context_getter,
    const net::ProxyConfig& proxy_config) {
  net::ProxyService* proxy_service =
      url_request_context_getter->GetURLRequestContext()->proxy_service();
  proxy_service->ResetConfigService(
      new net::ProxyConfigServiceFixed(proxy_config));
  done->Signal();
}

}  // namespace

// static
void App::Call(const std::string& method,
               const base::ListValue& arguments) {
  if (method == "Quit") {
    Quit();
  } else if (method == "CloseAllWindows") {
    CloseAllWindows();
  } else if (method == "CrashBrowser") {
    int* ptr = NULL;
    *ptr = 1;
  } else {
    NOTREACHED() << "Calling unknown method " << method << " of App.";
  }
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
  } else if (method == "ClearCache") {
    ClearCache(GetRenderProcessHost());
    return;
  } else if (method == "CreateShortcut") {
#if defined(OS_WIN)
    base::string16 path;
    arguments.GetString(0, &path);

    base::win::ShortcutProperties props;
    base::string16 appID;
    if (content::Shell::GetPackage()->root()->GetString("app-id", &appID) == false)
      content::Shell::GetPackage()->root()->GetString(switches::kmName, &appID);
    const std::wstring appName = base::UTF8ToWide(content::Shell::GetPackage()->GetName());
    props.set_app_id(appID);

    base::FilePath processPath;
    PathService::Get(base::FILE_EXE, &processPath);
    props.set_target(processPath);

    base::FilePath shortcutPath(path);
    result->AppendBoolean(base::win::CreateOrUpdateShortcutLink(shortcutPath, props, 
      base::PathExists(shortcutPath) ? base::win::SHORTCUT_UPDATE_EXISTING : base::win::SHORTCUT_CREATE_ALWAYS));
#else
    result->AppendBoolean(false);
#endif
    return;
  } else if (method == "GetPackage") {
    result->AppendString(shell->GetPackage()->package_string());
    return;
  } else if (method == "SetCrashDumpDir") {
    std::string path;
    arguments.GetString(0, &path);
    //FIXME: result->AppendBoolean(SetCrashDumpPath(path.c_str()));
    return;
  } else if (method == "RegisterGlobalHotKey") {
    int object_id = -1;
    arguments.GetInteger(0, &object_id);
    Shortcut* shortcut =
        static_cast<Shortcut*>(DispatcherHost::GetApiObject(object_id));
    bool success = GlobalShortcutListener::GetInstance()->RegisterAccelerator(
                       shortcut->GetAccelerator(), shortcut);
    if (!success)
      shortcut->OnFailed("Register global desktop keyboard shortcut failed.");

    result->AppendBoolean(success);
    return;
  } else if (method == "UnregisterGlobalHotKey") {
    int object_id = -1;
    arguments.GetInteger(0, &object_id);
    Shortcut* shortcut =
        static_cast<Shortcut*>(DispatcherHost::GetApiObject(object_id));
    GlobalShortcutListener::GetInstance()->UnregisterAccelerator(
        shortcut->GetAccelerator(), shortcut);
    return;
  } else if (method == "SetProxyConfig") {
    std::string proxy_config;
    arguments.GetString(0, &proxy_config);
    SetProxyConfig(GetRenderProcessHost(), proxy_config);
    return;
  }

  NOTREACHED() << "Calling unknown sync method " << method << " of App";
}

// static
void App::CloseAllWindows(bool force, bool quit) {
  std::vector<Shell*> windows = Shell::windows();

  for (size_t i = 0; i < windows.size(); ++i) {
    // Only send close event to browser windows, since devtools windows will
    // be automatically closed.
    if (!windows[i]->is_devtools()) {
      // If there is no js object bound to the window, then just close.
      if (force || windows[i]->ShouldCloseWindow(quit))
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

// static
void App::EmitReopenEvent() {
  std::set<RenderProcessHost*> rphs;
  std::set<RenderProcessHost*>::iterator it;

  GetRenderProcessHosts(rphs);
  for (it = rphs.begin(); it != rphs.end(); it++) {
    RenderProcessHost* rph = *it;
    DCHECK(rph != NULL);

    rph->Send(new ShellViewMsg_Reopen());
  }
}

void App::ClearCache(content::RenderProcessHost* render_process_host) {
  render_process_host->Send(new ShellViewMsg_ClearCache());
  nw::RemoveHttpDiskCache(render_process_host->GetBrowserContext(),
                          render_process_host->GetID());
}

void App::SetProxyConfig(content::RenderProcessHost* render_process_host,
                         const std::string& proxy_config) {
  net::ProxyConfig config;
  config.proxy_rules().ParseFromString(proxy_config);
  net::URLRequestContextGetter* context_getter =
    render_process_host->GetBrowserContext()->
    GetRequestContextForRenderProcess(render_process_host->GetID());

  base::WaitableEvent done(false, false);
  BrowserThread::PostTask(
      BrowserThread::IO, FROM_HERE,
      base::Bind(&SetProxyConfigCallback, &done,
                 make_scoped_refptr(context_getter), config));
  done.Wait();

}
}  // namespace nwapi
