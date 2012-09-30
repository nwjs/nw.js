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

#include "content/nw/src/api/menu/menu.h"

#include "base/values.h"
#include "content/nw/src/api/dispatcher_host.h"
#include "content/nw/src/api/menuitem/menuitem.h"
#include "content/nw/src/shell.h"

namespace api {

Menu::Menu(int id,
           DispatcherHost* dispatcher_host,
           const base::DictionaryValue& option)
    : Base(id, dispatcher_host, option) {
  Create(option);
}

Menu::~Menu() {
  Destroy();
}

void Menu::Call(const std::string& method,
                const base::ListValue& arguments) {
  if (method == "SetTitle") {
    std::string title;
    arguments.GetString(0, &title);
    SetTitle(title);
  } else if (method == "Append") {
    int object_id = 0;
    arguments.GetInteger(0, &object_id);
    Append(static_cast<MenuItem*>(dispatcher_host()->GetObject(object_id)));
  } else if (method == "Insert") {
    int object_id = 0;
    arguments.GetInteger(0, &object_id);
    int pos = 0;
    arguments.GetInteger(1, &pos);
    Insert(static_cast<MenuItem*>(dispatcher_host()->GetObject(object_id)),
           pos);
  } else if (method == "Remove") {
    int object_id = 0;
    arguments.GetInteger(0, &object_id);
    int pos_hint = 0;
    arguments.GetInteger(1, &pos_hint);
    Remove(static_cast<MenuItem*>(dispatcher_host()->GetObject(object_id)),
           pos_hint);
  } else if (method == "RemoveAt") {
    int pos = 0;
    arguments.GetInteger(0, &pos);
    RemoveAt(pos);
  } else if (method == "Popup") {
    int x = 0;
    arguments.GetInteger(0, &x);
    int y = 0;
    arguments.GetInteger(1, &y);
    Popup(x, y, content::Shell::FromRenderViewHost(
          dispatcher_host()->render_view_host()));
  } else {
    NOTREACHED() << "Invalid call to Menu method:" << method
                 << " arguments:" << arguments;
  }
}

}  // namespace api
