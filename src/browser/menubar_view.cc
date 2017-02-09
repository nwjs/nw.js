// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/menubar_view.h"

#include "content/nw/src/api/menuitem/menuitem.h"
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/browser/menubar_controller.h"
#include "ui/base/models/menu_model.h"
#include "ui/base/window_open_disposition.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/text_elider.h"
#include "ui/gfx/text_utils.h"
#include "ui/views/controls/button/menu_button.h"
#include "ui/views/controls/label.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/widget/widget.h"

#if defined(OS_WIN)
#include "ui/events/win/system_event_state_lookup.h"
#endif

using views::MenuRunner;

#if !defined(OS_WIN)
static const gfx::ElideBehavior kElideBehavior = gfx::FADE_TAIL;
#else
// Windows fade eliding causes text to darken; see http://crbug.com/388084
static const gfx::ElideBehavior kElideBehavior = gfx::ELIDE_TAIL;
#endif

namespace nw {

const char MenuBarView::kViewClassName[] = "MenuBarView";

// MenuBarButton  -------------------------------------------------------

// Buttons used on the menu bar. Copied from BookmarkFolderButton

class MenuBarButton : public views::MenuButton {
 public:
  MenuBarButton(const base::string16& title,
                views::MenuButtonListener* menu_button_listener,
                bool show_menu_marker)
      : MenuButton(base::string16(), menu_button_listener, show_menu_marker),
      title_(title) {
    int pos, span;
    SetText(gfx::RemoveAcceleratorChar(title, '&', &pos, &span));

    SetElideBehavior(kElideBehavior);
    set_request_focus_on_press(true);
    SetFocusForPlatform();
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

  void OnPaint(gfx::Canvas* canvas) override {
#if defined(OS_WIN)
    bool paint_mnemonic = ui::win::IsAltPressed();
#else
    bool paint_mnemonic = true;
#endif
    if(paint_mnemonic)
      PaintWithMnemonic(canvas);
    else
      views::MenuButton::OnPaint(canvas);
  }

 private:

  void PaintWithMnemonic(gfx::Canvas* canvas) {
    // make labels rendering nothing
    SkColor text_color = label()->enabled_color();
    SetEnabledTextColors(SK_ColorTRANSPARENT);
    views::MenuButton::OnPaint(canvas);
    SetEnabledTextColors(text_color);

    // draw text with underlines if ALT is down
    canvas->DrawStringRectWithFlags(title_,
                                    GetFontList(),
                                    text_color,
                                    GetContentsBounds(),
                                    gfx::Canvas::TEXT_ALIGN_CENTER | gfx::Canvas::SHOW_PREFIX);
  }
  base::string16 title_;
  DISALLOW_COPY_AND_ASSIGN(MenuBarButton);
};

MenuBarView::MenuBarView() {
  SetLayoutManager(new views::BoxLayout(views::BoxLayout::kHorizontal, 0, 0, 0));
}

MenuBarView::~MenuBarView() {
}

void MenuBarView::UpdateMenu(Menu* menu) {
  RemoveAllChildViews(true);
  InitView(menu);
  Layout();
  PreferredSizeChanged();
  SchedulePaint();
}

void MenuBarView::InitView(Menu* menu) {
  model_ = menu->model();
  for (int i = 0; i < model_->GetItemCount(); i++) {
    MenuBarButton* button = new MenuBarButton(model_->GetLabelAt(i), this, false);
    menu->menu_items_[i]->SetMenuBarButton(button);
    AddChildView(button);
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
  set_background(views::Background::CreateSolidBackground(theme->
       GetSystemColor(ui::NativeTheme::kColorId_MenuBackgroundColor)));
  // Force to repaint the menubar
  SchedulePaint();
}

} //namespace nw
