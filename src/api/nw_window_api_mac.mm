#include "content/nw/src/api/nw_window_api.h"

#include "base/strings/sys_string_conversions.h"

#import <Appkit/NSApplication.h>
#import <Appkit/NSBezierPath.h>
#import <Appkit/NSColor.h>
#import <Appkit/NSDocktile.h>
#import <Appkit/NSImageView.h>
#import <Appkit/NSProgressIndicator.h>
#import <Appkit/NSWindow.h>
#import <objc/runtime.h>

@interface NWProgressBar : NSProgressIndicator
@end

@implementation NWProgressBar

// override the drawing, so we can give color to the progress bar
- (void)drawRect:(NSRect)dirtyRect {
  
  [super drawRect:dirtyRect];
  
  if(self.style != NSProgressIndicatorStyleBar)
    return;
  
  NSRect sliceRect, remainderRect;
  double progressFraction = ([self doubleValue] - [self minValue]) /
  ([self maxValue] - [self minValue]);
  
  NSDivideRect(dirtyRect, &sliceRect, &remainderRect,
               NSWidth(dirtyRect) * progressFraction, NSMinXEdge);
  
  const int kProgressBarCornerRadius = 3;
  
  if (progressFraction == 0.0)
    return;
  
  NSBezierPath *path = [NSBezierPath bezierPathWithRoundedRect:sliceRect
                                                       xRadius:kProgressBarCornerRadius
                                                       yRadius:kProgressBarCornerRadius];
  // blue color with alpha blend 0.5
  [[NSColor colorWithCalibratedRed:0 green:0 blue:1 alpha:0.5] set];
  [path fill];
}
@end

namespace extensions {

void NwCurrentWindowInternalSetShadowInternalFunction::SetShadowOnWindow(gfx::NativeWindow window, bool shadow) {
  NSWindow* nswin = (NSWindow*)window.GetNativeNSWindow();
  nswin.hasShadow = shadow;
}

ExtensionFunction::ResponseAction
NwCurrentWindowInternalSetBadgeLabelInternalFunction::Run() {
  EXTENSION_FUNCTION_VALIDATE(args().size() >= 1);
  EXTENSION_FUNCTION_VALIDATE(args()[0].is_string());
  std::string badge = args()[0].GetString();
  [[NSApp dockTile] setBadgeLabel:base::SysUTF8ToNSString(badge)];
  return RespondNow(NoArguments());
}

ExtensionFunction::ResponseAction
NwCurrentWindowInternalRequestAttentionInternalFunction::Run() {
  EXTENSION_FUNCTION_VALIDATE(args().size());
  EXTENSION_FUNCTION_VALIDATE(args()[0].is_int());
  int count = args()[0].GetInt();
  
  //static map from web_content to its attention_request_id
  static std::unordered_map<content::WebContents*, int> attention_request_id;
  content::WebContents* web_content = GetSenderWebContents();
  
  if (count != 0) {
    attention_request_id[web_content] = count < 0 ? [NSApp requestUserAttention:NSInformationalRequest] :
      [NSApp requestUserAttention:NSCriticalRequest];
  } else {
    [NSApp cancelUserAttentionRequest:attention_request_id[web_content]];
    attention_request_id.erase(web_content);
  }
  return RespondNow(NoArguments());
}

ExtensionFunction::ResponseAction
NwCurrentWindowInternalSetProgressBarInternalFunction::Run() {
  EXTENSION_FUNCTION_VALIDATE(args().size() >= 1);
  EXTENSION_FUNCTION_VALIDATE(args()[0].is_double());
  double progress = args()[0].GetDouble();

  NSDockTile *dockTile = [NSApp dockTile];
  static const char kProgressIndicator = '\0';
  NWProgressBar *progressIndicator = objc_getAssociatedObject(dockTile, &kProgressIndicator);
  static const char kImageView = '\0';
  
  if (progressIndicator == NULL && progress >= 0) {
    // contentView might not be NULL, i.e when download progress is running
    if (dockTile.contentView == NULL) {
      // create image view to draw application icon
      NSImageView* iv = [[NSImageView alloc] init];
      [iv setImage:[NSApp applicationIconImage]];
      // set dockTile content view to app icon
      [dockTile setContentView:iv];
      objc_setAssociatedObject(dockTile, &kImageView, iv, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    }
    
    progressIndicator = [[NWProgressBar alloc]
                         initWithFrame:NSMakeRect(0.0f, 0.0f, dockTile.size.width, 15.)];
    
    [progressIndicator setStyle:NSProgressIndicatorStyleBar];
    
    [progressIndicator setBezeled:YES];
    [progressIndicator setMinValue:0];
    [progressIndicator setMaxValue:1];
    [progressIndicator setHidden:NO];
    [progressIndicator setUsesThreadedAnimation:false];
    
    // add progress indicator to image view
    [dockTile.contentView addSubview:progressIndicator];
    objc_setAssociatedObject(dockTile, &kProgressIndicator, progressIndicator, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
  }

  if(progress >= 0) {
    [progressIndicator setIndeterminate:progress > 1];
    if(progress > 1) {
      // progress Indicator is indeterminate
      // [progressIndicator startAnimation:window_];
      [progressIndicator setDoubleValue:1];
    }
    else {
      //[progressIndicator stopAnimation:window_];
      [progressIndicator setDoubleValue:progress];
    }
  }
  else if (progressIndicator) {
    // progress indicator < 0, destroy it
    objc_setAssociatedObject(dockTile, &kProgressIndicator, NULL, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    [progressIndicator removeFromSuperview];
    [progressIndicator release];
    progressIndicator = NULL;

    // if we the one created the ImageView, we are the one who clean it
    NSImageView* iv = objc_getAssociatedObject(dockTile,&kImageView);
    if (iv) {
      objc_setAssociatedObject(dockTile, &kImageView, NULL, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
      dockTile.contentView = NULL;
      [iv release];
      iv = NULL;
    }
  }
  
  [dockTile display];
  return RespondNow(NoArguments());
}

} // namespace extensions
