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
#include "base/memory/scoped_ptr.h"
#include "content/nw/src/api/base/base.h"

#include <string>
#include <vector>

#if defined(OS_WIN)
#include "ui/views/controls/menu/native_menu_win.h"
#endif

#if defined(OS_MACOSX)
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
namespace nw {
class NativeWindowAura;
}

namespace nwapi {
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

  // Overridden from MenuModel:
   bool HasIcons() const override;

protected:
  friend class nwapi::Menu;
};

} // namespace ui

#endif

namespace content {
class Shell;
}

namespace nwapi {

class MenuItem;

class Menu : public Base {
 public:
  Menu(int id,
       const base::WeakPtr<DispatcherHost>& dispatcher_host,
       const base::DictionaryValue& option);
   ~Menu() override;

   void Call(const std::string& method,
                    const base::ListValue& arguments) override;

#if defined(OS_WIN) || defined(OS_LINUX)
  void UpdateKeys(views::FocusManager *focus_manager);
  ui::NwMenuModel* model() { return menu_model_.get(); }
#endif

  bool enable_show_event() { return enable_show_event_; }
 protected:
  bool enable_show_event_;

 private:
  friend class MenuItem;
  friend class Tray;

  // Platform-independent implementations
  void Create(const base::DictionaryValue& option);
  void Destroy();
  void Append(MenuItem* menu_item);
  void Insert(MenuItem* menu_item, int pos);
  void Remove(MenuItem* menu_item, int pos);
  void Popup(int x, int y, content::Shell*);

#if defined(OS_LINUX)
  std::vector<MenuItem*> menu_items;
#endif

#if defined(OS_MACOSX)
  friend class nw::NativeWindowCocoa;
  NSMenu* menu_;
  NWMenuDelegate* menu_delegate_;
#elif defined(OS_LINUX)
  friend class nw::NativeWindowAura;

  views::FocusManager *focus_manager_;
  std::vector<MenuItem*> menu_items_;
  nw::NativeWindowAura* window_;
  // Flag to indicate the menu has been modified since last show, so we should
  // rebuild the menu before next show.
  bool is_menu_modified_;

  scoped_ptr<MenuDelegate> menu_delegate_;
  scoped_ptr<ui::NwMenuModel> menu_model_;
  void UpdateStates();

#elif defined(OS_WIN)
  friend class nw::NativeWindowAura;

  void Rebuild(const HMENU *parent_menu = NULL);
  void UpdateStates();
  void SetWindow(nw::NativeWindowAura* win);

  //**Never Try to free this pointer**
  //We get it from top widget
  views::FocusManager *focus_manager_;
  std::vector<MenuItem*> menu_items_;
  nw::NativeWindowAura* window_;
  // Flag to indicate the menu has been modified since last show, so we should
  // rebuild the menu before next show.
  bool is_menu_modified_;

  scoped_ptr<MenuDelegate> menu_delegate_;
  scoped_ptr<ui::NwMenuModel> menu_model_;
  scoped_ptr<views::NativeMenuWin> menu_;

  // A container for the handles of the icon bitmap.
  std::vector<HBITMAP> icon_bitmaps_;
#endif

  DISALLOW_COPY_AND_ASSIGN(Menu);
};

}  // namespace nwapi

#endif  // CONTENT_NW_SRC_API_MENU_MENU_H_
