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

#include "content/nw/src/api/menu/menu_delegate.h"

#include "base/logging.h"
#include "base/strings/string16.h"
#include "content/nw/src/api/object_manager.h"
#include "content/nw/src/api/menuitem/menuitem.h"

namespace nw {

MenuDelegate::MenuDelegate(ObjectManager* object_manager)
    : object_manager_(object_manager) {
}

MenuDelegate::~MenuDelegate() {
}

bool MenuDelegate::IsCommandIdChecked(int command_id) const {
  if (command_id < 0)
    return false;

  MenuItem* item = object_manager_->GetApiObject<MenuItem>(command_id);
  return item->is_checked_;
}

bool MenuDelegate::IsCommandIdEnabled(int command_id) const {
  if (command_id < 0)
    return false;

  MenuItem* item = object_manager_->GetApiObject<MenuItem>(command_id);
  if (!item)
    return false;
  return item->is_enabled_;
}

bool MenuDelegate::IsItemForCommandIdDynamic(int command_id) const {
  if (command_id < 0)
    return false;

  MenuItem* item = object_manager_->GetApiObject<MenuItem>(command_id);
  if (!item)
    return false;
  return item->is_modified_;
}

base::string16 MenuDelegate::GetLabelForCommandId(int command_id) const {
  MenuItem* item = object_manager_->GetApiObject<MenuItem>(command_id);
  return item->label_;
}


bool MenuDelegate::GetAcceleratorForCommandId(
      int command_id,
      ui::Accelerator* accelerator) const {
  MenuItem* item = object_manager_->GetApiObject<MenuItem>(command_id);
  if (!item)
    return false;

  if (!item->enable_shortcut_)
    return false;

  *accelerator = item->accelerator_;
  return true;
}

ui::ImageModel MenuDelegate::GetIconForCommandId(int command_id) const {
  MenuItem* item = object_manager_->GetApiObject<MenuItem>(command_id);
  if (!item)
    return ui::ImageModel();
  if (item->icon_.IsEmpty())
    return ui::ImageModel();

  return ui::ImageModel::FromImage(item->icon_);
}

void MenuDelegate::ExecuteCommand(int command_id, int event_flags) {
  if (command_id < 0)
    return;

  MenuItem* item = object_manager_->GetApiObject<MenuItem>(command_id);
  if (!item)
    return;
  item->OnClick();
}

bool MenuDelegate::HasIcon(int command_id) {
  if (command_id < 0)
    return false;

  MenuItem* item = object_manager_->GetApiObject<MenuItem>(command_id);
  if (!item)
    return false;
  return !item->icon_.IsEmpty();
}

}  // namespace nwapi
