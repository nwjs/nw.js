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

#ifndef CONTENT_NW_SRC_API_APP_APP_H_
#define CONTENT_NW_SRC_API_APP_APP_H_

#include "base/basictypes.h"
#include "../dispatcher_host.h"

#include <string>

namespace base {
class ListValue;
}

namespace content {
class RenderProcessHost;
class Shell;
}

namespace nwapi {

class App {
 public:
  static void Call(const std::string& method,
                   const base::ListValue& arguments);

  static void Call(content::Shell* shell,
                   const std::string& method,
                   const base::ListValue& arguments,
                   base::ListValue* result,
                   DispatcherHost* dispatcher_host);

  // Try to close all windows (then will cause whole app to quit).
  static void CloseAllWindows(bool force = false, bool quit = false);

  // Quit the whole app.
  static void Quit(content::RenderProcessHost* rph = NULL);

  // Post "open" event.
  static void EmitOpenEvent(const std::string& path);

  // Post "reopen" event.
  // (This event is received when the user clicked the icon in the Dock).
  static void EmitReopenEvent();

  static void ClearCache(content::RenderProcessHost* render_view_host);
  static void SetProxyConfig(content::RenderProcessHost* render_process_host,
                             const std::string& proxy_config);

 private:
  App();
  DISALLOW_COPY_AND_ASSIGN(App);
};

}  // namespace nwapi

#endif  // CONTENT_NW_SRC_API_APP_APP_H_

