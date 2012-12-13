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

#include "base/file_path.h"
#include "base/utf_string_conversions.h"
#include "base/values.h"
#include "content/nw/src/api/dispatcher_host.h"
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_shell.h"

namespace api {

void MenuItem::Create(const base::DictionaryValue& option) {
  is_modified_ = false;
  is_checked_ = false;
  is_enabled_ = true;
  type_ = "normal";
  submenu_ = NULL;

  option.GetString("type", &type_);
  option.GetString("label", &label_);
  option.GetString("tooltip", &tooltip_);
  option.GetBoolean("checked", &is_checked_);
  option.GetBoolean("enabled", &is_enabled_);

  std::string icon;
  if (option.GetString("icon", &icon) && !icon.empty())
    SetIcon(icon);

  int menu_id;
  if (option.GetInteger("submenu", &menu_id))
    SetSubmenu(dispatcher_host()->GetApiObject<Menu>(menu_id));
} 

void MenuItem::Destroy() {
}

void MenuItem::OnClick() {
  // Automatically flip checkbox.
  if (type_ == "checkbox")
    is_checked_ = !is_checked_;

  // Send event.
  base::ListValue args;
  dispatcher_host()->SendEvent(this, "click", args);
}

void MenuItem::SetLabel(const std::string& label) {
  is_modified_ = true;
  label_ = UTF8ToUTF16(label);
}

void MenuItem::SetIcon(const std::string& icon) {
  is_modified_ = true;
  if (icon.empty()) {
    icon_ = gfx::Image();
    return;
  }

  content::Shell* shell = content::Shell::FromRenderViewHost(
      dispatcher_host()->render_view_host());
  nw::Package* package = shell->GetPackage();
  package->GetImage(FilePath::FromUTF8Unsafe(icon), &icon_);
}

void MenuItem::SetTooltip(const std::string& tooltip) {
  tooltip_ = UTF8ToUTF16(tooltip);
}

void MenuItem::SetEnabled(bool enabled) {
  is_enabled_ = enabled;
}

void MenuItem::SetChecked(bool checked) {
  is_checked_ = checked;
}

void MenuItem::SetSubmenu(Menu* menu) {
  submenu_ = menu;
}

}  // namespace api
