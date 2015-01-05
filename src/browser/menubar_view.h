// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NW_BROWSER_MENUBAR_VIEWS_H_
#define NW_BROWSER_MENUBAR_VIEWS_H_

#include "base/strings/string16.h"
#include "ui/views/accessible_pane_view.h"
#include "ui/views/controls/button/button.h"
#include "ui/views/controls/button/menu_button_listener.h"

namespace views {
  class MenuRunner;
  class MenuButton;
}

namespace ui {
  class MenuModel;
}

namespace nw {

///////////////////////////////////////////////////////////////////////////////
//
//  copied from chrome/browser/ui/views/bookmarks/bookmark_bar_view.h
//
///////////////////////////////////////////////////////////////////////////////

class MenuBarView :
  public views::AccessiblePaneView,
  public views::MenuButtonListener,
  public views::ButtonListener {

 public:
  // The internal view class name.
  static const char kViewClassName[];

  // Maximum size of buttons
  static const int kMaxButtonWidth;

  // |browser_view| can be NULL during tests.
  MenuBarView();
  virtual ~MenuBarView();

  void UpdateMenu(ui::MenuModel* model);
  void InitView(ui::MenuModel* model);

  bool GetMenuButtonAtLocation(const gfx::Point& loc, ui::MenuModel** model, views::MenuButton** button);

  // views::MenuButtonListener:
  virtual void OnMenuButtonClicked(views::View* view,
                                   const gfx::Point& point) OVERRIDE;

  // views::ButtonListener:
  virtual void ButtonPressed(views::Button* sender,
                             const ui::Event& event) OVERRIDE;
  virtual void OnNativeThemeChanged(const ui::NativeTheme* theme) OVERRIDE;

 private:
  ui::MenuModel* model_;
  DISALLOW_COPY_AND_ASSIGN(MenuBarView);
};
} //namespace nw
#endif
