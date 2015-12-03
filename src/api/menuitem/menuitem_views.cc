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
#include "base/strings/utf_string_conversions.h"
#include "base/threading/thread_restrictions.h"
#include "base/values.h"
#include "content/nw/src/api/object_manager.h"
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/nw_base.h"
#include "content/nw/src/nw_content.h"
#include "content/nw/src/nw_package.h"
#include "ui/base/accelerators/accelerator.h"
#include "ui/events/event_constants.h"//for modifier key code
#include "ui/events/keycodes/keyboard_codes.h"//for keycode
#include "base/logging.h"

ui::KeyboardCode GetKeycodeFromText(std::string text);

namespace nw {

void MenuItem::Create(const base::DictionaryValue& option) {
  is_modified_ = false;
  is_checked_ = false;
  is_enabled_ = true;
  type_ = "normal";
  submenu_ = NULL;
  super_down_flag_ = false;
  meta_down_flag_ = false;

  focus_manager_ = NULL;
  menu_ = NULL;

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


  if (key.size() == 0){
    enable_shortcut_ = false;
  } else {
    enable_shortcut_ = true;
    keyval = ::GetKeycodeFromText(key);
  }

  //only code for ctrl, shift, alt, super and meta modifiers
  int modifiers_value = ui::EF_NONE;
  if (modifiers.find("ctrl")!=std::string::npos){
    modifiers_value = modifiers_value | ui::EF_CONTROL_DOWN;
  }
  if (modifiers.find("shift")!=std::string::npos){
    modifiers_value = modifiers_value | ui::EF_SHIFT_DOWN ;
  }
  if (modifiers.find("alt")!=std::string::npos){
    modifiers_value = modifiers_value | ui::EF_ALT_DOWN;
  }
  if (modifiers.find("super")!=std::string::npos){
    super_down_flag_ = true;
  }
  if (modifiers.find("meta")!=std::string::npos){
    meta_down_flag_ = true;
  }
  accelerator_ = ui::Accelerator(keyval,modifiers_value);


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

  nw::Package* package = nw::InitNWPackage();
  nw::GetImage(package, base::FilePath::FromUTF8Unsafe(icon), &icon_);
}

void MenuItem::SetIconIsTemplate(bool isTemplate) {
}

void MenuItem::SetTooltip(const std::string& tooltip) {
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

#if defined(OS_WIN) || defined(OS_LINUX)
bool MenuItem::AcceleratorPressed(const ui::Accelerator& accelerator) {

#if defined(OS_WIN)
  if (super_down_flag_){
    if ( ( (::GetKeyState(VK_LWIN) & 0x8000) != 0x8000)
         || ( (::GetKeyState(VK_LWIN) & 0x8000) != 0x8000) ){
      return true;
    }
  }
  if (meta_down_flag_){
    if ( (::GetKeyState(VK_APPS) & 0x8000) != 0x8000 ){
      return true;
    }
  }
#endif
  OnClick();
  return true;
}

bool MenuItem::CanHandleAccelerators() const {
  return true;
}

#endif
}  // namespace nwapi



ui::KeyboardCode GetKeycodeFromText(std::string text){
  ui::KeyboardCode retval = ui::VKEY_UNKNOWN;
  if (text.size() != 0){
    for (unsigned int i=0;i<text.size();++i){
      text[i] = ::toupper(text[i]);
    }
    if (text.size() == 1){
      char key = text[0];
      if (key>='0' && key<='9'){//handle digital
        retval = (ui::KeyboardCode)(ui::VKEY_0 + key - '0');
      } else if (key>='A'&&key<='Z'){//handle alphabet
        retval = (ui::KeyboardCode)(ui::VKEY_A + key - 'A');
      } else if (key == '`'){//handle all special symbols
        retval = ui::VKEY_OEM_3;
      } else if (key == ','){
        retval = ui::VKEY_OEM_COMMA;
      } else if (key == '.'){
        retval = ui::VKEY_OEM_PERIOD;
      } else if (key == '/'){
        retval = ui::VKEY_OEM_2;
      } else if (key == ';'){
        retval = ui::VKEY_OEM_1;
      } else if (key == '\''){
        retval = ui::VKEY_OEM_7;
      } else if (key == '['){
        retval = ui::VKEY_OEM_4;
      } else if (key == ']'){
        retval = ui::VKEY_OEM_6;
      } else if (key == '\\'){
        retval = ui::VKEY_OEM_5;
      } else if (key == '-'){
        retval = ui::VKEY_OEM_MINUS;
      } else if (key == '='){
        retval = ui::VKEY_OEM_PLUS;
      }
    } else {//handle long key name
      if (!text.compare("ESC")){
        retval = ui::VKEY_ESCAPE;
      } else if (!text.compare("BACKSPACE")){
        retval = ui::VKEY_BACK;
      } else if (!text.compare("F1")){
        retval = ui::VKEY_F1;
      } else if (!text.compare("F2")){
        retval = ui::VKEY_F2;
      } else if (!text.compare("F3")){
        retval = ui::VKEY_F3;
      } else if (!text.compare("F4")){
        retval = ui::VKEY_F4;
      } else if (!text.compare("F5")){
        retval = ui::VKEY_F5;
      } else if (!text.compare("F6")){
        retval = ui::VKEY_F6;
      } else if (!text.compare("F7")){
        retval = ui::VKEY_F7;
      } else if (!text.compare("F8")){
        retval = ui::VKEY_F8;
      } else if (!text.compare("F9")){
        retval = ui::VKEY_F9;
      } else if (!text.compare("F10")){
        retval = ui::VKEY_F10;
      } else if (!text.compare("F11")){
        retval = ui::VKEY_F11;
      } else if (!text.compare("F12")){
        retval = ui::VKEY_F12;
      } else if (!text.compare("INSERT")){
        retval = ui::VKEY_INSERT;
      } else if (!text.compare("HOME")){
        retval = ui::VKEY_HOME;
      } else if (!text.compare("DELETE")){
        retval = ui::VKEY_DELETE;
      } else if (!text.compare("END")){
        retval = ui::VKEY_END;
      } else if (!text.compare("PAGEUP")){
        retval = ui::VKEY_PRIOR;
      } else if (!text.compare("PAGEDOWN")){
        retval = ui::VKEY_NEXT;
      } else if (!text.compare("UP")){
        retval = ui::VKEY_UP;
      } else if (!text.compare("LEFT")){
        retval = ui::VKEY_LEFT;
      } else if (!text.compare("DOWN")){
        retval = ui::VKEY_DOWN;
      } else if (!text.compare("RIGHT")){
        retval = ui::VKEY_RIGHT;
      }
    }
  }
  return retval;
}
