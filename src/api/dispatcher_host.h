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

#ifndef CONTENT_NW_SRC_API_DISPATCHER_HOST_H_
#define CONTENT_NW_SRC_API_DISPATCHER_HOST_H_

#include "base/basictypes.h"
#include "base/id_map.h"
#include "base/memory/weak_ptr.h"
#include "content/public/browser/web_contents_observer.h"

#include <string>
#include <set>

namespace base {
class DictionaryValue;
class ListValue;
}

namespace WebKit {
class WebFrame;
}

namespace content {
class Shell;
}

namespace nwapi {

class Base;

class DispatcherHost : public content::WebContentsObserver {
 public:
  explicit DispatcherHost(content::RenderViewHost* render_view_host);
  virtual ~DispatcherHost();

  // Get C++ object from its id.
  static Base* GetApiObject(int id);

  static int AllocateId();

  // Helper function to convert type.
  template<class T>
  static T* GetApiObject(int id) {
    return static_cast<T*>(GetApiObject(id));
  }

  static void ClearObjectRegistry();

  // Send event to C++ object's corresponding js object.
  void SendEvent(Base* object,
                 const std::string& event,
                 const base::ListValue& arguments);

  virtual bool Send(IPC::Message* message) OVERRIDE;
  virtual void RenderViewHostChanged(content::RenderViewHost* old_host,
                                     content::RenderViewHost* new_host) OVERRIDE;
  content::RenderViewHost* render_view_host() const {
    return render_view_host_;
  }

 private:
  content::RenderViewHost* render_view_host_;
  friend class content::Shell;

  static IDMap<Base, IDMapOwnPointer> objects_registry_;
  static int next_object_id_;

  std::set<int> objects_;

  // Factory to generate weak pointer
  base::WeakPtrFactory<DispatcherHost> weak_ptr_factory_;

  // RenderViewHostObserver implementation.
  // WebContentsObserver implementation:
  virtual bool OnMessageReceived(content::RenderViewHost* render_view_host, const IPC::Message& message) OVERRIDE;

  void OnAllocateObject(int object_id,
                        const std::string& type,
                        const base::DictionaryValue& option);
  void OnDeallocateObject(int object_id);
  void OnCallObjectMethod(int object_id,
                          const std::string& type,
                          const std::string& method,
                          const base::ListValue& arguments);
  void OnCallObjectMethodSync(int object_id,
                              const std::string& type,
                              const std::string& method,
                              const base::ListValue& arguments,
                              base::ListValue* result);
  void OnCallStaticMethod(const std::string& type,
                          const std::string& method,
                          const base::ListValue& arguments);
  void OnCallStaticMethodSync(const std::string& type,
                              const std::string& method,
                              const base::ListValue& arguments,
                              base::ListValue* result);
  void OnUncaughtException(const std::string& err);
  void OnGetShellId(int* id);
  void OnCreateShell(const std::string& url,
                     const base::DictionaryValue& manifest,
                     int* routing_id);
  void OnAllocateId(int* ret);
  void OnSetForceClose(bool force, int* ret);

  DISALLOW_COPY_AND_ASSIGN(DispatcherHost);
};

nwapi::DispatcherHost* FindDispatcherHost(content::RenderViewHost* render_view_host);

}  // namespace nwapi

#endif  // CONTENT_NW_SRC_API_DISPATCHER_HOST_H_
