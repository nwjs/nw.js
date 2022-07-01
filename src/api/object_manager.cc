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

#include "content/nw/src/api/object_manager.h"

#include "base/logging.h"
#include "base/run_loop.h"
#include "base/threading/thread_restrictions.h"
#include "base/values.h"
#include "content/browser/child_process_security_policy_impl.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/nw/src/api/api_messages.h"
#include "content/nw/src/api/object_manager_factory.h"
//#include "content/nw/src/api/app/app.h"
#include "content/nw/src/api/base/base.h"
//#include "content/nw/src/api/clipboard/clipboard.h"
//#include "content/nw/src/api/event/event.h"
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/api/menuitem/menuitem.h"
//#include "content/nw/src/api/screen/screen.h"
#include "content/nw/src/api/shell/shell.h"
//#include "content/nw/src/api/shortcut/shortcut.h"
#include "content/nw/src/api/tray/tray.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_view_host.h"

#include "extensions/browser/event_router.h"

using content::WebContents;
using extensions::Event;
using extensions::EventRouter;

namespace nw {

base::IDMap<std::unique_ptr<Base>> nw::ObjectManager::objects_registry_;
int nw::ObjectManager::next_object_id_ = 0;

ObjectManager* ObjectManager::Get(content::BrowserContext* context) {
  return ObjectManagerFactory::GetForBrowserContext(context);
}

ObjectManager::ObjectManager(content::BrowserContext* context)
  : run_loop_(nullptr), browser_context_(context), weak_ptr_factory_(this) {
}

ObjectManager::~ObjectManager() {
  std::set<int>::iterator it;
  for (it = objects_.begin(); it != objects_.end(); it++) {
    if (objects_registry_.Lookup(*it))
      objects_registry_.Remove(*it);
  }
}

void ObjectManager::ClearObjectRegistry() {
  objects_registry_.Clear();
}

// static
Base* ObjectManager::GetApiObject(int id) {
  return objects_registry_.Lookup(id);
}

// static
int ObjectManager::AllocateId() {
  return ++next_object_id_;
}

void ObjectManager::OnAllocateObject(int object_id,
				     const std::string& type,
				     const base::DictionaryValue& option,
				     const std::string& extension_id) {
  DVLOG(1) << "OnAllocateObject: object_id:" << object_id
             << " type:" << type
             << " option:" << option;

  if (type == "Menu") {
    objects_registry_.AddWithID(std::make_unique<Menu>(object_id, weak_ptr_factory_.GetWeakPtr(), option, extension_id), object_id);
  } else if (type == "MenuItem") {
    objects_registry_.AddWithID(
                                std::make_unique<MenuItem>(object_id, weak_ptr_factory_.GetWeakPtr(), option, extension_id), object_id);
  } else if (type == "Tray") {
    objects_registry_.AddWithID(std::make_unique<Tray>(object_id, weak_ptr_factory_.GetWeakPtr(), option, extension_id), object_id);
  }
#if 0
  else if (type == "Clipboard") {
    objects_registry_.AddWithID(
                                std::make_unique<Clipboard>(object_id, weak_ptr_factory_.GetWeakPtr(), option), object_id);
  } else if (type == "Window") {
    objects_registry_.AddWithID(std::make_unique<Window>(object_id, weak_ptr_factory_.GetWeakPtr(), option), object_id);
  } else if (type == "Shortcut") {
    objects_registry_.AddWithID(std::make_unique<Shortcut>(object_id, weak_ptr_factory_.GetWeakPtr(), option), object_id);
  } else if (type == "Screen") {
    objects_registry_.AddWithID(std::make_unique<EventListener>(object_id, weak_ptr_factory_.GetWeakPtr(), option), object_id);
  }
#endif
  else {
    LOG(ERROR) << "Allocate an object of unknown type: " << type;
    objects_registry_.AddWithID(std::make_unique<Base>(object_id, weak_ptr_factory_.GetWeakPtr(), option, extension_id), object_id);
  }
  objects_.insert(object_id);
}

void ObjectManager::OnDeallocateObject(int object_id) {
  DLOG(INFO) << "OnDeallocateObject: object_id:" << object_id;
  Base* obj = objects_registry_.Lookup(object_id);
  if (obj) {
    if (obj->delay_destruction()) {
      obj->set_pending_destruction(true);
      return;
    }
    objects_registry_.Remove(object_id);
  }
  objects_.erase(object_id);
}

void ObjectManager::OnCallObjectMethod(
    content::RenderFrameHost* rvh,
    int object_id,
    const std::string& type,
    const std::string& method,
    const base::Value::List& arguments) {
#if 0
  DLOG(INFO) << "OnCallObjectMethod: object_id:" << object_id
             << " type:" << type
             << " method:" << method
             << " arguments:" << arguments;
#endif
  Base* object = GetApiObject(object_id);
  if (object)
    object->Call(method, arguments, rvh);
  else
    DLOG(WARNING) << "Unknown object: " << object_id
             << " type:" << type
             << " method:" << method
             << " arguments:" << arguments;
}

void ObjectManager::OnCallObjectMethodSync(
    content::RenderFrameHost* rvh,
    int object_id,
    const std::string& type,
    const std::string& method,
    const base::Value::List& arguments,
    base::Value::List* result) {
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

void ObjectManager::OnCallStaticMethod(
    content::RenderFrameHost* rvh,
    const std::string& type,
    const std::string& method,
    const base::Value::List& arguments) {
  DLOG(INFO) << "OnCallStaticMethod: "
             << " type:" << type
             << " method:" << method
             << " arguments:" << arguments;
  if (type == "Shell") {
    nw::Shell::Call(method, arguments, browser_context_);
    return;
  }
#if 0
 else if (type == "App") {
    nwapi::App::Call(method, arguments);
    return;
  }
#endif
  NOTREACHED() << "Calling unknown method " << method << " of class " << type;
}

void ObjectManager::OnCallStaticMethodSync(
    content::RenderFrameHost* rvh,
    const std::string& type,
    const std::string& method,
    const base::ListValue& arguments,
    base::ListValue* result) {
  DLOG(INFO) << "OnCallStaticMethodSync: "
             << " type:" << type
             << " method:" << method
             << " arguments:" << arguments;
#if 0
  if (type == "App") {
    content::Shell* shell =
        content::Shell::FromRenderFrameHost(render_view_host());
    nwapi::App::Call(shell, method, arguments, result, this);
    return;
  } else if (type == "Screen") {
    nwapi::Screen::Call(this, method, arguments, result);
    return;
  }

  NOTREACHED() << "Calling unknown method " << method << " of class " << type;
#endif
}

void ObjectManager::SendEvent(Base* object,
                              const std::string& event_name,
                              const base::ListValue& args) {
  EventRouter* event_router = EventRouter::Get(browser_context_);
  if (!event_router)
    return;
  base::Value::List arguments;
  arguments.Append(base::Value(object->id()));
  for (size_t i = 0; i < args.GetListDeprecated().size(); i++)
    arguments.Append(args.GetListDeprecated()[i].Clone());
  std::unique_ptr<Event> event(new Event(extensions::events::UNKNOWN, "NWObject" + event_name, std::move(arguments), browser_context_));
  event->user_gesture = EventRouter::USER_GESTURE_ENABLED;
  event_router->DispatchEventToExtension(object->extension_id_, std::move(event));
}
}  // namespace nw
