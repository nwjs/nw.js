#include "content/nw/src/browser/menubar_controller.h"

#include "base/run_loop.h"
#include "base/stl_util.h"
#include "content/nw/src/browser/menubar_view.h"
#include "ui/base/models/menu_model.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/views/controls/button/menu_button.h"
#include "ui/views/controls/menu/menu_item_view.h"
#include "ui/views/widget/widget.h"

namespace nw {

MenuBarController::ModelToMenuMap MenuBarController::model_to_menu_map_;
MenuBarController* MenuBarController::master_;

MenuBarController::MenuBarController(MenuBarView* menubar, ui::MenuModel* menu_model, MenuBarController* master)
  :MenuModelAdapter(menu_model), menubar_(menubar), active_menu_model_(menu_model) {

  MenuBarController::CreateMenu(menubar, menu_model, this);
  if (!master) {
    master_ = this;
    menu_runner_.reset(new views::MenuRunner(menu_model,
                                             views::MenuRunner::HAS_MNEMONICS,
                                             base::Bind(&MenuBarController::OnMenuClose,
                                                        base::Unretained(this))));
  }
}

MenuBarController::~MenuBarController() {
  if (master_ == this) {
    for (auto* ctr : controllers_)
      delete ctr;
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
  master_->active_menu_model_ = model;
  if (!model_to_menu_map_[model]) {
    MenuBarController* controller = new MenuBarController(menubar_, model, master_);
    CreateMenu(menubar_, model, controller);
    controllers_.push_back(controller);
  }

  return model_to_menu_map_[model];
}

void MenuBarController::ExecuteCommand(int id) {
  ui::MenuModel* model = master_->active_menu_model_;
  int index = 0;
  if (ui::MenuModel::GetModelAndIndexForCommandId(id, &model, &index)) {
    model->ActivatedAt(index);
    return;
  }

  NOTREACHED();
}

void MenuBarController::ExecuteCommand(int id, int mouse_event_flags) {
  ui::MenuModel* model = master_->active_menu_model_;
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

  menu_runner_->RunMenuAt(view->GetWidget()->GetTopLevelWidget(),
                                       menu_button,
                                       bounds,
                                       views::MENU_ANCHOR_TOPLEFT,
                                       ui::MENU_SOURCE_NONE);
  {
    base::AutoReset<base::Closure> reset_quit_closure(&message_loop_quit_,
                                                      base::Closure());
  
    base::MessageLoop* loop = base::MessageLoop::current();
    base::MessageLoop::ScopedNestableTaskAllower allow(loop);
    base::RunLoop run_loop;
    message_loop_quit_ = run_loop.QuitClosure();
  
    run_loop.Run();
  }
  delete this;
}

void MenuBarController::OnMenuClose() {
  CHECK(!message_loop_quit_.is_null());
  message_loop_quit_.Run();
  
#if !defined(OS_WIN)
  // Ask PlatformEventSource to stop dispatching
  // events in this message loop
  // iteration. We want our menu's loop to return
  // before the next event.
  if (ui::PlatformEventSource::GetInstance())
    ui::PlatformEventSource::GetInstance()->StopCurrentEventStream();
#endif
}

} //namespace nw
