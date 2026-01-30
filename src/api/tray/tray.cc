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
#include "content/nw/src/api/object_manager.h"
#include "content/nw/src/api/menu/menu.h"

namespace nw {

Tray::Tray(int id,
           const base::WeakPtr<ObjectManager>& object_manager,
           const base::DictValue& option,
           const std::string& extension_id)
  : Base(id, object_manager, option, extension_id) {
  Create(option);

  const std::string* title = option.FindString("title");
  if (title)
    SetTitle(*title);

  std::optional<bool> areTemplates = option.FindBool("iconsAreTemplates");
  if (areTemplates)
    SetIconsAreTemplates(*areTemplates);

  const std::string* icon = option.FindString("icon");
  if (icon && !icon->empty())
    SetIcon(*icon);

  const std::string* alticon = option.FindString("alticon");
  if (alticon && !alticon->empty())
    SetAltIcon(*alticon);

  const std::string* tooltip = option.FindString("tooltip");
  if (tooltip)
    SetTooltip(*tooltip);

  std::optional<int> menu_id = option.FindInt("menu");
  if (menu_id)
    SetMenu(object_manager->GetApiObject<Menu>(*menu_id));

  ShowAfterCreate();
}

Tray::~Tray() {
  Destroy();
}

void Tray::Call(const std::string& method,
                const base::ListValue& arguments,
                content::RenderFrameHost* rvh) {
  if (method == "SetTitle") {
    std::string title = arguments[0].GetString();
    SetTitle(title);
  } else if (method == "SetIcon") {
    std::string icon = arguments[0].GetString();
    SetIcon(icon);
  } else if (method == "SetAltIcon") {
    std::string alticon = arguments[0].GetString();
    SetAltIcon(alticon);
  } else if (method == "SetIconsAreTemplates") {
    bool areTemplates = arguments[0].GetBool();
    SetIconsAreTemplates(areTemplates);
  } else if (method == "SetTooltip") {
    std::string tooltip = arguments[0].GetString();
    SetTooltip(tooltip);
  } else if (method == "SetMenu") {
    int object_id = arguments[0].GetInt();
    SetMenu(object_manager()->GetApiObject<Menu>(object_id));
  } else if (method == "Remove") {
    Remove();
  } else {
    NOTREACHED() << "Invalid call to Tray method:" << method
                 << " arguments:" << arguments;
  }
}

}  // namespace nwapi
