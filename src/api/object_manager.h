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

#ifndef CONTENT_NW_SRC_API_OBJECT_MANAGER_H_
#define CONTENT_NW_SRC_API_OBJECT_MANAGER_H_

#include "base/containers/id_map.h"
#include "base/memory/weak_ptr.h"
#include "components/keyed_service/core/keyed_service.h"
#include "content/public/browser/web_contents_observer.h"

#include <string>
#include <set>

namespace base {
class DictionaryValue;
class ListValue;
class RunLoop;
}

namespace WebKit {
class WebFrame;
}

namespace content {
class BrowserContext;
class RenderFrameHost;
}

namespace nw {

class Base;

class ObjectManager : public KeyedService {
 public:
  explicit ObjectManager(content::BrowserContext* context);
  ~ObjectManager() override;

  // Get C++ object from its id.
  static Base* GetApiObject(int id);

  static int AllocateId();

  static ObjectManager* Get(content::BrowserContext* context);
  // Helper function to convert type.
  template<class T>
  static T* GetApiObject(int id) {
    return static_cast<T*>(GetApiObject(id));
  }

  static void ClearObjectRegistry();

  void set_run_loop(base::RunLoop* run_loop) { run_loop_ = run_loop; }
  base::RunLoop* run_loop() { return run_loop_; }

  // Send event to C++ object's corresponding js object.
  void SendEvent(Base* object,
                 const std::string& event,
                 const base::ListValue& arguments);

  void OnAllocateObject(int object_id,
                        const std::string& type,
                        const base::DictionaryValue& option,
                        const std::string& extension_id);
  void OnDeallocateObject(int object_id);
  void OnCallObjectMethod(content::RenderFrameHost* rvh,
                          int object_id,
                          const std::string& type,
                          const std::string& method,
                          const base::Value::List& arguments);
  void OnCallObjectMethodSync(content::RenderFrameHost* rvh,
                              int object_id,
                              const std::string& type,
                              const std::string& method,
                              const base::Value::List& arguments,
                              base::Value::List* result);
  void OnCallStaticMethod(content::RenderFrameHost* rvh,
                          const std::string& type,
                          const std::string& method,
                          const base::Value::List& arguments);
  void OnCallStaticMethodSync(content::RenderFrameHost* rvh,
                              const std::string& type,
                              const std::string& method,
                              const base::ListValue& arguments,
                              base::ListValue* result);
 private:
  static base::IDMap<std::unique_ptr<Base>> objects_registry_;
  static int next_object_id_;

  base::RunLoop* run_loop_;

  std::set<int> objects_;

  content::BrowserContext* browser_context_;
  // Factory to generate weak pointer
  base::WeakPtrFactory<ObjectManager> weak_ptr_factory_;

};

}  // namespace nw

#endif
