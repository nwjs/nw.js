#ifndef NW_BROWSER_MENUBAR_CONTROLLER_H
#define NW_BROWSER_MENUBAR_CONTROLLER_H

#include "ui/views/controls/menu/menu_model_adapter.h"

#include <map>

namespace ui {
class MenuModel;
}

namespace nw {
class MenuBarView;

class MenuBarController : public views::MenuModelAdapter {
 public:
  MenuBarController(MenuBarView* menubar, ui::MenuModel* menu_model);
  virtual ~MenuBarController();

  static views::MenuItemView* CreateMenu(MenuBarView* menubar, ui::MenuModel* model);

  virtual views::MenuItemView* GetSiblingMenu(
      views::MenuItemView* menu,
      const gfx::Point& screen_point,
      views::MenuAnchorPosition* anchor,
      bool* has_mnemonics,
      views::MenuButton** button) OVERRIDE;

 private:
  typedef std::map<const ui::MenuModel*, views::MenuItemView*> ModelToMenuMap;

  MenuBarView* menubar_;
  static ModelToMenuMap model_to_menu_map_;

  DISALLOW_COPY_AND_ASSIGN(MenuBarController);
};

} //namespace nw
#endif
