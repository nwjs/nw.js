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
  std::string id, icon;
  option.GetString("id", &id);
  option.GetString("icon", &icon);
  if (icon.empty()) {
    status_item_ = app_indicator_new(id.c_str(), "indicator-messages-new", 
        APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
  }
  else {
    char *theme_dir = TrimString(icon.c_str(), 0, icon.find_last_of("/")+1);
    char *icon_name = TrimString(icon.c_str(), icon.find_last_of("/")+1, icon.find_last_of("."));
    status_item_ = app_indicator_new_with_path(id.c_str(), icon_name, 
        APP_INDICATOR_CATEGORY_APPLICATION_STATUS, theme_dir);
    delete[] icon_name;
    delete[] theme_dir;
  }
}

void Tray::ShowAfterCreate() {
}

void Tray::Destroy() {
  // This object will be destroyed after Remove is called, so we don't release
  // in destructor but in the Remove function.
}

void Tray::SetTitle(const std::string& title) {
  app_indicator_set_title(status_item_, title.c_str());
}

void Tray::SetIcon(const std::string& path) {
  char *theme_dir = TrimString(path.c_str(), 0, path.find_last_of("/")+1);
  char *icon_name = TrimString(path.c_str(), path.find_last_of("/")+1, path.find_last_of("."));
  app_indicator_set_icon_theme_path(status_item_, theme_dir);
  app_indicator_set_status (status_item_, APP_INDICATOR_STATUS_ACTIVE);
  app_indicator_set_icon_full(status_item_, icon_name, icon_name);
  delete[] icon_name;
  delete[] theme_dir;
}

void Tray::SetTooltip(const std::string& tooltip) {
}

void Tray::SetMenu(Menu* menu) {
  menu_ = menu;
  app_indicator_set_menu(status_item_, GTK_MENU(menu_->menu_));
}

void Tray::Remove() {
}

void Tray::SetAltIcon(const std::string& alticon_path) {
}

char *Tray::TrimString(const char *file_name, size_t begin, size_t past_end) {
  int new_length = (int)(past_end - begin + 1);
  char *new_string = new char[new_length];
  for (int i=0; i<(new_length-1); i++) {
    new_string[i] = file_name[i+(int)begin];
  }
  new_string[new_length-1] = '\0';
  return new_string;
}


}  // namespace nwapi
