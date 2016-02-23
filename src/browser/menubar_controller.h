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
  ~MenuBarController() override;

  static views::MenuItemView* CreateMenu(MenuBarView* menubar, ui::MenuModel* model, MenuBarController* controller);
  void RunMenuAt(views::View* view, const gfx::Point& point);

  views::MenuItemView* GetSiblingMenu(
      views::MenuItemView* menu,
      const gfx::Point& screen_point,
      views::MenuAnchorPosition* anchor,
      bool* has_mnemonics,
      views::MenuButton** button) override;

  void ExecuteCommand(int id) override;
  void ExecuteCommand(int id, int mouse_event_flags) override;

 private:
  typedef std::map<const ui::MenuModel*, views::MenuItemView*> ModelToMenuMap;

  MenuBarView* menubar_;
  ui::MenuModel* active_menu_model_;
  scoped_ptr<views::MenuRunner> menu_runner_;
  std::vector<MenuBarController*> controllers_;
  static ModelToMenuMap model_to_menu_map_;
  static MenuBarController* master_;

  DISALLOW_COPY_AND_ASSIGN(MenuBarController);
};

} //namespace nw
#endif
