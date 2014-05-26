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

#include "ui/gfx/image/image.h"
#include "content/public/browser/render_view_host.h"

#include "content/nw/src/nw_notification_manager.h"
#if defined(OS_MACOSX)
#include "content/nw/src/nw_notification_manager_mac.h"
#elif defined(OS_WIN)
#include "content/nw/src/nw_notification_manager_win.h"
#elif defined(OS_LINUX)
#include "content/nw/src/nw_notification_manager_linux.h"
#endif

namespace nw
{
NotificationManager* NotificationManager::singleton_ = NULL;

NotificationManager::NotificationManager() {
}

NotificationManager::~NotificationManager() {
  singleton_ = NULL;
}

NotificationManager* NotificationManager::getSingleton() {
  if (singleton_ == NULL) {
#if defined(OS_MACOSX)
    singleton_ = new NotificationManagerMac();
#elif defined(OS_WIN)
    singleton_ = new NotificationManagerWin();
#elif defined(OS_LINUX)
    singleton_ = new NotificationManagerLinux();
#endif
  }
  return singleton_;
}


void NotificationManager::ImageDownloadCallback(int id, int http_status, const GURL& image_url, const std::vector<SkBitmap>& bitmaps, const std::vector<gfx::Size>& size) {
  NotificationManager *singleton = getSingleton();
  DesktopNotificationParams params = singleton->desktop_notification_params_[id];
  singleton->AddDesktopNotification(params.params_, params.render_process_id_, params.render_view_id_, params.worker_, &bitmaps);
  singleton->desktop_notification_params_.erase(id);
}

bool NotificationManager::AddDesktopNotification(const content::ShowDesktopNotificationHostMsgParams& params,
  const int render_process_id, const int render_view_id, const bool worker, const std::vector<SkBitmap>* bitmaps) {
  NOTIMPLEMENTED();
  return false;
}

bool NotificationManager::DesktopNotificationPostClick(int render_process_id, int render_view_id, int notification_id) {
  content::RenderViewHost* host = content::RenderViewHost::FromID(render_process_id, render_view_id);
  if (host == NULL)
    return false;

  host->DesktopNotificationPostClick(notification_id);
  return true;
}

bool NotificationManager::DesktopNotificationPostClose(int render_process_id, int render_view_id, int notification_id, bool by_user) {
  content::RenderViewHost* host = content::RenderViewHost::FromID(render_process_id, render_view_id);
  if (host == NULL)
    return false;

  host->DesktopNotificationPostClose(notification_id, by_user);
  return true;
}

bool NotificationManager::DesktopNotificationPostDisplay(int render_process_id, int render_view_id, int notification_id) {
  content::RenderViewHost* host = content::RenderViewHost::FromID(render_process_id, render_view_id);
  if (host == NULL)
    return false;

  host->DesktopNotificationPostDisplay(notification_id);
  return true;
}

bool NotificationManager::DesktopNotificationPostError(int render_process_id, int render_view_id, int notification_id, const base::string16& message) {
  content::RenderViewHost* host = content::RenderViewHost::FromID(render_process_id, render_view_id);
  if (host == NULL)
    return false;

  host->DesktopNotificationPostError(notification_id, message);
  return true;
}
} // namespace nw
