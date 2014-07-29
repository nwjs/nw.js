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

#ifndef CONTENT_NW_NOTIFICATION_MANAGER_WIN_H_
#define CONTENT_NW_NOTIFICATION_MANAGER_WIN_H_

#include "content/nw/src/nw_notification_manager.h"
class StatusTrayWin;
class StatusIcon;

namespace nw {
class NotificationManagerWin : public NotificationManager{
  // The global presentation of system tray.
  StatusTrayWin* status_tray_;

  // StatusIcon pointer created by ME
  StatusIcon* status_icon_;

  // number of notification in the queue
  int notification_count_;

  // decrement the status_icon_count_, if the value is 0 remove the status_icon_ from the tray
  bool ReleaseNotification();

  // Click observer.
  friend class TrayObserver;
  TrayObserver* status_observer_;

  // variable to store the latest notification data, windows can only show 1 notification
  int render_process_id_, render_view_id_, notification_id_;

  // dispatch the events from the latest notification
  bool DesktopNotificationPostClick() {
    return NotificationManager::DesktopNotificationPostClick(render_process_id_, render_view_id_, notification_id_);
  }
  bool DesktopNotificationPostClose(bool by_user) {
    return NotificationManager::DesktopNotificationPostClose(render_process_id_, render_view_id_, notification_id_, by_user);
  }
  bool DesktopNotificationPostDisplay() {
    return NotificationManager::DesktopNotificationPostDisplay(render_process_id_, render_view_id_, notification_id_);
  }
  bool DesktopNotificationPostError(const base::string16& message) {
    return NotificationManager::DesktopNotificationPostError(render_process_id_, render_view_id_, notification_id_, message);
  }

  // internal function for AddDesktopNotification
  virtual bool AddDesktopNotification(const content::ShowDesktopNotificationHostMsgParams& params,
    const int render_process_id, const int render_view_id, const bool worker, const std::vector<SkBitmap>* bitmaps) OVERRIDE;

public:
  explicit NotificationManagerWin();
  virtual ~NotificationManagerWin();
  virtual bool AddDesktopNotification(const content::ShowDesktopNotificationHostMsgParams& params,
    const int render_process_id, const int render_view_id, const bool worker) OVERRIDE{
    return AddDesktopNotification(params, render_process_id, render_view_id, worker, NULL);
  }
  virtual bool CancelDesktopNotification(int render_process_id, int render_view_id, int notification_id) OVERRIDE;
};

} // namespace nw

#endif // CONTENT_NW_NOTIFICATION_MANAGER_WIN_H_
