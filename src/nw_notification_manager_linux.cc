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

#include "content/public/browser/web_contents.h"
#include "content/public/browser/render_view_host.h"
#include "content/nw/src/browser/native_window.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_shell.h"

#include "ui/gfx/image/image.h"
#include "base/strings/utf_string_conversions.h"

#include "content/nw/src/nw_notification_manager_linux.h"

#include "ui/gfx/gtk_util.h"

//#define GET_NOTIFICATION(id) mNotificationIDmap.find(id)
//Ubuntu notify-osd can only show 1 notification, this is the "hack" to do that
#define GET_NOTIFICATION(id) mNotificationIDmap.begin()

namespace nw {

NotificationManagerLinux::NotificationManagerLinux() {
  notify_init (content::Shell::GetPackage()->GetName().c_str());
}

NotificationManagerLinux::~NotificationManagerLinux() {
  notify_uninit();
}

void NotificationManagerLinux::onClose(NotifyNotification *notif)
{
  NotificationManagerLinux* singleton = static_cast<NotificationManagerLinux*>(NotificationManagerLinux::getSingleton());
  std::map<int, NotifyNotification*>::iterator i;
  for (i = singleton->mNotificationIDmap.begin(); i!=singleton->mNotificationIDmap.end(); i++) {
    if (i->second == notif)
    	break;
  }
  singleton->mNotificationIDmap.erase(i);
  g_object_unref(G_OBJECT(notif));
};

bool NotificationManagerLinux::AddDesktopNotification(const content::ShowDesktopNotificationHostMsgParams& params,
  const int render_process_id, const int render_view_id, const bool worker, const std::vector<SkBitmap>* bitmaps) {

  content::RenderViewHost* host = content::RenderViewHost::FromID(render_process_id, render_view_id);
  if (host == NULL)
    return false;

  content::Shell* shell = content::Shell::FromRenderViewHost(host);

  if (bitmaps == NULL) {
    // called from public function, save the params
    DesktopNotificationParams desktop_notification_params;
    desktop_notification_params.params_ = params;
    desktop_notification_params.render_process_id_ = render_process_id;
    desktop_notification_params.render_view_id_ = render_view_id;

    // download the icon image first
    content::WebContents::ImageDownloadCallback imageDownloadCallback = base::Bind(&NotificationManager::ImageDownloadCallback);
    int id = shell->web_contents()->DownloadImage(params.icon_url, true, 0, imageDownloadCallback);
    desktop_notification_params_[id] = desktop_notification_params;

    // wait for the image download callback
    return true;
  }

  // if we reach here, it means the function is called from image download callback

  SkBitmap bitmap;
  // try to get the notification icon image given by image download callback
  if (bitmaps->size())
    bitmap = bitmaps->at(0);
  else {
	// set the default notification icon as the app icon
	bitmap = shell->window()->app_icon().AsBitmap();
  }

  NotifyNotification * notif;
  std::map<int, NotifyNotification*>::iterator i = GET_NOTIFICATION(params.notification_id);
  if (i==mNotificationIDmap.end()) {
    notif = notify_notification_new (
      base::UTF16ToUTF8(params.title).c_str(), base::UTF16ToUTF8(params.body).c_str(), NULL);
    mNotificationIDmap[params.notification_id] = notif;
  }
  else {
    notif = i->second;
    notify_notification_update(notif, base::UTF16ToUTF8(params.title).c_str(),
      base::UTF16ToUTF8(params.body).c_str(), NULL);
  }

  GdkPixbuf* pixbuf = gfx::GdkPixbufFromSkBitmap(bitmap);
  notify_notification_set_image_from_pixbuf(notif, pixbuf);
  g_object_unref(pixbuf);

  NOTIFY_NOTIFICATION_GET_CLASS(notif)->closed = onClose;

  GError* error = NULL;
  if (notify_notification_show (notif, &error)) {
    DesktopNotificationPostDisplay(render_process_id, render_view_id, params.notification_id);
  }
  else {
    base::string16 errorMsg = base::UTF8ToUTF16(error->message);
    DesktopNotificationPostError(render_process_id, render_view_id, params.notification_id, errorMsg);
  }
  return error==NULL;
}

bool NotificationManagerLinux::CancelDesktopNotification(int render_process_id, int render_view_id, int notification_id) {
  std::map<int, NotifyNotification*>::const_iterator i = GET_NOTIFICATION(notification_id);
  if (i!=mNotificationIDmap.end()) {
    return notify_notification_close(i->second, NULL);
  }
  return false;
}
} // namespace nw
