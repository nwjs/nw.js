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
#include "content/public/browser/render_frame_host.h"
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
  NSNumber *notification_id = [notification.userInfo objectForKey : @"notification_id"];
  

  nw::NotificationManager::getSingleton()->DesktopNotificationPostDisplay(render_process_id.intValue,
      notification_id.intValue);
  return YES;
}

-(void)userNotificationCenter:(NSUserNotificationCenter *)center didActivateNotification : (NSUserNotification *)notification {
  NSNumber *render_process_id = [notification.userInfo objectForKey : @"render_process_id"];
  NSNumber *notification_id = [notification.userInfo objectForKey : @"notification_id"];
  

  nw::NotificationManager::getSingleton()->DesktopNotificationPostClick(render_process_id.intValue,
    notification_id.intValue);
}
@end

namespace nw {
NotificationManagerMac::NotificationManagerMac() {
}

bool NotificationManagerMac::AddDesktopNotification(const content::ShowDesktopNotificationHostMsgParams& params,
  const int render_process_id, const int notification_id, const bool worker) {

  NSUserNotification *notification = [[NSUserNotification alloc] init];
  [notification setTitle : base::SysUTF16ToNSString(params.title)];
  [notification setInformativeText : base::SysUTF16ToNSString(params.body)];
  notification.hasActionButton = YES;

  if (params.icon.getSize() > 0 && base::mac::IsOSMavericksOrLater()) {
    // try to get the notification icon image given by image download callback
    gfx::Image icon = gfx::Image::CreateFrom1xBitmap(params.icon);

    // this function only runs on Mavericks or later
    [notification setContentImage : icon.ToNSImage()];
  }

  notification.userInfo = @{ @"render_process_id" :[NSNumber numberWithInt : render_process_id],
    @"notification_id" :[NSNumber numberWithInt : notification_id],
  };


  [[NSUserNotificationCenter defaultUserNotificationCenter] setDelegate:[NWUserNotificationCenterDelegate defaultNWUserNotificationCenterDelegate]];

  [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification:notification];

  [notification release];

  return true;
}

bool NotificationManagerMac::CancelDesktopNotification(int render_process_id, int notification_id){
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
