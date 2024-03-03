// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NW_BROWSER_MENUBAR_VIEWS_H_
#define NW_BROWSER_MENUBAR_VIEWS_H_

#include "ui/base/metadata/metadata_header_macros.h"
#include "ui/views/accessible_pane_view.h"
#include "ui/views/controls/button/button.h"

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

class MenuBarView : public views::AccessiblePaneView {
  METADATA_HEADER(MenuBarView, views::AccessiblePaneView)

 public:
  bool GetMenuButtonAtLocation(const gfx::Point& loc, ui::MenuModel** model, views::MenuButton** button);

  // Maximum size of buttons
  static const int kMaxButtonWidth;

  // |browser_view| can be NULL during tests.
  MenuBarView();
  ~MenuBarView() override;

  // views::View
  void OnThemeChanged() override;

  void UpdateMenu(ui::MenuModel* model);
  void InitView(ui::MenuModel* model);

  void ButtonPressed(int index, const ui::Event& event);

 private:
  raw_ptr<ui::MenuModel> model_;
};
} //namespace nw
#endif
