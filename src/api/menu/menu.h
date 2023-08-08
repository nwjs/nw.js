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

#ifndef CONTENT_NW_SRC_API_MENU_MENU_H_
#define CONTENT_NW_SRC_API_MENU_MENU_H_ 

#include "base/compiler_specific.h"
#include "content/nw/src/api/base/base.h"

#include <string>
#include <vector>

#if defined(OS_MAC)
#if __OBJC__
@class NSMenu;
@class NWMenuDelegate;
#else
class NSMenu;
class NWMenuDelegate;
#endif  // __OBJC__

namespace nw {
class NativeWindowCocoa;
}

#elif defined(OS_WIN) || defined(OS_LINUX)
#include "content/nw/src/api/menu/menu_delegate.h"
#include "chrome/browser/status_icons/status_icon_menu_model.h"
#include "ui/views/focus/focus_manager.h"
namespace extensions {
class AppWindow;
}

namespace nw {
class Menu;
}

namespace ui {

// A derived class to override |HasIcons| to prevent the |NativeMenuWin| from
// being a owner-draw control.
// Note: this method maybe confused when the menuitem has an icon. I always
// return |false| here, and set the icon by using |SetMenuItemBitmaps|.
class NwMenuModel : public SimpleMenuModel {
 public:
  NwMenuModel(Delegate* delegate);

protected:
  friend class nw::Menu;
};

} // namespace ui

#endif

namespace aura {
class Window;
}

namespace content {
class RenderFrameHost;
class RenderFrameHost;
}

namespace views {
class MenuRunner;
}

namespace nw {

class MenuItem;

class Menu : public Base {
 public:
  Menu(int id,
       const base::WeakPtr<ObjectManager>& object_manager,
       const base::Value::Dict& option,
       const std::string& extension_id);
   ~Menu() override;

   void Call(const std::string& method,
             const base::Value::List& arguments,
             content::RenderFrameHost* rvh = nullptr) override;

#if defined(OS_MAC)
  static Menu* GetMenuFromNative(NSMenu* menu);
#endif

#if defined(OS_WIN) || defined(OS_LINUX)
  void UpdateKeys(views::FocusManager *focus_manager);
  void RemoveKeys();
  ui::NwMenuModel* model() { return menu_model_.get(); }
  aura::Window* GetActiveNativeView(content::RenderFrameHost* rfh);
#endif

  bool enable_show_event() { return enable_show_event_; }
  bool enable_show_event_;

  // Platform-independent implementations
  void Create(const base::Value::Dict& option);
  void Destroy();
  void Append(MenuItem* menu_item);
  void Insert(MenuItem* menu_item, int pos);
  void Remove(MenuItem* menu_item, int pos);
  void Popup(int x, int y, content::RenderFrameHost*);

#if defined(OS_MAC)
  RAW_PTR_EXCLUSION NSMenu* menu_;
  RAW_PTR_EXCLUSION NWMenuDelegate* menu_delegate_;
#elif defined(OS_LINUX) || defined(OS_WIN)

  void UpdateStates();

  void OnMenuClosed();

  //**Never Try to free this pointer**
  //We get it from top widget
  raw_ptr<views::FocusManager> focus_manager_;
  std::vector<MenuItem*> menu_items_;
  raw_ptr<extensions::AppWindow> window_;
  // Flag to indicate the menu has been modified since last show, so we should
  // rebuild the menu before next show.
  bool is_menu_modified_;

  std::unique_ptr<MenuDelegate> menu_delegate_;
  std::unique_ptr<ui::NwMenuModel> menu_model_;
  std::unique_ptr<views::MenuRunner> menu_runner_;
  base::RepeatingClosure message_loop_quit_;
#endif

};

}  // namespace nw

#endif  // CONTENT_NW_SRC_API_MENU_MENU_H_
