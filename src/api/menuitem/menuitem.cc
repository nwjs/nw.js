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

#include "content/nw/src/api/menuitem/menuitem.h"

#include "base/logging.h"
#include "base/values.h"
#include "content/nw/src/api/dispatcher_host.h"
#include "content/nw/src/api/menu/menu.h"

#include <string.h>

namespace nwapi {

MenuItem::MenuItem(int id,
                   const base::WeakPtr<DispatcherHost>& dispatcher_host,
                   const base::DictionaryValue& option)
    : Base(id, dispatcher_host, option) {
  Create(option);
}

MenuItem::~MenuItem() {
  Destroy();
}

void MenuItem::Call(const std::string& method,
                    const base::ListValue& arguments) {
  if (method == "SetLabel") {
    std::string label;
    arguments.GetString(0, &label);
    SetLabel(label);
  } else if (method == "SetIcon") {
    std::string icon;
    arguments.GetString(0, &icon);
    SetIcon(icon);
  } else if (method == "SetIconIsTemplate") {
    bool isTemplate;
    arguments.GetBoolean(0, &isTemplate);
    SetIconIsTemplate(isTemplate);
  } else if (method == "SetTooltip") {
    std::string tooltip;
    arguments.GetString(0, &tooltip);
    SetTooltip(tooltip);
  } else if (method == "SetEnabled") {
    bool enabled = true;
    arguments.GetBoolean(0, &enabled);
    SetEnabled(enabled);
  } else if (method == "SetChecked") {
    bool checked = false;
    arguments.GetBoolean(0, &checked);
    SetChecked(checked);
  } else if (method == "SetSubmenu") {
    int object_id = 0;
    arguments.GetInteger(0, &object_id);
    SetSubmenu(dispatcher_host()->GetApiObject<Menu>(object_id));
#if defined(OS_MACOSX)
  } else if (method == "SetKey") {
    std::string key;
    arguments.GetString(0, &key);
    SetKey(key);
  } else if (method == "SetModifiers") {
    std::string mod;
    arguments.GetString(0, &mod);
    SetModifiers(mod);
#endif
  } else {
    NOTREACHED() << "Invalid call to MenuItem method:" << method
                 << " arguments:" << arguments;
  }
}

}  // namespace nwapi
