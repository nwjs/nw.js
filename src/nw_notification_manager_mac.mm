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

#import <Cocoa/Cocoa.h>
#include "base/mac/mac_util.h"
#include "base/strings/sys_string_conversions.h"

#include "content/public/browser/web_contents.h"
#include "content/public/browser/render_view_host.h"
#include "content/nw/src/browser/native_window.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_shell.h"

#include "content/nw/src/nw_notification_manager_mac.h"

#if !defined(MAC_OS_X_VERSION_10_8) || \
    MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_8
@interface NSUserNotificationCenter : NSObject
@end
@interface NSUserNotification : NSObject
@end
@implementation NSUserNotification
@end
#endif

@interface NWUserNotificationCenterDelegate : NSObject<NSUserNotificationCenterDelegate> {
}
@end
@implementation NWUserNotificationCenterDelegate

static NWUserNotificationCenterDelegate *singleton_ = nil;

+(NWUserNotificationCenterDelegate *)defaultNWUserNotificationCenterDelegate{
  @synchronized(self) {
    if (singleton_ == nil)
      singleton_ = [[self alloc] init];
  }
  return singleton_;
}

-(BOOL)userNotificationCenter:(NSUserNotificationCenter *)center
  shouldPresentNotification : (NSUserNotification *)notification {

  NSNumber *render_process_id = [notification.userInfo objectForKey : @"render_process_id"];
  NSNumber *render_view_id = [notification.userInfo objectForKey : @"render_view_id"];
  NSNumber *notification_id = [notification.userInfo objectForKey : @"notification_id"];

  nw::NotificationManager::getSingleton()->DesktopNotificationPostDisplay(render_process_id.intValue,
      render_view_id.intValue,
      notification_id.intValue);
  return YES;
}

-(void)userNotificationCenter:(NSUserNotificationCenter *)center didActivateNotification : (NSUserNotification *)notification {
  NSNumber *render_process_id = [notification.userInfo objectForKey : @"render_process_id"];
  NSNumber *render_view_id = [notification.userInfo objectForKey : @"render_view_id"];
  NSNumber *notification_id = [notification.userInfo objectForKey : @"notification_id"];

  nw::NotificationManager::getSingleton()->DesktopNotificationPostClick(render_process_id.intValue,
    render_view_id.intValue,
    notification_id.intValue);
}
@end

namespace nw {
NotificationManagerMac::NotificationManagerMac() {
}

bool NotificationManagerMac::AddDesktopNotification(const content::ShowDesktopNotificationHostMsgParams &params, const int render_process_id, const int render_view_id, const bool worker) {
  return AddDesktopNotification(params, render_process_id, render_view_id, worker, NULL);
}

bool NotificationManagerMac::AddDesktopNotification(const content::ShowDesktopNotificationHostMsgParams& params,
  const int render_process_id, const int render_view_id, const bool worker, const std::vector<SkBitmap>* bitmaps) {

  content::RenderViewHost* host = content::RenderViewHost::FromID(render_process_id, render_view_id);
  if (host == nullptr)
    return false;
  content::Shell* shell = content::Shell::FromRenderViewHost(host);

  static const bool downloadImage = base::mac::IsOSMavericksOrLater();

  if (bitmaps == NULL && downloadImage) {
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

  NSUserNotification *notification = [[NSUserNotification alloc] init];
  [notification setTitle : base::SysUTF16ToNSString(params.title)];
  [notification setInformativeText : base::SysUTF16ToNSString(params.body)];
  notification.hasActionButton = YES;

  if (bitmaps && bitmaps->size()) {
    // try to get the notification icon image given by image download callback
    gfx::Image icon = gfx::Image::CreateFrom1xBitmap(bitmaps->at(0));

    // this function only runs on Mavericks or later
    [notification setContentImage : icon.ToNSImage()];
  }

  notification.userInfo = @{ @"render_process_id" :[NSNumber numberWithInt : render_process_id],
    @"render_view_id" :[NSNumber numberWithInt : render_view_id],
    @"notification_id" :[NSNumber numberWithInt : params.notification_id],
  };

  [notification setSoundName : @"NSUserNotificationDefaultSoundName"];

  [[NSUserNotificationCenter defaultUserNotificationCenter] setDelegate:[NWUserNotificationCenterDelegate defaultNWUserNotificationCenterDelegate]];

  [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification:notification];

  [notification release];

  return true;
}

bool NotificationManagerMac::CancelDesktopNotification(int render_process_id, int render_view_id, int notification_id){
  for (NSUserNotification *notification in[[NSUserNotificationCenter defaultUserNotificationCenter] deliveredNotifications]) {
    NSNumber *current_notification_id = [notification.userInfo objectForKey : @"notification_id"];
    if (current_notification_id.intValue == notification_id){
      [[NSUserNotificationCenter defaultUserNotificationCenter] removeDeliveredNotification:notification];
      return true;
    }
  }
  return false;
}
} // namespace nw
