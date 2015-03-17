// Copyright (c) 2014 Jefry Tedjokusumo <jtg_gg@yahoo.com.sg>
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

#include "chrome/browser/status_icons/status_icon.h"
#include "chrome/browser/status_icons/status_icon_observer.h"
#include "chrome/browser/ui/views/status_icons/status_tray_win.h"

#include "content/public/browser/web_contents.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/common/platform_notification_data.h"
#include "content/nw/src/browser/native_window.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_shell.h"

#include "ui/gfx/image/image.h"
#include "base/strings/utf_string_conversions.h"

#include "content/nw/src/nw_notification_manager_win.h"

#include <shellapi.h>

namespace nw {
class TrayObserver : public StatusIconObserver {
public:
  TrayObserver(NotificationManagerWin* tray)
    : tray_(tray) {
  }

  virtual ~TrayObserver() {
  }

  virtual void OnStatusIconClicked() override {
  }

  virtual void OnBalloonEvent(int event) override {
    switch (event) {
    case NIN_BALLOONHIDE:
      tray_->DesktopNotificationPostClose(true);
      tray_->ReleaseNotification();
      break;
    case NIN_BALLOONTIMEOUT:
      tray_->DesktopNotificationPostClose(false);
      tray_->ReleaseNotification();
      break;
    case NIN_BALLOONSHOW:
      tray_->DesktopNotificationPostDisplay();
      break;
    }
  }

  virtual void OnBalloonClicked() override {
    tray_->DesktopNotificationPostClick();
    tray_->ReleaseNotification();
  }
private:
  NotificationManagerWin* tray_;
};

NotificationManagerWin::NotificationManagerWin() : status_icon_(NULL) {
  status_tray_ = static_cast<StatusTrayWin*>(StatusTray::GetSingleton());

  // check if status icon is already created
  StatusIcon* status_icon = status_tray_->GetStatusIcon();

  // if status icon already created, set the notification count to 1 and add the observer
  notification_count_ = status_icon ? 1 : 0;
  if (status_icon) {
    status_observer_ = new TrayObserver(this);
    status_icon->AddObserver(status_observer_);
  }
}

bool NotificationManagerWin::ReleaseNotification() {
  if (notification_count_ > 0) {
    if (notification_count_ == 1) {
      // if I create the status_icon_ I am responsible to delete it
      if (status_icon_) {
        status_icon_->RemoveObserver(status_observer_);
        status_tray_->RemoveStatusIcon(status_icon_);
        status_icon_ = NULL;
      }

      delete status_observer_;
      status_observer_ = NULL;
    }
    notification_count_--;
    return true;
  }
  return false;
}


NotificationManagerWin::~NotificationManagerWin() {
  ReleaseNotification();

  // this is to clean up status_observer_ if it is created by the constructor
  if (status_observer_) {
    StatusIcon* status_icon = status_tray_->GetStatusIcon();
    if (status_icon)
      status_icon->RemoveObserver(status_observer_);

    delete status_observer_;
    status_observer_ = NULL;
  }
}

bool NotificationManagerWin::AddDesktopNotification(const content::PlatformNotificationData& params,
  const int render_process_id, const int notification_id, const SkBitmap& bitmap_icon) {

  content::Shell* shell = content::Shell::windows()[0];

  // if we reach here, it means the function is called from image download callback
  render_process_id_ = render_process_id;
  notification_id_ = notification_id;

  // set the default notification icon as the app icon
  gfx::Image icon = shell->window()->app_icon();

  // always check if status icon is exist or not
  StatusIcon* status_icon = status_tray_->GetStatusIcon();

  // status_icon_ is null, it means we need to create and adds it to the tray
  if (status_icon == NULL) {
    nw::Package* package = shell->GetPackage();
    status_icon_ = status_tray_->CreateStatusIcon(StatusTray::NOTIFICATION_TRAY_ICON,
      *(shell->window()->app_icon().ToImageSkia()), base::UTF8ToUTF16(package->GetName()));
    status_icon = status_icon_;
    status_observer_ = new TrayObserver(this);
    status_icon->AddObserver(status_observer_);
  }

  // add the counter
  notification_count_++;
  // try to get the notification icon image given by image download callback
  if (bitmap_icon.getSize())
    icon = gfx::Image::CreateFrom1xBitmap(bitmap_icon);

  //if body is empty string, the baloon won't shown
  base::string16 body = params.body;
  if (body.empty()) body = L" ";
  
  //show the baloon, this only works if iconsize >= 32x32
  bool result = status_icon->DisplayBalloon(icon.Width() < 32 || icon.Height() < 32 ? gfx::ImageSkia() : *icon.ToImageSkia(), params.title, body);
  if (!result) {
    DesktopNotificationPostError(L"DisplayBalloon fail");
    ReleaseNotification();
  }

  return result;
}

bool NotificationManagerWin::CancelDesktopNotification(int notification_id) {
  //windows  can only have 1 notification, cannot delete existing notification
  return true;
}
} // namespace nw
