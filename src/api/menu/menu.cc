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
#include "content/nw/src/nw_shell.h"

namespace nwapi {

Menu::Menu(int id,
           const base::WeakPtr<DispatcherHost>& dispatcher_host,
           const base::DictionaryValue& option)
  : Base(id, dispatcher_host, option), enable_show_event_(false)  {
  Create(option);
}

Menu::~Menu() {
  Destroy();
}

void Menu::Call(const std::string& method,
                const base::ListValue& arguments) {
  if (method == "Append") {
    int object_id = 0;
    arguments.GetInteger(0, &object_id);
    Append(dispatcher_host()->GetApiObject<MenuItem>(object_id));
  } else if (method == "Insert") {
    int object_id = 0;
    arguments.GetInteger(0, &object_id);
    int pos = 0;
    arguments.GetInteger(1, &pos);
    Insert(dispatcher_host()->GetApiObject<MenuItem>(object_id), pos);
  } else if (method == "Remove") {
    int object_id = 0;
    arguments.GetInteger(0, &object_id);
    int pos = 0;
    arguments.GetInteger(1, &pos);
    Remove(dispatcher_host()->GetApiObject<MenuItem>(object_id), pos);
  } else if (method == "Popup") {
    int x = 0;
    arguments.GetInteger(0, &x);
    int y = 0;
    arguments.GetInteger(1, &y);
    Popup(x, y, content::Shell::FromRenderViewHost(
          dispatcher_host()->render_view_host()));
  } else if (method == "EnableShowEvent") {
    arguments.GetBoolean(0, &enable_show_event_);
  } else {
    NOTREACHED() << "Invalid call to Menu method:" << method
                 << " arguments:" << arguments;
  }
}

}  // namespace nwapi
