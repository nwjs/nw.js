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

#include "content/nw/src/api/tray/tray.h"

#include "base/values.h"
#include "content/nw/src/api/dispatcher_host.h"
#include "content/nw/src/api/menu/menu.h"

namespace nwapi {

void Tray::Create(const base::DictionaryValue& option) {
  menu_ = NULL;
  status_item_ = gtk_status_icon_new();
}

void Tray::ShowAfterCreate() {
  g_signal_connect(status_item_, "activate",
                   G_CALLBACK(OnClickThunk), this);
  g_signal_connect(status_item_, "popup-menu",
                   G_CALLBACK(OnPopupMenuThunk), this);
  gtk_status_icon_set_visible(status_item_, TRUE);
}

void Tray::Destroy() {
  // This object will be destroyed after Remove is called, so we don't release
  // in destructor but in the Remove function.
}

void Tray::SetTitle(const std::string& title) {
  gtk_status_icon_set_title(status_item_, title.c_str());
}

void Tray::SetIcon(const std::string& path) {
  gtk_status_icon_set_from_file(status_item_, path.c_str());
}

void Tray::SetTooltip(const std::string& tooltip) {
  gtk_status_icon_set_tooltip_text(GTK_STATUS_ICON(status_item_),
                                   tooltip.c_str());
}

void Tray::SetMenu(Menu* menu) {
  menu_ = menu;
}

void Tray::Remove() {
  g_object_unref(G_OBJECT(status_item_));
}

void Tray::OnClick(GtkWidget* widget) {
  base::ListValue args;
  dispatcher_host()->SendEvent(this, "click", args);
}

void Tray::OnPopupMenu(GtkWidget* widget, guint button, guint time) {
#if 0//FIXME
  if (menu_) {
    gtk_menu_popup(GTK_MENU(menu_->menu_), NULL, NULL,
                   gtk_status_icon_position_menu,
                   status_item_, button, time);
  }
#endif
}

void Tray::SetAltIcon(const std::string& alticon_path) {
}

void Tray::SetIconsAreTemplates(bool areTemplates) {
}

}  // namespace nwapi
