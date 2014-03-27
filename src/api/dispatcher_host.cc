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

#include "content/nw/src/api/dispatcher_host.h"

#include "base/logging.h"
#include "base/threading/thread_restrictions.h"
#include "base/values.h"
#include "content/browser/child_process_security_policy_impl.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/nw/src/api/api_messages.h"
#include "content/nw/src/api/app/app.h"
#include "content/nw/src/api/base/base.h"
#include "content/nw/src/api/clipboard/clipboard.h"
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/api/menuitem/menuitem.h"
#include "content/nw/src/api/shell/shell.h"
#include "content/nw/src/api/shortcut/shortcut.h"
#include "content/nw/src/api/tray/tray.h"
#include "content/nw/src/api/window/window.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/shell_browser_context.h"
#include "content/nw/src/nw_shell.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_view_host.h"

using content::WebContents;
using content::ShellBrowserContext;
using content::Shell;

namespace nwapi {

IDMap<Base, IDMapOwnPointer> nwapi::DispatcherHost::objects_registry_;
int nwapi::DispatcherHost::next_object_id_ = 1;
static std::map<content::RenderViewHost*, DispatcherHost*> g_dispatcher_host_map;

DispatcherHost::DispatcherHost(content::RenderViewHost* host)
  : content::WebContentsObserver(content::WebContents::FromRenderViewHost(host)),
    render_view_host_(host),
    weak_ptr_factory_(this) {
  g_dispatcher_host_map[render_view_host_] = this;
}

DispatcherHost::~DispatcherHost() {
  g_dispatcher_host_map.erase(render_view_host());
  std::set<int>::iterator it;
  for (it = objects_.begin(); it != objects_.end(); it++) {
    if (objects_registry_.Lookup(*it))
      objects_registry_.Remove(*it);
  }
}

DispatcherHost*
FindDispatcherHost(content::RenderViewHost* render_view_host) {
  std::map<content::RenderViewHost*, DispatcherHost*>::iterator it
    = g_dispatcher_host_map.find(render_view_host);
  if (it == g_dispatcher_host_map.end())
    return NULL;
  return it->second;
}

void DispatcherHost::ClearObjectRegistry() {
  objects_registry_.Clear();
}

// static
Base* DispatcherHost::GetApiObject(int id) {
  return objects_registry_.Lookup(id);
}

// static
int DispatcherHost::AllocateId() {
  return next_object_id_++;
}

void DispatcherHost::SendEvent(Base* object,
                               const std::string& event,
                               const base::ListValue& arguments) {
  Send(new ShellViewMsg_Object_On_Event(
       routing_id(), object->id(), event, arguments));
}

bool DispatcherHost::Send(IPC::Message* message) {
  return render_view_host_->Send(message);
}

bool DispatcherHost::OnMessageReceived(content::RenderViewHost* render_view_host,
                                       const IPC::Message& message) {
  if (render_view_host != render_view_host_)
    return false;
  bool handled = true;
  base::ThreadRestrictions::ScopedAllowIO allow_io;
  base::ThreadRestrictions::ScopedAllowWait allow_wait;
  IPC_BEGIN_MESSAGE_MAP(DispatcherHost, message)
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_Allocate_Object, OnAllocateObject)
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_Deallocate_Object, OnDeallocateObject)
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_Call_Object_Method, OnCallObjectMethod)
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_Call_Object_Method_Sync,
                        OnCallObjectMethodSync)
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_Call_Static_Method, OnCallStaticMethod)
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_Call_Static_Method_Sync,
                        OnCallStaticMethodSync)
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_UncaughtException,
                        OnUncaughtException);
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_GetShellId, OnGetShellId);
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_CreateShell, OnCreateShell);
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_AllocateId, OnAllocateId);
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_SetForceClose, OnSetForceClose);
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()

  return handled;
}

void DispatcherHost::RenderViewHostChanged(content::RenderViewHost* old_host,
                                           content::RenderViewHost* new_host) {
  // LOG(INFO) << "RenderViewHostChanged(" << this << "): " << old_host << " --> " << new_host << " ; " << render_view_host_;
  if (render_view_host_ != new_host)
    delete this;
}

void DispatcherHost::OnAllocateObject(int object_id,
                                      const std::string& type,
                                      const base::DictionaryValue& option) {
  DVLOG(1) << "OnAllocateObject: object_id:" << object_id
             << " type:" << type
             << " option:" << option;

  if (type == "Menu") {
    objects_registry_.AddWithID(new Menu(object_id, weak_ptr_factory_.GetWeakPtr(), option), object_id);
  } else if (type == "MenuItem") {
    objects_registry_.AddWithID(
        new MenuItem(object_id, weak_ptr_factory_.GetWeakPtr(), option), object_id);
  } else if (type == "Tray") {
    objects_registry_.AddWithID(new Tray(object_id, weak_ptr_factory_.GetWeakPtr(), option), object_id);
  } else if (type == "Clipboard") {
    objects_registry_.AddWithID(
        new Clipboard(object_id, weak_ptr_factory_.GetWeakPtr(), option), object_id);
  } else if (type == "Window") {
    objects_registry_.AddWithID(new Window(object_id, weak_ptr_factory_.GetWeakPtr(), option), object_id);
  } else if (type == "Shortcut") {
    objects_registry_.AddWithID(new Shortcut(object_id, weak_ptr_factory_.GetWeakPtr(), option), object_id);
  } else {
    LOG(ERROR) << "Allocate an object of unknown type: " << type;
    objects_registry_.AddWithID(new Base(object_id, weak_ptr_factory_.GetWeakPtr(), option), object_id);
  }
  objects_.insert(object_id);
}

