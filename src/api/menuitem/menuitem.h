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

#ifndef CONTENT_NW_SRC_API_MENUITEM_MENUITEM_H_
#define CONTENT_NW_SRC_API_MENUITEM_MENUITEM_H_

#include "base/compiler_specific.h"
#include "content/nw/src/api/base/base.h"
#include "ui/events/keycodes/dom/keycode_converter.h"
#include "ui/events/keycodes/keyboard_codes.h"//for keycode
#include "ui/events/keycodes/keyboard_code_conversion.h"

#include <string>

#if defined(OS_MAC)
#if __OBJC__
@class NSMenuItem;
@class MenuItemDelegate;
#else
class NSMenuItem;
class MenuItemDelegate;
#endif  // __OBJC__
#elif defined(OS_WIN) || defined(OS_LINUX)
#include "ui/gfx/image/image.h"
#include "ui/base/accelerators/accelerator.h"
#include "ui/views/focus/focus_manager.h"
#endif  // defined(OS_MAC)

namespace nw {

ui::KeyboardCode GetKeycodeFromText(std::string text);

class Menu;

#if defined(OS_WIN) || defined(OS_LINUX)
class MenuItem : public Base ,
                 public ui::AcceleratorTarget {
#else
class MenuItem : public Base {
#endif
 public:
  MenuItem(int id,
           const base::WeakPtr<ObjectManager>& dispatcher_host,
           const base::DictionaryValue& option,
           const std::string& extension_id);
  ~MenuItem() override;

  void Call(const std::string& method,
             const base::ListValue& arguments,
             content::RenderFrameHost* rvh = nullptr) override;
  void CallSync(const std::string& method,
                        const base::ListValue& arguments,
                        base::ListValue* result) override;

#if defined(OS_MAC)
  static std::unique_ptr<base::DictionaryValue> CreateFromNative(NSMenuItem* menu_item, Menu* menu, int index);
  static MenuItem* GetMenuItemFromNative(NSMenuItem* menu_item);
#endif

#if defined(OS_WIN) || defined(OS_LINUX)
  bool AcceleratorPressed(const ui::Accelerator& accelerator) override;
  bool CanHandleAccelerators() const override;
  void UpdateKeys(views::FocusManager *focus_manager);
  void RemoveKeys();
#endif

  void OnClick();

 private:
  friend class Menu;

  // Platform-independent implementations
  void Create(const base::DictionaryValue& option);
  void Destroy();
  void SetLabel(const std::string& label);
  void SetIcon(const std::string& icon);
  void SetTooltip(const std::string& tooltip);
  void SetKey(const std::string& key);
  void SetModifiers(const std::string& modifiers);
  void SetEnabled(bool enabled);
  void SetChecked(bool checked);
  void SetSubmenu(Menu* sub_menu);

  bool GetChecked();

  // Template icon works only on Mac OS X
  void SetIconIsTemplate(bool isTemplate);

#if defined(OS_MAC)
  std::string type_;

  NSMenuItem* menu_item_;
  MenuItemDelegate* delegate_;
  bool iconIsTemplate;
  bool native_;

#elif defined(OS_WIN) || defined(OS_LINUX)
  friend class MenuDelegate;

  Menu* menu_;
  //**Never Try to free this pointer**
  //We get it from top widget
  views::FocusManager *focus_manager_;

  ui::Accelerator accelerator_;

  // Flag to indicate we need refresh.
  bool is_modified_;

  // Item attributes.
  bool is_checked_;
  bool is_enabled_;
  gfx::Image icon_;
  std::string type_;
  std::u16string label_;
  std::u16string tooltip_;
  Menu* submenu_;
  bool enable_shortcut_;

  bool meta_down_flag_;

#endif

  DISALLOW_COPY_AND_ASSIGN(MenuItem);
};

}  // namespace nw

#endif  // CONTENT_NW_SRC_API_MENUITEM_MENUITEM_H_
