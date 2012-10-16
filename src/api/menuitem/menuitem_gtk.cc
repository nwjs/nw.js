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

#include "content/nw/src/api/menuitem/menuitem.h"

#include "base/values.h"
#include "content/nw/src/api/dispatcher_host.h"
#include "content/nw/src/api/menu/menu.h"

namespace api {

void MenuItem::Create(const base::DictionaryValue& option) {
  std::string type;
  option.GetString("type", &type);

  if (type == "separator") {
    menu_item_ = gtk_separator_menu_item_new();
  } else {
    if (type == "checkbox") {
      menu_item_ = gtk_check_menu_item_new();
      bool checked;
      if (option.GetBoolean("checked", &checked))
        SetChecked(checked);
    } else {
      menu_item_ = gtk_image_menu_item_new();
      std::string icon;
      if (option.GetString("icon", &icon))
        SetIcon(icon);
    }

    bool enabled;
    if (option.GetBoolean("enabled", &enabled))
      SetEnabled(enabled);

    std::string label;
    if (option.GetString("label", &label))
      SetLabel(label);

    std::string tooltip;
    if (option.GetString("tooltip", &tooltip))
      SetTooltip(tooltip);

    int menu_id;
    if (option.GetInteger("submenu", &menu_id))
      SetSubmenu(dispatcher_host()->GetObject<Menu>(menu_id));

    block_active_ = false;
    g_signal_connect(menu_item_, "activate",
                     G_CALLBACK(OnClickThunk), this);
  }

  gtk_widget_show(menu_item_);
  g_object_ref_sink(G_OBJECT(menu_item_));
} 

void MenuItem::Destroy() {
  gtk_widget_destroy(menu_item_);
  g_object_unref(G_OBJECT(menu_item_));
}

void MenuItem::SetLabel(const std::string& label) {
  gtk_menu_item_set_label(GTK_MENU_ITEM(menu_item_), label.c_str());
}

void MenuItem::SetIcon(const std::string& icon) {
  if (icon.empty())
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item_), NULL);
  else
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item_),
                                  gtk_image_new_from_file(icon.c_str()));
}

void MenuItem::SetTooltip(const std::string& tooltip) {
  gtk_widget_set_tooltip_text(menu_item_, tooltip.c_str());
}

void MenuItem::SetEnabled(bool enabled) {
  gtk_widget_set_sensitive(menu_item_, enabled);
}

void MenuItem::SetChecked(bool checked) {
  // Set active will cause 'activate' to be emitted, so block here
  block_active_ = true;
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menu_item_), checked);
  block_active_ = false;
}

void MenuItem::SetSubmenu(Menu* sub_menu) {
  if (sub_menu == NULL)
    gtk_menu_item_remove_submenu(GTK_MENU_ITEM(menu_item_));
  else
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item_), sub_menu->menu_);
}

void MenuItem::OnClick(GtkWidget* widget) {
  if (block_active_)
    return;

  base::ListValue args;
  dispatcher_host()->SendEvent(this, "click", args);
}

}  // namespace api
