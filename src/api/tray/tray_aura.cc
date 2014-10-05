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

#include "base/files/file_path.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"
#include "chrome/browser/status_icons/status_icon.h"
#include "chrome/browser/status_icons/status_icon_observer.h"
#include "chrome/browser/status_icons/status_tray.h"
#include "content/nw/src/api/dispatcher_host.h"
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_shell.h"
#include "ui/gfx/image/image.h"

namespace nwapi {

StatusTray* Tray::status_tray_ = NULL;

class TrayObserver : public StatusIconObserver {
 public:
  TrayObserver(Tray* tray)
      : tray_(tray) {
  }

  virtual ~TrayObserver() {
  }

  virtual void OnStatusIconClicked() OVERRIDE {
    base::ListValue args;
    tray_->dispatcher_host()->SendEvent(tray_, "click", args);
  }

 private:
  Tray* tray_;
};

void Tray::Create(const base::DictionaryValue& option) {
  if (!status_tray_)
    status_tray_ = StatusTray::GetSingleton();

  status_icon_ = status_tray_->CreateStatusIcon(StatusTray::NOTIFICATION_TRAY_ICON,
                                                gfx::ImageSkia(), base::string16());
  status_observer_ = new TrayObserver(this);
  status_icon_->AddObserver(status_observer_);
}

void Tray::ShowAfterCreate() {
}

void Tray::Destroy() {
  if (status_icon_)
    Remove();
}

void Tray::SetTitle(const std::string& title) {
}

void Tray::SetIcon(const std::string& path) {
  gfx::Image icon;
  content::Shell* shell = content::Shell::FromRenderViewHost(
      dispatcher_host()->render_view_host());
  nw::Package* package = shell->GetPackage();
  package->GetImage(base::FilePath::FromUTF8Unsafe(path), &icon);

  if (!icon.IsEmpty())
    status_icon_->SetImage(*icon.ToImageSkia());
}

void Tray::SetTooltip(const std::string& tooltip) {
  status_icon_->SetToolTip(base::UTF8ToUTF16(tooltip));
}

void Tray::SetMenu(Menu* menu) {
  status_icon_->UpdatePlatformContextMenu(menu->menu_model_.get());
}

void Tray::Remove() {
  if (status_icon_) {
    status_icon_->RemoveObserver(status_observer_);
    delete status_observer_;

    status_tray_->RemoveStatusIcon(status_icon_);
    status_icon_ = NULL;
  }
}

void Tray::SetAltIcon(const std::string& alticon_path) {
}

}  // namespace nwapi
