#include "content/nw/src/api/nw_window_api.h"

#include "base/strings/sys_string_conversions.h"

#import <Appkit/NSApplication.h>
#import <Appkit/NSBezierPath.h>
#import <Appkit/NSColor.h>
#import <Appkit/NSDocktile.h>
#import <Appkit/NSImageView.h>
#import <Appkit/NSProgressIndicator.h>

@interface NWProgressBar : NSProgressIndicator
@end

@implementation NWProgressBar

// override the drawing, so we can give color to the progress bar
- (void)drawRect:(NSRect)dirtyRect {
  
  [super drawRect:dirtyRect];
  
  if(self.style != NSProgressIndicatorBarStyle)
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
  
bool NwCurrentWindowInternalSetBadgeLabelFunction::RunAsync() {
  EXTENSION_FUNCTION_VALIDATE(args_);
  std::string badge;
  EXTENSION_FUNCTION_VALIDATE(args_->GetString(0, &badge));
  [[NSApp dockTile] setBadgeLabel:base::SysUTF8ToNSString(badge)];
  return true;
}

bool NwCurrentWindowInternalRequestAttentionInternalFunction::RunAsync() {
  EXTENSION_FUNCTION_VALIDATE(args_);
  int count;
  EXTENSION_FUNCTION_VALIDATE(args_->GetInteger(0, &count));
  
  //static map from web_content to its attention_request_id
  static base::hash_map<content::WebContents*, int> attention_request_id;
  content::WebContents* web_content = GetSenderWebContents();
  
  if (count != 0) {
    attention_request_id[web_content] = count < 0 ? [NSApp requestUserAttention:NSInformationalRequest] :
      [NSApp requestUserAttention:NSCriticalRequest];
  } else {
    [NSApp cancelUserAttentionRequest:attention_request_id[web_content]];
    attention_request_id.erase(web_content);
  }
  return true;
}

bool NwCurrentWindowInternalSetProgressBarFunction::RunAsync() {
  EXTENSION_FUNCTION_VALIDATE(args_);
  double progress;
  EXTENSION_FUNCTION_VALIDATE(args_->GetDouble(0, &progress));
  NSDockTile *dockTile = [NSApp dockTile];
  NWProgressBar *progressIndicator = NULL;

  if ((dockTile.contentView == NULL || [dockTile.contentView.subviews count] == 0 )&& progress >= 0) {
    
    // create image view to draw application icon
    NSImageView *iv = [[NSImageView alloc] init];
    [iv setImage:[NSApp applicationIconImage]];
    
    // set dockTile content view to app icon
    [dockTile setContentView:iv];
    
    progressIndicator = [[NWProgressBar alloc]
                         initWithFrame:NSMakeRect(0.0f, 0.0f, dockTile.size.width, 15.)];
    
    [progressIndicator setStyle:NSProgressIndicatorBarStyle];
    
    [progressIndicator setBezeled:YES];
    [progressIndicator setMinValue:0];
    [progressIndicator setMaxValue:1];
    [progressIndicator setHidden:NO];
    [progressIndicator setUsesThreadedAnimation:false];
    
    // add progress indicator to image view
    [iv addSubview:progressIndicator];
  }
  
  progressIndicator = (NWProgressBar*)[dockTile.contentView.subviews objectAtIndex:0];
  
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
  else {
    // progress indicator < 0, destroy it
    [[dockTile.contentView.subviews objectAtIndex:0]release];
    [dockTile.contentView release];
    dockTile.contentView = NULL;
  }
  
  [dockTile display];
  SendResponse(true);
  return true;
}

} // namespace extensions
