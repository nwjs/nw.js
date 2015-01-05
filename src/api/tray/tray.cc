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

#include "content/nw/src/api/tray/tray.h"

#include "base/values.h"
#include "chrome/browser/status_icons/status_tray.h"
#include "content/nw/src/api/dispatcher_host.h"
#include "content/nw/src/api/menu/menu.h"

namespace nwapi {

Tray::Tray(int id,
           const base::WeakPtr<DispatcherHost>& dispatcher_host,
           const base::DictionaryValue& option)
    : Base(id, dispatcher_host, option) {
  Create(option);

  std::string title;
  if (option.GetString("title", &title))
    SetTitle(title);

  bool areTemplates;
  if (option.GetBoolean("iconsAreTemplates", &areTemplates))
    SetIconsAreTemplates(areTemplates);

  std::string icon;
  if (option.GetString("icon", &icon) && !icon.empty())
    SetIcon(icon);

  std::string alticon;
  if (option.GetString("alticon", &alticon) && !alticon.empty())
    SetAltIcon(alticon);

  std::string tooltip;
  if (option.GetString("tooltip", &tooltip))
    SetTooltip(tooltip);

  int menu_id;
  if (option.GetInteger("menu", &menu_id))
    SetMenu(dispatcher_host->GetApiObject<Menu>(menu_id));

  ShowAfterCreate();
}

Tray::~Tray() {
  Destroy();
}

void Tray::Call(const std::string& method,
                const base::ListValue& arguments) {
  if (method == "SetTitle") {
    std::string title;
    arguments.GetString(0, &title);
    SetTitle(title);
  } else if (method == "SetIcon") {
    std::string icon;
    arguments.GetString(0, &icon);
    SetIcon(icon);
  } else if (method == "SetAltIcon") {
    std::string alticon;
    arguments.GetString(0, &alticon);
    SetAltIcon(alticon);
  } else if (method == "SetIconsAreTemplates") {
    bool areTemplates;
    arguments.GetBoolean(0, &areTemplates);
    SetIconsAreTemplates(areTemplates);
  } else if (method == "SetTooltip") {
    std::string tooltip;
    arguments.GetString(0, &tooltip);
    SetTooltip(tooltip);
  } else if (method == "SetMenu") {
    int object_id = 0;
    arguments.GetInteger(0, &object_id);
    SetMenu(dispatcher_host()->GetApiObject<Menu>(object_id));
  } else if (method == "Remove") {
    Remove();
  } else {
    NOTREACHED() << "Invalid call to Tray method:" << method
                 << " arguments:" << arguments;
  }
}

}  // namespace nwapi