void DispatcherHost::OnDeallocateObject(int object_id) {
  DLOG(INFO) << "OnDeallocateObject: object_id:" << object_id;
  if (objects_registry_.Lookup(object_id))
    objects_registry_.Remove(object_id);
  objects_.erase(object_id);
}

void DispatcherHost::OnCallObjectMethod(
    int object_id,
    const std::string& type,
    const std::string& method,
    const base::ListValue& arguments) {
  DLOG(INFO) << "OnCallObjectMethod: object_id:" << object_id
             << " type:" << type
             << " method:" << method
             << " arguments:" << arguments;

  Base* object = GetApiObject(object_id);
  if (object)
    object->Call(method, arguments);
  else
    DLOG(WARNING) << "Unknown object: " << object_id
             << " type:" << type
             << " method:" << method
             << " arguments:" << arguments;
}

void DispatcherHost::OnCallObjectMethodSync(
    int object_id,
    const std::string& type,
    const std::string& method,
    const base::ListValue& arguments,
    base::ListValue* result) {
  DLOG(INFO) << "OnCallObjectMethodSync: object_id:" << object_id
             << " type:" << type
             << " method:" << method
             << " arguments:" << arguments;

  Base* object = GetApiObject(object_id);
  if (object)
    object->CallSync(method, arguments, result);
  else
    DLOG(WARNING) << "Unknown object: " << object_id
             << " type:" << type
             << " method:" << method
             << " arguments:" << arguments;
}

void DispatcherHost::OnCallStaticMethod(
    const std::string& type,
    const std::string& method,
    const base::ListValue& arguments) {
  DLOG(INFO) << "OnCallStaticMethod: "
             << " type:" << type
             << " method:" << method
             << " arguments:" << arguments;

  if (type == "Shell") {
    nwapi::Shell::Call(method, arguments);
    return;
  } else if (type == "App") {
    nwapi::App::Call(method, arguments);
    return;
  }

  NOTREACHED() << "Calling unknown method " << method << " of class " << type;
}

void DispatcherHost::OnCallStaticMethodSync(
    const std::string& type,
    const std::string& method,
    const base::ListValue& arguments,
    base::ListValue* result) {
  DLOG(INFO) << "OnCallStaticMethodSync: "
             << " type:" << type
             << " method:" << method
             << " arguments:" << arguments;

  if (type == "App") {
    content::Shell* shell =
        content::Shell::FromRenderViewHost(render_view_host());
    nwapi::App::Call(shell, method, arguments, result);
    return;
  }

  NOTREACHED() << "Calling unknown method " << method << " of class " << type;
}

void DispatcherHost::OnUncaughtException(const std::string& err) {
  content::Shell* shell =
      content::Shell::FromRenderViewHost(render_view_host());
  shell->PrintCriticalError("Uncaught node.js Error", err);
}

void DispatcherHost::OnGetShellId(int* id) {
  content::Shell* shell =
      content::Shell::FromRenderViewHost(render_view_host());
  *id = shell->id();
}

void DispatcherHost::OnCreateShell(const std::string& url,
                                   const base::DictionaryValue& manifest,
                                   int* routing_id) {
  WebContents* base_web_contents =
      content::Shell::FromRenderViewHost(render_view_host())->web_contents();
  ShellBrowserContext* browser_context =
      static_cast<ShellBrowserContext*>(base_web_contents->GetBrowserContext());
  scoped_ptr<base::DictionaryValue> new_manifest(manifest.DeepCopy());
  bool new_renderer = false;
  if (new_manifest->GetBoolean(switches::kmNewInstance,
                               &new_renderer) && new_renderer)
    browser_context->set_pinning_renderer(false);

  WebContents::CreateParams create_params(browser_context,
                                          new_renderer ? NULL : base_web_contents->GetSiteInstance());

  std::string filename;
  if (new_manifest->GetString(switches::kmInjectJSDocStart, &filename))
    create_params.nw_inject_js_doc_start = filename;
  if (new_manifest->GetString(switches::kmInjectJSDocEnd, &filename))
    create_params.nw_inject_js_doc_end = filename;
  if (new_manifest->GetString(switches::kmInjectCSS, &filename))
    create_params.nw_inject_css_fn = filename;

  WebContents* web_contents = content::WebContentsImpl::CreateWithOpener(
      create_params,
      static_cast<content::WebContentsImpl*>(base_web_contents));

  content::Shell::Create(base_web_contents,
                           GURL(url),
                           new_manifest.get(),
                           web_contents);

  if (new_renderer) {
    browser_context->set_pinning_renderer(true);
  }

  *routing_id = web_contents->GetRoutingID();

  int object_id = 0;
  if (new_manifest->GetInteger("object_id", &object_id)) {
    DispatcherHost* dhost = FindDispatcherHost(web_contents->GetRenderViewHost());
    dhost->OnAllocateObject(object_id, "Window", *new_manifest.get());
  }
}

void DispatcherHost::OnAllocateId(int * ret) {
  *ret = AllocateId();
}

void DispatcherHost::OnSetForceClose(bool force, int* ret) {
  content::Shell* shell =
      content::Shell::FromRenderViewHost(render_view_host());
  shell->set_force_close(force);
  *ret = 0;
}

}  // namespace nwapi
