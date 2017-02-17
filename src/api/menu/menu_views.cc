// Copyright (c) 2012 Intel Corp
// Copyright (c) 2012 The Chromium Authors
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell co
// pies of the Software, and to permit persons to whom the Software is furnished
//  to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in al
// l copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IM
// PLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNES
// S FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
//  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WH
// ETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "content/nw/src/api/menu/menu.h"

#include "base/values.h"
#include "base/strings/utf_string_conversions.h"
#include "content/nw/src/api/object_manager.h"
#include "content/nw/src/api/menuitem/menuitem.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/web_contents.h"
#include "extensions/browser/app_window/app_window.h"
#include "skia/ext/image_operations.h"
#include "ui/aura/client/screen_position_client.h"
#include "ui/aura/window.h"
#include "ui/aura/window_tree_host.h"
#include "ui/views/controls/menu/menu_runner.h"
#include "ui/views/widget/widget.h"
#include "ui/views/focus/focus_manager.h"
#include "vector"

namespace ui {

NwMenuModel::NwMenuModel(Delegate* delegate) : SimpleMenuModel(delegate) {
}

bool NwMenuModel::HasIcons() const {
  // Always return false, see the comment about |NwMenuModel|.
  return false;
}

} // namespace ui

namespace nw {

void Menu::Create(const base::DictionaryValue& option) {
  is_menu_modified_ = true;
  menu_delegate_.reset(new MenuDelegate(object_manager()));
  menu_model_.reset(new ui::NwMenuModel(menu_delegate_.get()));

  focus_manager_ = NULL;
  window_ = NULL;

  std::string type;

  menu_items_.empty();
}

void Menu::Destroy() {
}

void Menu::Append(MenuItem* menu_item) {
  if (menu_item->submenu_)
    menu_model_->AddSubMenu(menu_item->id(), menu_item->label_,
                            menu_item->submenu_->menu_model_.get());
  else if (menu_item->type_ == "normal")
    menu_model_->AddItem(menu_item->id(), menu_item->label_);
  else if (menu_item->type_ == "checkbox")
    menu_model_->AddCheckItem(menu_item->id(), menu_item->label_);
  else if (menu_item->type_ == "separator")
    menu_model_->AddSeparator(ui::NORMAL_SEPARATOR);

  is_menu_modified_ = true;
  menu_items_.push_back(menu_item);
  menu_item->menu_ = this;
}

void Menu::Insert(MenuItem* menu_item, int pos) {
  if (menu_item->submenu_)
    menu_model_->InsertSubMenuAt(pos, menu_item->id(), menu_item->label_,
                                 menu_item->submenu_->menu_model_.get());
  else if (menu_item->type_ == "normal")
    menu_model_->InsertItemAt(pos, menu_item->id(), menu_item->label_);
  else if (menu_item->type_ == "checkbox")
    menu_model_->InsertCheckItemAt(pos, menu_item->id(), menu_item->label_);
  else if (menu_item->type_ == "separator")
    menu_model_->InsertSeparatorAt(pos, ui::NORMAL_SEPARATOR);

  is_menu_modified_ = true;
  menu_item->menu_ = this;

}

void Menu::Remove(MenuItem* menu_item, int pos) {
  menu_model_->RemoveItemAt(pos);
  is_menu_modified_ = true;
  menu_item->menu_ = NULL;
}

void Menu::Popup(int x, int y, content::RenderFrameHost* rfh) {
  // Rebuild();

  // Map point from document to screen.
  gfx::Point screen_point(x, y);

  // Convert from content coordinates to window coordinates.
  // This code copied from chrome_web_contents_view_delegate_views.cc
  aura::Window* target_window = GetActiveNativeView(rfh);
  aura::Window* root_window = target_window->GetRootWindow();
  views::Widget* top_level_widget =
    views::Widget::GetTopLevelWidgetForNativeView(target_window);
  aura::client::ScreenPositionClient* screen_position_client =
        aura::client::GetScreenPositionClient(root_window);
  if (screen_position_client) {
    screen_position_client->ConvertPointToScreen(target_window,
             &screen_point);
  }
  set_delay_destruction(true);
  views::MenuRunner runner(menu_model_.get(), views::MenuRunner::CONTEXT_MENU);
  ignore_result(
      runner.RunMenuAt(top_level_widget,
                       NULL,
                       gfx::Rect(screen_point, gfx::Size()),
                       views::MENU_ANCHOR_TOPRIGHT,
                       ui::MENU_SOURCE_NONE));
  set_delay_destruction(false);
  if (pending_destruction())
    object_manager_->OnDeallocateObject(id_);
}

void Menu::UpdateKeys(views::FocusManager *focus_manager){
  if (focus_manager == NULL){
    return ;
  } else {
    focus_manager_ = focus_manager;
    std::vector<MenuItem*>::iterator it = menu_items_.begin();
    while(it!=menu_items_.end()){
      (*it)->UpdateKeys(focus_manager);
      ++it;
    }
  }
}

void Menu::UpdateStates() {
}

aura::Window* Menu::GetActiveNativeView(content::RenderFrameHost* rfh) {
  content::WebContents* web_contents =
    content::WebContents::FromRenderFrameHost(rfh);
  if (!web_contents) {
    LOG(ERROR) << "Menu: couldn't find WebContents";
    return NULL;
  }
  return web_contents->GetFullscreenRenderWidgetHostView()
             ? web_contents->GetFullscreenRenderWidgetHostView()
                   ->GetNativeView()
             : web_contents->GetNativeView();
}

}  // namespace nw
