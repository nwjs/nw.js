// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/menubar_view.h"

#include "content/nw/src/browser/menubar_controller.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/base/models/menu_model.h"
#include "ui/base/window_open_disposition.h"
#include "ui/gfx/text_elider.h"
#include "ui/views/controls/button/menu_button.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/widget/widget.h"
#include "ui/views/background.h"

using views::MenuRunner;

#if !defined(OS_WIN)
static const gfx::ElideBehavior kElideBehavior = gfx::FADE_TAIL;
#else
// Windows fade eliding causes text to darken; see http://crbug.com/388084
static const gfx::ElideBehavior kElideBehavior = gfx::ELIDE_TAIL;
#endif

namespace nw {

// MenuBarButton  -------------------------------------------------------

// Buttons used on the menu bar. Copied from BookmarkFolderButton

class MenuBarButton : public views::MenuButton {
 public:
  METADATA_HEADER(MenuBarButton);
  explicit MenuBarButton(const std::u16string& title,
                PressedCallback callback,
                bool show_menu_marker)
    : MenuButton(std::move(callback), title) {
    SetElideBehavior(kElideBehavior);
  }
  MenuBarButton(const MenuBarButton&) = delete;
  MenuBarButton& operator=(const MenuBarButton&) = delete;
  ~MenuBarButton() override;

  std::u16string GetTooltipText(const gfx::Point& p) const override {
    if (label()->GetPreferredSize().width() > label()->size().width())
      return GetText();
    return std::u16string();
  }
};

MenuBarButton::~MenuBarButton() {
}

BEGIN_METADATA(MenuBarButton)
END_METADATA

MenuBarView::MenuBarView() {
  auto layout = std::make_unique<views::BoxLayout>(views::BoxLayout::Orientation::kHorizontal, gfx::Insets(), 0);
  SetLayoutManager(std::move(layout));
}

void MenuBarView::OnThemeChanged() {
  views::AccessiblePaneView::OnThemeChanged();
  const ui::ColorProvider* color_provider = GetColorProvider();
  SkColor theme_color =
    color_provider->GetColor(ui::kColorMenuBackground);
  SetBackground(views::CreateSolidBackground(theme_color));
}

MenuBarView::~MenuBarView() {
}

void MenuBarView::UpdateMenu(ui::MenuModel* model) {
  RemoveAllChildViews();
  InitView(model);
  LayoutImmediately();
  PreferredSizeChanged();
  SchedulePaint();
}

void MenuBarView::InitView(ui::MenuModel* model) {
  model_ = model;
  for (size_t i = 0; i < model_->GetItemCount(); i++) {
    AddChildView(new MenuBarButton(model_->GetLabelAt(i),
                                   base::BindRepeating(&MenuBarView::ButtonPressed,
                                                       base::Unretained(this), i),
                                   false));
  }
}

bool MenuBarView::GetMenuButtonAtLocation(const gfx::Point& loc, ui::MenuModel** model, views::MenuButton** button) {
  if (!model_)
    return false;
  if (loc.x() < 0 || loc.x() >= width() || loc.y() < 0 || loc.y() >= height())
    return false;
  for (size_t i = 0; i < model_->GetItemCount(); i++) {
    views::View* child = children()[i];
    if (child->bounds().Contains(loc) &&
        (model_->GetTypeAt(i) == ui::MenuModel::TYPE_SUBMENU)) {
      *model = model_->GetSubmenuModelAt(i);
      *button = static_cast<views::MenuButton*>(child);
      return true;
    }
  }
  return false;
}

void MenuBarView::ButtonPressed(int button_index,
                                const ui::Event& event) {
  views::View* view = children()[button_index];
  DCHECK_NE(-1, button_index);
  ui::MenuModel::ItemType type = model_->GetTypeAt(button_index);
  if (type == ui::MenuModel::TYPE_SUBMENU) {
    MenuBarController* controller = new MenuBarController(this, model_->GetSubmenuModelAt(button_index), NULL);
    controller->RunMenuAt(view);
  }
  model_->ActivatedAt(button_index, event.flags());
}

BEGIN_METADATA(MenuBarView, views::AccessiblePaneView)
END_METADATA

} //namespace nw
