#include "content/nw/src/browser/menubar_controller.h"

#include "base/stl_util.h"
#include "content/nw/src/browser/menubar_view.h"
#include "ui/base/models/menu_model.h"
#include "ui/views/controls/button/menu_button.h"
#include "ui/views/controls/menu/menu_item_view.h"
#include "ui/views/widget/widget.h"

namespace nw {

MenuBarController::ModelToMenuMap MenuBarController::model_to_menu_map_;
MenuBarController* MenuBarController::master_;

MenuBarController::MenuBarController(MenuBarView* menubar, ui::MenuModel* menu_model, MenuBarController* master)
  :MenuModelAdapter(menu_model), menubar_(menubar), active_menu_model_(menu_model) {

  views::MenuItemView* menu = MenuBarController::CreateMenu(menubar, menu_model, this);
  if (!master) {
    master_ = this;
    menu_runner_.reset(new views::MenuRunner(menu, views::MenuRunner::HAS_MNEMONICS));
  }
}

MenuBarController::~MenuBarController() {
  if (master_ == this) {
    STLDeleteElements(&controllers_);
    model_to_menu_map_.clear();
  }
}

views::MenuItemView* MenuBarController::GetSiblingMenu(
      views::MenuItemView* menu,
      const gfx::Point& screen_point,
      views::MenuAnchorPosition* anchor,
      bool* has_mnemonics,
      views::MenuButton** button) {
  if (!menubar_)
    return NULL;
  gfx::Point menubar_loc(screen_point);
  views::View::ConvertPointFromScreen(menubar_, &menubar_loc);
  ui::MenuModel* model;
  if (!menubar_->GetMenuButtonAtLocation(menubar_loc, &model, button))
    return NULL;

  *has_mnemonics = false;
  *anchor = views::MENU_ANCHOR_TOPLEFT;
  active_menu_model_ = model;
  if (!model_to_menu_map_[model]) {
    MenuBarController* controller = new MenuBarController(menubar_, model, master_);
    CreateMenu(menubar_, model, controller);
    controllers_.push_back(controller);
  }

  return model_to_menu_map_[model];
}

void MenuBarController::ExecuteCommand(int id) {
  ui::MenuModel* model = active_menu_model_;
  int index = 0;
  if (ui::MenuModel::GetModelAndIndexForCommandId(id, &model, &index)) {
    model->ActivatedAt(index);
    return;
  }

  NOTREACHED();
}

void MenuBarController::ExecuteCommand(int id, int mouse_event_flags) {
  ui::MenuModel* model = active_menu_model_;
  int index = 0;
  if (ui::MenuModel::GetModelAndIndexForCommandId(id, &model, &index)) {
    model->ActivatedAt(index, mouse_event_flags);
    return;
  }

  NOTREACHED();
}

views::MenuItemView* MenuBarController::CreateMenu(MenuBarView* menubar,
                                                   ui::MenuModel* model,
                                                   MenuBarController* controller) {
  views::MenuItemView* menu = new views::MenuItemView(controller);
  controller->BuildMenu(menu);
  model_to_menu_map_[model] = menu;

  return menu;
}

void MenuBarController::RunMenuAt(views::View* view, const gfx::Point& point) {

  views::MenuButton* menu_button = static_cast<views::MenuButton*>(view);
  gfx::Point screen_loc;
  views::View::ConvertPointToScreen(menu_button, &screen_loc);
  // Subtract 1 from the height to make the popup flush with the button border.
  gfx::Rect bounds(screen_loc.x(), screen_loc.y(), menu_button->width(),
                   menu_button->height() - 1);

  ignore_result(menu_runner_->RunMenuAt(view->GetWidget()->GetTopLevelWidget(),
                                       menu_button,
                                       bounds,
                                       views::MENU_ANCHOR_TOPLEFT,
                                       ui::MENU_SOURCE_NONE));
  delete this;
}

} //namespace nw
