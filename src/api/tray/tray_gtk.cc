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
  srand(time(NULL));
  status_item_ = app_indicator_new(std::to_string(rand()).c_str(), "Node-Webkit Icon", APP_INDICATOR_CATEGORY_OTHER);
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
  char *pathbuf = strdup(path.c_str());
  char *cwdbuf = get_current_dir_name();
  char *fullpathbuf = (char *)malloc((strlen(pathbuf)+strlen(cwdbuf)+2)*sizeof(char));
  strncpy(fullpathbuf, cwdbuf, strlen(cwdbuf));
  fullpathbuf[strlen(cwdbuf)] = '/'; fullpathbuf[strlen(cwdbuf)+1] = '\0';
  strncat(fullpathbuf, pathbuf, strlen(pathbuf));
  app_indicator_set_icon_theme_path(status_item_, dirname(fullpathbuf));
  app_indicator_set_icon_full(status_item_, basename(fullpathbuf), basename(fullpathbuf));
  free(fullpathbuf);
  free(cwdbuf);
  free(pathbuf);
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
