// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/menubar_view.h"


#include "content/nw/src/browser/menubar_controller.h"
#include "ui/base/models/menu_model.h"
#include "ui/base/window_open_disposition.h"
#include "ui/gfx/text_elider.h"
#include "ui/views/controls/button/menu_button.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/widget/widget.h"

using views::MenuRunner;

#if !defined(OS_WIN)
static const gfx::ElideBehavior kElideBehavior = gfx::FADE_TAIL;
#else
// Windows fade eliding causes text to darken; see http://crbug.com/388084
static const gfx::ElideBehavior kElideBehavior = gfx::ELIDE_TAIL;
#endif

namespace nw {

const char MenuBarView::kViewClassName[] = "BookmarkBarView";

// MenuBarButton  -------------------------------------------------------

// Buttons used on the menu bar. Copied from BookmarkFolderButton

class MenuBarButton : public views::MenuButton {
 public:
  MenuBarButton(const base::string16& title,
                views::MenuButtonListener* menu_button_listener,
                bool show_menu_marker)
      : MenuButton(title, menu_button_listener, show_menu_marker) {
    SetElideBehavior(kElideBehavior);
  }

  bool GetTooltipText(const gfx::Point& p,
                              base::string16* tooltip) const override {
    if (label()->GetPreferredSize().width() > label()->size().width())
      *tooltip = GetText();
    return !tooltip->empty();
  }

  void OnNativeThemeChanged(const ui::NativeTheme* theme) override {
    views::MenuButton::OnNativeThemeChanged(theme);
    SetEnabledTextColors(theme->GetSystemColor(ui::NativeTheme::kColorId_EnabledMenuItemForegroundColor));
  }

 private:

  DISALLOW_COPY_AND_ASSIGN(MenuBarButton);
};

MenuBarView::MenuBarView() {
  SetLayoutManager(new views::BoxLayout(views::BoxLayout::kHorizontal, gfx::Insets(), 0));
}

MenuBarView::~MenuBarView() {
}

void MenuBarView::UpdateMenu(ui::MenuModel* model) {
  RemoveAllChildViews(true);
  InitView(model);
  Layout();
  PreferredSizeChanged();
  SchedulePaint();
}

void MenuBarView::InitView(ui::MenuModel* model) {
  model_ = model;
  for (int i = 0; i < model_->GetItemCount(); i++) {
    AddChildView(new MenuBarButton(model_->GetLabelAt(i), this, false));
  }
}

bool MenuBarView::GetMenuButtonAtLocation(const gfx::Point& loc, ui::MenuModel** model, views::MenuButton** button) {
  if (!model_)
    return false;
  if (loc.x() < 0 || loc.x() >= width() || loc.y() < 0 || loc.y() >= height())
    return false;
  for (int i = 0; i < model_->GetItemCount(); i++) {
    views::View* child = child_at(i);
    if (child->bounds().Contains(loc) &&
        (model_->GetTypeAt(i) == ui::MenuModel::TYPE_SUBMENU)) {
      *model = model_->GetSubmenuModelAt(i);
      *button = static_cast<views::MenuButton*>(child);
      return true;
    }
  }
  return false;
}

void MenuBarView::OnMenuButtonClicked(views::MenuButton* view,
                                          const gfx::Point& point,
                                          const ui::Event* event) {
  int button_index = GetIndexOf(view);
  DCHECK_NE(-1, button_index);
  ui::MenuModel::ItemType type = model_->GetTypeAt(button_index);
  if (type == ui::MenuModel::TYPE_SUBMENU) {
    MenuBarController* controller = new MenuBarController(this, model_->GetSubmenuModelAt(button_index), NULL);
    controller->RunMenuAt(view, point);
  }
  model_->ActivatedAt(button_index, event->flags());
}

void MenuBarView::ButtonPressed(views::Button* sender,
                                const ui::Event& event) {
}

void MenuBarView::OnNativeThemeChanged(const ui::NativeTheme* theme) {
  // Use menu background color for menubar
  SetBackground(views::CreateSolidBackground(theme->
       GetSystemColor(ui::NativeTheme::kColorId_MenuBackgroundColor)));
  // Force to repaint the menubar
  SchedulePaint();
}

} //namespace nw
