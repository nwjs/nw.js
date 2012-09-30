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
#include "content/public/browser/render_view_host_observer.h"

#include <string>

namespace base {
class DictionaryValue;
class ListValue;
}

namespace WebKit {
class WebFrame;
}

namespace api {

class Base;

class DispatcherHost : public content::RenderViewHostObserver {
 public:
  explicit DispatcherHost(content::RenderViewHost* render_view_host);
  virtual ~DispatcherHost();

  Base* GetObject(int id);
  void SendEvent(Base* object,
                 const std::string& event,
                 const base::ListValue& arguments);

  content::RenderViewHost* render_view_host() const {
    return content::RenderViewHostObserver::render_view_host();
  }

 private:
  IDMap<Base, IDMapOwnPointer> objects_registry_;

  // RenderViewHostObserver implementation.
  virtual bool OnMessageReceived(const IPC::Message& message) OVERRIDE;

  void OnAllocateObject(int object_id,
                        const std::string& type,
                        const base::DictionaryValue& option);
  void OnDeallocateObject(int object_id);
  void OnCallObjectMethod(int object_id,
                          const std::string& type,
                          const std::string& method,
                          const base::ListValue& arguments);

  DISALLOW_COPY_AND_ASSIGN(DispatcherHost);
};

}  // namespace api

#endif  // CONTENT_NW_SRC_API_DISPATCHER_HOST_H_
