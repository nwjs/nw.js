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
#include "ui/events/event_constants.h"//for modifier key code
#include "base/logging.h"

namespace nw {

namespace {

static const int kIconWidth = 16;
static const int kIconHeight = 16;

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

  option.GetString("type", &type_);
  std::string label;
  option.GetString("label", &label);
  label_ = base::UTF8ToUTF16(label);
  std::string tooltip;
  option.GetString("tooltip", &tooltip);
  tooltip_ = base::UTF8ToUTF16(tooltip);
  is_checked_ = option.FindBoolKey("checked").value_or(false);
  is_enabled_ = option.FindBoolKey("enabled").value_or(false);

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
  if (submenu_) submenu_->RemoveKeys();

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
    if (submenu_ != NULL){
      submenu_->UpdateKeys(focus_manager);
    }
  }
}

void MenuItem::RemoveKeys() {
  if (!focus_manager_) return;

  if (enable_shortcut_) {
    focus_manager_->UnregisterAccelerator(accelerator_, this);
  }
  if (submenu_) {
    submenu_->RemoveKeys();
  }
  focus_manager_ = NULL;
}

#if defined(OS_WIN) || defined(OS_LINUX)
bool MenuItem::AcceleratorPressed(const ui::Accelerator& accelerator) {
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
  return enable_shortcut_ && is_enabled_;
}

#endif
}  // namespace nwapi

