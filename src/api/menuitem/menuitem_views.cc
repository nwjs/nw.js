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

#include "base/files/file_path.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/thread_restrictions.h"
#include "base/values.h"
#include "content/nw/src/api/object_manager.h"
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/nw_base.h"
#include "content/nw/src/nw_content.h"
#include "content/nw/src/nw_package.h"
#include "ui/base/accelerators/accelerator.h"
#include "ui/gfx/image/image_skia_operations.h"
#include "ui/gfx/text_utils.h"
#include "ui/events/event_constants.h"//for modifier key code
#include "ui/events/keycodes/keyboard_codes.h"
#include "base/logging.h"

namespace nw {

namespace {

static const int kIconWidth = 16;
static const int kIconHeight = 16;

bool GetAcceleratorFromLabel(const base::string16& label, ui::Accelerator* accelerator) {
  int pos = -1;
  int span = 0;
  base::string16 new_label = gfx::RemoveAcceleratorChar(label, '&', &pos, &span);
  if (span) {
    base::char16 ch = base::ToUpperASCII(new_label[pos]);
    if ((ch >= ui::KeyboardCode::VKEY_0 && ch <= ui::KeyboardCode::VKEY_9)
      || (ch >= ui::KeyboardCode::VKEY_A && ch <= ui::KeyboardCode::VKEY_Z)) {
      *accelerator = ui::Accelerator((ui::KeyboardCode)ch, ui::EF_ALT_DOWN);
      return true;
    }
  }
  return false;
}

} // namespace

void MenuItem::Create(const base::DictionaryValue& option) {
  is_modified_ = false;
  is_checked_ = false;
  is_enabled_ = true;
  type_ = "normal";
  submenu_ = NULL;
  meta_down_flag_ = false;

  focus_manager_ = NULL;
  menu_ = NULL;

  menubar_button_ = NULL;

  option.GetString("type", &type_);
  option.GetString("label", &label_);
  option.GetString("tooltip", &tooltip_);
  option.GetBoolean("checked", &is_checked_);
  option.GetBoolean("enabled", &is_enabled_);

  std::string key;
  std::string modifiers;
  option.GetString("key",&key);
  option.GetString("modifiers",&modifiers);

  ui::KeyboardCode keyval = ui::VKEY_UNKNOWN;

  keyval = GetKeycodeFromText(key);
  if (keyval == ui::VKEY_UNKNOWN){
    enable_shortcut_ = false;
  } else {
    enable_shortcut_ = true;
    //only code for ctrl, shift, alt, super and meta modifiers
    int modifiers_value = ui::EF_NONE;
    modifiers = base::ToLowerASCII(modifiers);
    if (modifiers.find("ctrl")!=std::string::npos){
      modifiers_value |= ui::EF_CONTROL_DOWN;
    }
    if (modifiers.find("shift")!=std::string::npos){
      modifiers_value |= ui::EF_SHIFT_DOWN ;
    }
    if (modifiers.find("alt")!=std::string::npos){
      modifiers_value |= ui::EF_ALT_DOWN;
    }
    if (modifiers.find("super")!=std::string::npos
     || modifiers.find("cmd")!=std::string::npos
     || modifiers.find("command")!=std::string::npos){
      modifiers_value |= ui::EF_COMMAND_DOWN;
    }
    if (modifiers.find("meta")!=std::string::npos){
      meta_down_flag_ = true;
    }
    accelerator_ = ui::Accelerator(keyval,modifiers_value);
  }

  if (GetAcceleratorFromLabel(label_, &accelerator_mnemonic_)) {
    enable_mnemonic_ = true;
  } else {
    enable_mnemonic_ = false;
  }

  std::string icon;
  if (option.GetString("icon", &icon) && !icon.empty())
    SetIcon(icon);

  int menu_id;
  if (option.GetInteger("submenu", &menu_id))
    SetSubmenu(object_manager()->GetApiObject<Menu>(menu_id));
}

void MenuItem::Destroy() {
}

void MenuItem::OnClick() {
  // Automatically flip checkbox.
  if (type_ == "checkbox")
    is_checked_ = !is_checked_;

  // Send event.
  base::ListValue args;
  object_manager()->SendEvent(this, "click", args);
}

void MenuItem::SetLabel(const std::string& label) {
  is_modified_ = true;
  label_ = base::UTF8ToUTF16(label);

  if (menu_)
    menu_->UpdateStates();
}

void MenuItem::SetIcon(const std::string& icon) {
  base::ThreadRestrictions::ScopedAllowIO allow_io;
  is_modified_ = true;
  if (icon.empty()) {
    icon_ = gfx::Image();
    return;
  }

  gfx::Image originImage;
  nw::Package* package = nw::InitNWPackage();
  if (nw::GetImage(package, base::FilePath::FromUTF8Unsafe(icon), &originImage)) {
    const gfx::ImageSkia* originImageSkia = originImage.ToImageSkia();
    gfx::ImageSkia resizedImageSkia = gfx::ImageSkiaOperations::CreateResizedImage(*originImageSkia,
                                                                                   skia::ImageOperations::RESIZE_GOOD,
                                                                                   gfx::Size(kIconWidth, kIconHeight));
    icon_ = gfx::Image(resizedImageSkia);
  }
}

void MenuItem::SetIconIsTemplate(bool isTemplate) {
}

void MenuItem::SetTooltip(const std::string& tooltip) {
  is_modified_ = true;
  tooltip_ = base::UTF8ToUTF16(tooltip);
  if (menu_)
    menu_->UpdateStates();
}

void MenuItem::SetEnabled(bool enabled) {
  is_enabled_ = enabled;
  if (menu_)
    menu_->UpdateStates();
}

void MenuItem::SetChecked(bool checked) {
  is_checked_ = checked;
  if (menu_)
    menu_->UpdateStates();
}

void MenuItem::SetSubmenu(Menu* menu) {
  submenu_ = menu;
}

bool MenuItem::GetChecked() {
  return is_checked_;
}

void MenuItem::UpdateKeys(views::FocusManager *focus_manager){
  if (focus_manager == NULL){
    return ;
  } else {
    focus_manager_ = focus_manager;
    if (enable_shortcut_){
      focus_manager->RegisterAccelerator(
        accelerator_,
        ui::AcceleratorManager::kHighPriority,
        this);
    }
    if (enable_mnemonic_ && menubar_button_) {
      focus_manager->RegisterAccelerator(
        accelerator_mnemonic_,
        ui::AcceleratorManager::kHighPriority,
        this);
    }
    if (submenu_ != NULL){
      submenu_->UpdateKeys(focus_manager);
    }
  }
}

#if defined(OS_WIN) || defined(OS_LINUX)
bool MenuItem::AcceleratorPressed(const ui::Accelerator& accelerator) {
  if (menubar_button_) {
    ui::KeyEvent event(accelerator.type(), accelerator.key_code(), accelerator.modifiers());
    if (menubar_button_->request_focus_on_press())
      menubar_button_->RequestFocus();
    menubar_button_->Activate(&event);
  }
#if defined(OS_WIN)
  if (meta_down_flag_) {
    if ((::GetKeyState(VK_APPS) & 0x8000) != 0x8000) {
      return true;
    }
  }
#endif
  OnClick();
  return true;
}

bool MenuItem::CanHandleAccelerators() const {
  return (enable_shortcut_ || enable_mnemonic_) && is_enabled_;
}

void MenuItem::SetMenuBarButton(views::MenuButton* menubar_button) {
  menubar_button_ = menubar_button;
}

#endif
}  // namespace nwapi

