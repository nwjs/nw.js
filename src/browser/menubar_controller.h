#ifndef NW_BROWSER_MENUBAR_CONTROLLER_H
#define NW_BROWSER_MENUBAR_CONTROLLER_H

#include "ui/views/controls/menu/menu_model_adapter.h"
#include "ui/views/controls/menu/menu_runner.h"
#include "ui/views/view.h"

#include <map>

namespace ui {
class MenuModel;
}

namespace nw {
class MenuBarView;

class MenuBarController : public views::MenuModelAdapter {
 public:
  MenuBarController(MenuBarView* menubar, ui::MenuModel* menu_model, MenuBarController* master);
  virtual ~MenuBarController();

  static views::MenuItemView* CreateMenu(MenuBarView* menubar, ui::MenuModel* model, MenuBarController* controller);
  void RunMenuAt(views::View* view, const gfx::Point& point);

  virtual views::MenuItemView* GetSiblingMenu(
      views::MenuItemView* menu,
      const gfx::Point& screen_point,
      views::MenuAnchorPosition* anchor,
      bool* has_mnemonics,
      views::MenuButton** button) OVERRIDE;

 private:
  typedef std::map<const ui::MenuModel*, views::MenuItemView*> ModelToMenuMap;

  MenuBarView* menubar_;
  scoped_ptr<views::MenuRunner> menu_runner_;
  std::vector<MenuBarController*> controllers_;
  static ModelToMenuMap model_to_menu_map_;
  static MenuBarController* master_;

  DISALLOW_COPY_AND_ASSIGN(MenuBarController);
};

} //namespace nw
#endif
