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
#include "content/nw/src/api/api_messages.h"

namespace api {

DispatcherHost::DispatcherHost(content::RenderViewHost* render_view_host)
    : content::RenderViewHostObserver(render_view_host) {
}

DispatcherHost::~DispatcherHost() {
}

bool DispatcherHost::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(DispatcherHost, message)
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_Allocate_Object, OnAllocateObject)
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_Deallocate_Object, OnDeallocateObject)
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_Call_Object_Method, OnCallObjectMethod)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()

  return handled;
}

void DispatcherHost::OnAllocateObject(int object_id,
                                      const std::string& type,
                                      const base::DictionaryValue& option) {
  LOG(INFO) << "OnAllocateObject: object_id:" << object_id
            << " type:" << type;
}

void DispatcherHost::OnDeallocateObject(int object_id) {
  LOG(INFO) << "OnDeallocateObject: object_id:" << object_id;
}

void DispatcherHost::OnCallObjectMethod(
    int object_id,
    const std::string& type,
    const std::string& method,
    const base::ListValue& arguments) {
  LOG(INFO) << "OnCallObjectMethod: object_id:" << object_id
            << " type:" << type
            << " method:" << method << " (";
  int size = arguments.GetSize();
  for (int i = 0; i < size; ++i) {
    const Value* value;
    arguments.Get(i, &value);
    std::string str;
    if (value->GetAsString(&str))
      LOG(INFO) << str;
    else
      LOG(INFO) << "[other type]";

    if (i != size - 1)
      LOG(INFO) << ", ";
  }
  LOG(INFO) << ")";
}

}  // namespace api
