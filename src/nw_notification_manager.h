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
#include "content/public/browser/platform_notification_service.h"

namespace nw {

using namespace content;

class NotificationManager : public content::PlatformNotificationService{
private:
  static NotificationManager *singleton_;

protected:
  explicit NotificationManager();

public:
  ~NotificationManager() override;
  static NotificationManager* getSingleton();

  virtual bool AddDesktopNotification(const content::PlatformNotificationData& params,
    const int render_process_id,
    const int notification_id,
    const SkBitmap& icon) = 0;
  
  virtual bool CancelDesktopNotification(int notification_id) = 0;

  bool DesktopNotificationPostClick(int render_process_id, int notification_id);
  bool DesktopNotificationPostClose(int render_process_id, int notification_id, bool by_user);
  bool DesktopNotificationPostDisplay(int render_process_id, int notification_id);
  bool DesktopNotificationPostError(int render_process_id, int notification_id, const base::string16& message);
  
  // PlatformNotificationService functions
  blink::WebNotificationPermission CheckPermission(ResourceContext* resource_context,
                                                   const GURL& origin,
                                                   int render_process_id) override;
  
  void DisplayNotification(BrowserContext* browser_context,
                           const GURL& origin,
                           const SkBitmap& icon,
                           const PlatformNotificationData& notification_data,
                           scoped_ptr<DesktopNotificationDelegate> delegate,
                           int render_process_id,
                           base::Closure* cancel_callback) override;

  void DisplayPersistentNotification(BrowserContext* browser_context,
                                     int64 service_worker_registration_id,
                                     const GURL& origin,
                                     const SkBitmap& icon,
                                     const PlatformNotificationData& notification_data,
                                     int render_process_id) override;

  void ClosePersistentNotification(BrowserContext* browser_context,
                                   const std::string& persistent_notification_id) override;


};

} // namespace nw

#endif // CONTENT_NW_NOTIFICATION_MANAGER_H_
