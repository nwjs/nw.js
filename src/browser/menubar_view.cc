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
  MenuBarButton(views::ButtonListener* listener,
                const base::string16& title,
                views::MenuButtonListener* menu_button_listener,
                bool show_menu_marker)
      : MenuButton(listener, title, menu_button_listener, show_menu_marker) {
    SetElideBehavior(kElideBehavior);
  }

  bool GetTooltipText(const gfx::Point& p,
                              base::string16* tooltip) const override {
    if (label()->GetPreferredSize().width() > label()->size().width())
      *tooltip = GetText();
    return !tooltip->empty();
  }

  bool IsTriggerableEvent(const ui::Event& e) override {
    // Left clicks and taps should show the menu contents and right clicks
    // should show the context menu. They should not trigger the opening of
    // underlying urls.
    if (e.type() == ui::ET_GESTURE_TAP ||
        (e.IsMouseEvent() && (e.flags() &
             (ui::EF_LEFT_MOUSE_BUTTON | ui::EF_RIGHT_MOUSE_BUTTON))))
      return false;

    if (e.IsMouseEvent())
      return ui::DispositionFromEventFlags(e.flags()) != CURRENT_TAB;
    return false;
  }

 private:

  DISALLOW_COPY_AND_ASSIGN(MenuBarButton);
};

MenuBarView::MenuBarView() {
  SetLayoutManager(new views::BoxLayout(views::BoxLayout::kHorizontal, 0, 0, 0));
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
    AddChildView(new MenuBarButton(this, model_->GetLabelAt(i), this, false));
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

void MenuBarView::OnMenuButtonClicked(views::View* view,
                                      const gfx::Point& point) {
  int button_index = GetIndexOf(view);
  DCHECK_NE(-1, button_index);
  ui::MenuModel::ItemType type = model_->GetTypeAt(button_index);
  if (type == ui::MenuModel::TYPE_SUBMENU) {
    MenuBarController* controller = new MenuBarController(this, model_->GetSubmenuModelAt(button_index), NULL);
    controller->RunMenuAt(view, point);
  }
}

void MenuBarView::ButtonPressed(views::Button* sender,
                                const ui::Event& event) {
}

void MenuBarView::OnNativeThemeChanged(const ui::NativeTheme* theme) {
  set_background(views::Background::CreateSolidBackground(GetNativeTheme()->
       GetSystemColor(ui::NativeTheme::kColorId_MenuBackgroundColor)));
}

} //namespace nw
