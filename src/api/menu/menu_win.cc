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
#include "content/nw/src/api/dispatcher_host.h"
#include "content/nw/src/api/menuitem/menuitem.h"
#include "content/nw/src/browser/native_window_win.h"
#include "content/nw/src/nw_shell.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_view.h"
#include "skia/ext/image_operations.h"
#include "ui/gfx/gdi_util.h"
#include "ui/gfx/icon_util.h"
#include "ui/views/controls/menu/menu_2.h"
#include "ui/views/widget/widget.h"

#include "ui/views/focus/focus_manager.h"
#include "vector"

namespace {

HBITMAP GetNativeBitmapFromSkBitmap(const SkBitmap& bitmap) {
  int width = bitmap.width();
  int height = bitmap.height();

  BITMAPV4HEADER native_bitmap_header;
  gfx::CreateBitmapV4Header(width, height, &native_bitmap_header);

  HDC dc = ::GetDC(NULL);
  void* bits;
  HBITMAP native_bitmap = ::CreateDIBSection(dc,
      reinterpret_cast<BITMAPINFO*>(&native_bitmap_header),
      DIB_RGB_COLORS,
      &bits,
      NULL,
      0);
  DCHECK(native_bitmap);
  ::ReleaseDC(NULL, dc);
  bitmap.copyPixelsTo(bits, width * height * 4, width * 4);
  return native_bitmap;
}

} // namespace


namespace ui {

NwMenuModel::NwMenuModel(Delegate* delegate) : SimpleMenuModel(delegate) {
}

bool NwMenuModel::HasIcons() const {
  // Always return false, see the comment about |NwMenuModel|.
  return false;
}

} // namespace ui

namespace nwapi {

// The width of the icon for the menuitem
static const int kIconWidth = 16;
// The height of the icon for the menuitem
static const int kIconHeight = 16;

void Menu::Create(const base::DictionaryValue& option) {
  is_menu_modified_ = true;
  menu_delegate_.reset(new MenuDelegate(dispatcher_host()));
  menu_model_.reset(new ui::NwMenuModel(menu_delegate_.get()));
  menu_.reset(new views::NativeMenuWin(menu_model_.get(), NULL));

  focus_manager_ = NULL;

  std::string type;
  if (option.GetString("type", &type) && type == "menubar")
    menu_->set_is_popup_menu(false);
  menu_items_.empty();
}

void Menu::Destroy() {
  for (size_t index = 0; index < icon_bitmaps_.size(); ++index) {
    ::DeleteObject(icon_bitmaps_[index]);
  }
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
}

void Menu::Remove(MenuItem* menu_item, int pos) {
  menu_model_->RemoveAt(pos);
  is_menu_modified_ = true;
}

void Menu::Popup(int x, int y, content::Shell* shell) {
  Rebuild();

  // Map point from document to screen.
  POINT screen_point = { x, y };
  ClientToScreen((HWND)shell->web_contents()->GetView()->GetNativeView(),
                 &screen_point);

  menu_->RunMenuAt(gfx::Point(screen_point.x, screen_point.y),
                   views::Menu2::ALIGN_TOPLEFT);
}

void Menu::Rebuild(const HMENU *parent_menu) {
  if (is_menu_modified_) {
    // Refresh menu before show.
    menu_->Rebuild(NULL);
    menu_->UpdateStates();
    for (size_t index = 0; index < icon_bitmaps_.size(); ++index) {
      ::DeleteObject(icon_bitmaps_[index]);
    }
    icon_bitmaps_.clear();

    HMENU native_menu = parent_menu == NULL ?
        menu_->GetNativeMenu() : *parent_menu;

    for (int model_index = 0;
         model_index < menu_model_->GetItemCount();
         ++model_index) {
      int command_id = menu_model_->GetCommandIdAt(model_index);

      if (menu_model_->GetTypeAt(model_index) == ui::MenuModel::TYPE_COMMAND ||
          menu_model_->GetTypeAt(model_index) == ui::MenuModel::TYPE_SUBMENU) {
        gfx::Image icon;
        menu_model_->GetIconAt(model_index, &icon);
        if (!icon.IsEmpty()) {
          SkBitmap resized_bitmap =
              skia::ImageOperations::Resize(*icon.ToSkBitmap(),
                                            skia::ImageOperations::RESIZE_GOOD,
                                            kIconWidth,
                                            kIconHeight);
          HBITMAP icon_bitmap = GetNativeBitmapFromSkBitmap(resized_bitmap);
          ::SetMenuItemBitmaps(native_menu, command_id, MF_BYCOMMAND,
                               icon_bitmap, icon_bitmap);
          icon_bitmaps_.push_back(icon_bitmap);
        }
      }

      MenuItem* item = dispatcher_host()->GetApiObject<MenuItem>(command_id);
      if (item != NULL && item->submenu_) {
        item->submenu_->Rebuild(&native_menu);
      }
    }

    is_menu_modified_ = false;
  }
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



}  // namespace nwapi
