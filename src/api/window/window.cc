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

#include "content/nw/src/api/window/window.h"

#include "base/values.h"
#include "content/nw/src/api/dispatcher_host.h"
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/browser/native_window.h"
#include "content/nw/src/nw_shell.h"

namespace api {

Window::Window(int id,
               DispatcherHost* dispatcher_host,
               const base::DictionaryValue& option)
    : Base(id, dispatcher_host, option),
      shell_(content::Shell::FromRenderViewHost(dispatcher_host->
           render_view_host())) {
  // Set ID for Shell
  shell_->set_id(id);
}

Window::~Window() {
  // Window object got deleted when we launch new render view host and
  // delete the old one; at this time the Shell should be decoupled
  // with the renderer side
  shell_->set_id(-1);
}

void Window::Call(const std::string& method,
                  const base::ListValue& arguments) {
  if (method == "Show") {
    shell_->window()->Show();
  } else if (method == "Close") {
    bool force = false;
    arguments.GetBoolean(0, &force);
    shell_->set_force_close(force);
    shell_->window()->Close();
  } else if (method == "Hide") {
    shell_->window()->Hide();
  } else if (method == "Maximize") {
    shell_->window()->Maximize();
  } else if (method == "Unmaximize") {
    shell_->window()->Unmaximize();
  } else if (method == "Minimize") {
    shell_->window()->Minimize();
  } else if (method == "Restore") {
    shell_->window()->Restore();
  } else if (method == "EnterFullscreen") {
    shell_->window()->SetFullscreen(true);
  } else if (method == "LeaveFullscreen") {
    shell_->window()->SetFullscreen(false);
  } else if (method == "ToggleFullscreen") {
    shell_->window()->SetFullscreen(!shell_->window()->IsFullscreen());
  } else if (method == "EnterKioskMode") {
    shell_->window()->SetKiosk(true);
  } else if (method == "LeaveKioskMode") {
    shell_->window()->SetKiosk(false);
  } else if (method == "ToggleKioskMode") {
    shell_->window()->SetKiosk(!shell_->window()->IsKiosk());
  } else if (method == "ShowDevTools") {
    shell_->ShowDevTools();
  } else if (method == "ResizeTo") {
    int width, height;
    if (arguments.GetInteger(0, &width) &&
        arguments.GetInteger(1, &height))
      shell_->window()->SetSize(gfx::Size(width, height));
  } else if (method == "SetMaximumSize") {
    int width, height;
    if (arguments.GetInteger(0, &width) &&
        arguments.GetInteger(1, &height))
      shell_->window()->SetMaximumSize(width, height);
  } else if (method == "SetMinimumSize") {
    int width, height;
    if (arguments.GetInteger(0, &width) &&
        arguments.GetInteger(1, &height))
      shell_->window()->SetMinimumSize(width, height);
  } else if (method == "SetResizable") {
    bool resizable;
    if (arguments.GetBoolean(0, &resizable))
      shell_->window()->SetResizable(resizable);
  } else if (method == "SetAlwaysOnTop") {
    bool top;
    if (arguments.GetBoolean(0, &top))
      shell_->window()->SetAlwaysOnTop(top);
  } else if (method == "MoveTo") {
    int x, y;
    if (arguments.GetInteger(0, &x) &&
        arguments.GetInteger(1, &y))
      shell_->window()->SetPosition(gfx::Point(x, y));
  } else if (method == "RequestAttention") {
    bool flash;
    if (arguments.GetBoolean(0, &flash))
      shell_->window()->FlashFrame(flash);
  } else if (method == "SetMenu") {
    int id;
    if (arguments.GetInteger(0, &id))
      shell_->window()->SetMenu(dispatcher_host()->GetApiObject<Menu>(id));
  } else if (method == "Reload") {
    int type;
    if (arguments.GetInteger(0, &type))
      shell_->Reload(static_cast<content::Shell::ReloadType>(type));
  } else if (method == "BeginOffclientMouseMove") {
    shell_->window()->BeginOffclientMouseMove();
  } else if (method == "EndOffclientMouseMove") {
    shell_->window()->EndOffclientMouseMove();
  } else if (method == "CapturePage") {
    std::string image_format_str;
    if (arguments.GetString(0, &image_format_str))
      shell_->window()->CapturePage(image_format_str);
  } else {
    NOTREACHED() << "Invalid call to Window method:" << method
                 << " arguments:" << arguments;
  }
}

void Window::CallSync(const std::string& method,
                      const base::ListValue& arguments,
                      base::ListValue* result) {
  if (method == "IsFullscreen") {
    result->AppendBoolean(shell_->window()->IsFullscreen());
  } else if (method == "IsKioskMode") {
    result->AppendBoolean(shell_->window()->IsKiosk());
  } else if (method == "IsTransparent") {
    result->AppendBoolean(shell_->window()->IsTransparent());
  } else if (method == "GetSize") {
    gfx::Size size = shell_->window()->GetSize();
    result->AppendInteger(size.width());
    result->AppendInteger(size.height());
  } else if (method == "GetPosition") {
    gfx::Point position = shell_->window()->GetPosition();
    result->AppendInteger(position.x());
    result->AppendInteger(position.y());
  } else if (method == "GetMousePosition") {
    gfx::Point position = shell_->window()->GetMousePosition();
    result->AppendInteger(position.x());
    result->AppendInteger(position.y());
  } else {
    NOTREACHED() << "Invalid call to Window method:" << method
                 << " arguments:" << arguments;
  }
}

}  // namespace api
