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
#include <cstdio>

namespace nwapi {

void Tray::Create(const base::DictionaryValue& option) {
  menu_ = NULL;
  std::string id, icon;
  option.GetString("id", &id);
  option.GetString("icon", &icon);
  printf("Icon: %s\n", icon.c_str());
  if (icon.empty()) {
    status_item_ = app_indicator_new(id.c_str(), "indicator-messages-new", 
        APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
  }
  else {
    char *theme_dir = strdup(icon.substr(0, icon.find_last_of("/")+1).c_str());
    char *icon_name = strdup(icon.substr(icon.find_last_of("/")+1, icon.find_last_of(".")).c_str());
    printf("Dirname: %s\nBasename: %s\n", theme_dir , icon_name);
    status_item_ = app_indicator_new_with_path(id.c_str(), icon_name, 
        APP_INDICATOR_CATEGORY_APPLICATION_STATUS, theme_dir);
    free(icon_name);
    free(theme_dir);
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
  //char *pathbuf = strdup(path.c_str());
  //app_indicator_set_icon_theme_path(status_item_, dirname(pathbuf));
  //std::string file_name(basename(pathbuf));
  //std::string icon_name = file_name.substr(0, file_name.find_last_of("."));
  ////app_indicator_set_icon_full(status_item_, basename(pathbuf), basename(pathbuf));
  //app_indicator_set_status (status_item_, APP_INDICATOR_STATUS_ACTIVE);
  //app_indicator_set_attention_icon (status_item_, icon_name.c_str());
  //free(pathbuf);
  char *theme_dir = strdup(path.substr(0, path.find_last_of("/")+1).c_str());
  char *icon_name = strdup(path.substr(path.find_last_of("/")+1, path.find_last_of(".")).c_str());
  printf("Set Icon: Dirname: %s\nBasename: %s\n", theme_dir , icon_name);
  app_indicator_set_icon_theme_path(status_item_, theme_dir);
  app_indicator_set_status (status_item_, APP_INDICATOR_STATUS_ACTIVE);
  app_indicator_set_icon_full(status_item_, icon_name, icon_name);
  free(icon_name);
  free(theme_dir);
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

}  // namespace nwapi
