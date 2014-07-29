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

#ifndef CONTENT_NW_NOTIFICATION_MANAGER_H_
#define CONTENT_NW_NOTIFICATION_MANAGER_H_

#include "base/basictypes.h"
#include "content/public/common/show_desktop_notification_params.h"

namespace nw {

class NotificationManager{
private:
  static NotificationManager *singleton_;

protected:
  explicit NotificationManager();

  // icon image download callback
  static void ImageDownloadCallback(int id, int http_status, const GURL& image_url, const std::vector<SkBitmap>& bitmaps, const std::vector<gfx::Size>& size);
  struct DesktopNotificationParams {
    content::ShowDesktopNotificationHostMsgParams params_;
    int render_process_id_;
    int render_view_id_;
    bool worker_;
  };

  // map used to stored desktop notification params used by ImageDownloadCallback
  std::map<int, DesktopNotificationParams> desktop_notification_params_;

  // internal function for AddDesktopNotification
  virtual bool AddDesktopNotification(const content::ShowDesktopNotificationHostMsgParams& params,
    const int render_process_id, const int render_view_id, const bool worker, const std::vector<SkBitmap>* bitmaps);

public:
  virtual ~NotificationManager();
  static NotificationManager* getSingleton();
  virtual bool AddDesktopNotification(const content::ShowDesktopNotificationHostMsgParams& params,
    const int render_process_id, const int render_view_id, const bool worker) = 0;
  virtual bool CancelDesktopNotification(int render_process_id, int render_view_id, int notification_id) = 0;

  bool DesktopNotificationPostClick(int render_process_id, int render_view_id, int notification_id);
  bool DesktopNotificationPostClose(int render_process_id, int render_view_id, int notification_id, bool by_user);
  bool DesktopNotificationPostDisplay(int render_process_id, int render_view_id, int notification_id);
  bool DesktopNotificationPostError(int render_process_id, int render_view_id, int notification_id, const base::string16& message);

};

} // namespace nw

#endif // CONTENT_NW_NOTIFICATION_MANAGER_H_
