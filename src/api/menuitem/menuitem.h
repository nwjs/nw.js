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

#include <string>

#if defined(OS_MACOSX)
#if __OBJC__
@class NSMenuItem;
@class MenuItemDelegate;
#else
class NSMenuItem;
class MenuItemDelegate;
#endif  // __OBJC__
#elif defined(TOOLKIT_GTK)
#include <gtk/gtk.h>
#include "ui/base/gtk/gtk_signal.h"
#elif defined(OS_WIN)
#include "base/strings/string16.h"
#include "ui/gfx/image/image.h"
#endif  // defined(OS_MACOSX)

namespace nwapi {

class Menu;

class MenuItem : public Base {
 public:
  MenuItem(int id,
           const base::WeakPtr<DispatcherHost>& dispatcher_host,
           const base::DictionaryValue& option);
  virtual ~MenuItem();

  virtual void Call(const std::string& method,
                    const base::ListValue& arguments) OVERRIDE;

#if defined(OS_MACOSX) || defined(OS_WIN)
  void OnClick();
#endif

 private:
  friend class Menu;

  // Platform-independent implementations
  void Create(const base::DictionaryValue& option);
  void Destroy();
  void SetLabel(const std::string& label);
  void SetIcon(const std::string& icon);
  void SetTooltip(const std::string& tooltip);
  void SetEnabled(bool enabled);
  void SetChecked(bool checked);
  void SetSubmenu(Menu* sub_menu);

#if defined(OS_MACOSX)
  std::string type_;

  NSMenuItem* menu_item_;
  MenuItemDelegate* delegate_;
#elif defined(TOOLKIT_GTK)
  GtkWidget* menu_item_;

  // Don't send click event on active.
  bool block_active_;

  // Callback invoked when user left-clicks on the menu item.
  CHROMEGTK_CALLBACK_0(MenuItem, void, OnClick);
#elif defined(OS_WIN)
  friend class MenuDelegate;

  // Flag to indicate we need refresh.
  bool is_modified_;

  // Item attributes.
  bool is_checked_;
  bool is_enabled_;
  gfx::Image icon_;
  std::string type_;
  string16 label_;
  string16 tooltip_;
  Menu* submenu_;
#endif

  DISALLOW_COPY_AND_ASSIGN(MenuItem);
};

}  // namespace nwapi

#endif  // CONTENT_NW_SRC_API_MENUITEM_MENUITEM_H_
