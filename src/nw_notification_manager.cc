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
#include "content/public/browser/render_process_host.h"
#include "content/common/platform_notification_messages.h"


#include "content/nw/src/nw_notification_manager.h"
#if defined(OS_MACOSX)
#include "content/nw/src/nw_notification_manager_mac.h"
#elif defined(OS_WIN)
#include "content/nw/src/nw_notification_manager_win.h"
#include "content/nw/src/nw_notification_manager_toast_win.h"
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
    if (NotificationManagerToastWin::IsSupported())
      singleton_ = new NotificationManagerToastWin();
    else
      singleton_ = new NotificationManagerWin();
#elif defined(OS_LINUX)
    singleton_ = new NotificationManagerLinux();
#endif
  }
  return singleton_;
}

bool NotificationManager::DesktopNotificationPostClick(int render_process_id, int notification_id) {
  content::RenderProcessHost* rfh = content::RenderProcessHost::FromID(render_process_id);
  if (!rfh)
    return false;

  rfh->Send(new PlatformNotificationMsg_DidClick(notification_id));
  return true;
}

bool NotificationManager::DesktopNotificationPostClose(int render_process_id, int notification_id, bool by_user) {
  content::RenderProcessHost* rfh = content::RenderProcessHost::FromID(render_process_id);
  if (!rfh)
    return false;

  rfh->Send(new PlatformNotificationMsg_DidClose(notification_id));
  return true;
}

bool NotificationManager::DesktopNotificationPostDisplay(int render_process_id, int notification_id) {
  content::RenderProcessHost* rfh = content::RenderProcessHost::FromID(render_process_id);
  if (!rfh)
    return false;

  rfh->Send(new PlatformNotificationMsg_DidShow(notification_id));
  return true;
}

bool NotificationManager::DesktopNotificationPostError(int render_process_id, int notification_id, const base::string16& message) {
#if 0 //FIXME
  content::RenderProcessHost* rfh = content::RenderProcessHost::FromID(render_process_id);
  if (!rfh)
    return false;

  // Google remove the error notification messaging !!
  rfh->Send(new PlatformNotificationMsg_DidError(rfh->GetRoutingID(), notification_id));
#endif
  return true;
}
} // namespace nw
