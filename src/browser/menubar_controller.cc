#include "content/nw/src/browser/menubar_controller.h"

#include "content/nw/src/browser/menubar_view.h"
#include "ui/views/controls/menu/menu_item_view.h"

namespace nw {

MenuBarController::ModelToMenuMap MenuBarController::model_to_menu_map_;

MenuBarController::MenuBarController(MenuBarView* menubar, ui::MenuModel* menu_model)
  :MenuModelAdapter(menu_model), menubar_(menubar) {
}

MenuBarController::~MenuBarController() {
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
  if (!model_to_menu_map_[model])
    CreateMenu(menubar_, model);

  return model_to_menu_map_[model];
}

views::MenuItemView* MenuBarController::CreateMenu(MenuBarView* menubar, ui::MenuModel* model) {
  MenuBarController* controller = new MenuBarController(menubar, model);
  views::MenuItemView* menu = new views::MenuItemView(controller);
  controller->BuildMenu(menu);
  model_to_menu_map_[model] = menu;

  return menu;
}

} //namespace nw
