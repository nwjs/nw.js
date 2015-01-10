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
#include "chrome/browser/ui/libgtk2ui/skia_utils_gtk2.h"

#include "content/public/browser/web_contents.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/nw/src/browser/native_window.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_shell.h"

#include "ui/gfx/image/image.h"
#include "base/strings/utf_string_conversions.h"

#include "content/nw/src/nw_notification_manager_linux.h"


namespace nw {

NotificationManagerLinux::NotificationManagerLinux() {
  notify_init (content::Shell::GetPackage()->GetName().c_str());
  char* info[4];
  if (notify_get_server_info(&info[0], &info[1], &info[2], &info[3]))
    //Ubuntu notify-osd can only show 1 notification, this is the "hack" to do that
    mForceOneNotification = strcmp(info[0], "notify-osd") == 0;
}

NotificationManagerLinux::~NotificationManagerLinux() {
  notify_uninit();
}

NotificationManagerLinux::NotificationMap::iterator NotificationManagerLinux::getNotification(int id) {
  if (mForceOneNotification) {
    return mNotificationIDmap.begin();
  }
  return mNotificationIDmap.find(id);
}

void NotificationManagerLinux::onClose(NotifyNotification *notif)
{
  NotificationManagerLinux* singleton = static_cast<NotificationManagerLinux*>(NotificationManagerLinux::getSingleton());
  NotificationMap::iterator i;
  for (i = singleton->mNotificationIDmap.begin(); i!=singleton->mNotificationIDmap.end(); i++) {
    if (i->second.mNotification == notif)
    	break;
  }
  //int close_reason = notify_notification_get_closed_reason(notif);
  //printf("close reason %d\n", close_reason);
  singleton->DesktopNotificationPostClose(i->second.mRenderProcessId, i->first, false);
  singleton->mNotificationIDmap.erase(i);
  g_object_unref(G_OBJECT(notif));
};

bool NotificationManagerLinux::AddDesktopNotification(const content::ShowDesktopNotificationHostMsgParams& params,
  const int render_process_id,
  const int notification_id,
  const bool worker) {
  content::Shell* shell = content::Shell::windows()[0];
  SkBitmap bitmap;
  if(params.icon.getSize()) {
    bitmap = params.icon;
  } else {
    bitmap = shell->window()->app_icon().AsBitmap();
  }

  NotifyNotification * notif;
  NotificationMap::iterator i = getNotification(notification_id);
  if (i==mNotificationIDmap.end()) {
    notif = notify_notification_new (
      base::UTF16ToUTF8(params.title).c_str(), base::UTF16ToUTF8(params.body).c_str(), NULL);
    NotificationData data;
    data.mNotification = notif;
    data.mRenderProcessId = render_process_id;
    mNotificationIDmap[notification_id] = data;
  }
  else {
    notif = i->second.mNotification;
    notify_notification_update(notif, base::UTF16ToUTF8(params.title).c_str(),
      base::UTF16ToUTF8(params.body).c_str(), NULL);

    // means this is the notify-osd hack
    if (i->first != notification_id) {
      NotificationData data;
      data.mNotification = notif;
      g_object_ref(G_OBJECT(notif));
      onClose(notif);
      data.mRenderProcessId = render_process_id;
      mNotificationIDmap[notification_id] = data;
    }
  }

  GdkPixbuf* pixbuf = libgtk2ui::GdkPixbufFromSkBitmap(bitmap);
  if (pixbuf) {
    notify_notification_set_image_from_pixbuf(notif, pixbuf);
    g_object_unref(pixbuf);
  }

  NOTIFY_NOTIFICATION_GET_CLASS(notif)->closed = onClose;

  GError* error = NULL;
  if (notify_notification_show (notif, &error)) {
    DesktopNotificationPostDisplay(render_process_id, notification_id);
  }
  else {
    base::string16 errorMsg = base::UTF8ToUTF16(error->message);
    DesktopNotificationPostError(render_process_id, notification_id, errorMsg);
  }
  return error==NULL;
}

bool NotificationManagerLinux::CancelDesktopNotification(int render_process_id, int notification_id) {
  NotificationMap::const_iterator i = getNotification(notification_id);
  if (i!=mNotificationIDmap.end()) {
    return notify_notification_close(i->second.mNotification, NULL);
  }
  return false;
}

} // namespace nw
