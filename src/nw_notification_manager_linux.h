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

#ifndef CONTENT_NW_NOTIFICATION_MANAGER_LINUX_H_
#define CONTENT_NW_NOTIFICATION_MANAGER_LINUX_H_

#include "content/nw/src/nw_notification_manager.h"
#include <libnotify/notify.h>

namespace nw {
class NotificationManagerLinux : public NotificationManager {

  std::map<int, NotifyNotification*>  mNotificationIDmap;
  static void onClose(NotifyNotification *notif);


  // internal function for AddDesktopNotification
  virtual bool AddDesktopNotification(const content::ShowDesktopNotificationHostMsgParams& params,
    const int render_process_id, const int render_view_id, const bool worker, const std::vector<SkBitmap>* bitmaps) OVERRIDE;

public:
  explicit NotificationManagerLinux();
  virtual ~NotificationManagerLinux();
  virtual bool AddDesktopNotification(const content::ShowDesktopNotificationHostMsgParams& params,
    const int render_process_id, const int render_view_id, const bool worker) OVERRIDE {
    return AddDesktopNotification(params, render_process_id, render_view_id, worker, NULL);
  }
  virtual bool CancelDesktopNotification(int render_process_id, int render_view_id, int notification_id) OVERRIDE;
};

} // namespace nw

#endif // CONTENT_NW_NOTIFICATION_MANAGER_LINUX_H_
