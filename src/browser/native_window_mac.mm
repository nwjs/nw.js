// Copyright (c) 2012 Intel Corp
// Copyright (c) 2012 The Chromium Authors
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

#include "content/nw/src/browser/native_window_mac.h"

#include "base/mac/mac_util.h"
#include "base/sys_string_conversions.h"
#include "base/values.h"
#import "chrome/browser/ui/cocoa/custom_frame_view.h"
#include "chrome/common/extensions/draggable_region.h"
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/api/app/app.h"
#include "content/nw/src/browser/native_window_helper_mac.h"
#include "content/nw/src/browser/shell_toolbar_delegate_mac.h"
#include "content/nw/src/browser/standard_menus_mac.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_shell.h"
#include "content/public/browser/native_web_keyboard_event.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_view.h"
#include "third_party/skia/include/core/SkRegion.h"
#import "ui/base/cocoa/underlay_opengl_hosting_window.h"

@interface NSWindow (NSPrivateApis)
- (void)setBottomCornerRounded:(BOOL)rounded;
@end

@interface NSView (WebContentsView)
- (void)setMouseDownCanMoveWindow:(BOOL)can_move;
@end

// Replicate specific 10.7 SDK declarations for building with prior SDKs.
#if !defined(MAC_OS_X_VERSION_10_7) || \
    MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_7

enum {
  NSWindowCollectionBehaviorFullScreenPrimary = 1 << 7,
  NSWindowCollectionBehaviorFullScreenAuxiliary = 1 << 8
};

enum {
  NSWindowDocumentVersionsButton = 6,
  NSWindowFullScreenButton
};

@interface NSWindow (LionSDKDeclarations)
- (void)toggleFullScreen:(id)sender;
@end

#endif  // MAC_OS_X_VERSION_10_7

@interface NativeWindowDelegate : NSObject<NSWindowDelegate> {
 @private
  content::Shell* shell_;
}
- (id)initWithShell:(content::Shell*)shell;
@end

@implementation NativeWindowDelegate

- (id)initWithShell:(content::Shell*)shell {
  if ((self = [super init])) {
    shell_ = shell;
  }
  return self;
}

- (BOOL)windowShouldClose:(id)window {
  // If this window is bound to a js object and is not forced to close,
  // then send event to renderer to let the user decide.
  if (!shell_->ShouldCloseWindow())
    return NO;

  // Clean ourselves up and do the work after clearing the stack of anything
  // that might have the shell on it.
  [self performSelectorOnMainThread:@selector(cleanup:)
                         withObject:window
                      waitUntilDone:NO];

  return YES;
}

- (void)windowWillEnterFullScreen:(NSNotification*)notification {
  static_cast<nw::NativeWindowCocoa*>(shell_->window())->
      set_is_fullscreen(true);
  shell_->SendEvent("enter-fullscreen");
}

- (void)windowWillExitFullScreen:(NSNotification*)notification {
  static_cast<nw::NativeWindowCocoa*>(shell_->window())->
      set_is_fullscreen(false);
  shell_->SendEvent("leave-fullscreen");
}

- (void)windowDidBecomeKey:(NSNotification *)notification {
  shell_->web_contents()->Focus();
  shell_->SendEvent("focus");
}

- (void)windowDidResignKey:(NSNotification *)notification {
  shell_->SendEvent("blur");
}

- (void)windowDidMiniaturize:(NSNotification *)notification{
  shell_->SendEvent("minimize");
}

- (void)windowDidDeminiaturize:(NSNotification *)notification {
  shell_->SendEvent("restore");
}

- (BOOL)windowShouldZoom:(NSWindow*)window toFrame:(NSRect)newFrame {
  // Cocoa doen't have concept of maximize/unmaximize, so wee need to emulate
  // them by calculating size change when zooming.
  if (newFrame.size.width < [window frame].size.width ||
      newFrame.size.height < [window frame].size.height)
    shell_->SendEvent("unmaximize");
  else
    shell_->SendEvent("maximize");

  return YES;
}

- (void)cleanup:(id)window {
  delete shell_;

  [self release];
}

@end

@interface ControlRegionView : NSView {
 @private
  nw::NativeWindowCocoa* shellWindow_;  // Weak; owns self.
}
@end

@implementation ControlRegionView

- (id)initWithShellWindow:(nw::NativeWindowCocoa*)shellWindow {
  if ((self = [super init]))
    shellWindow_ = shellWindow;
  return self;
}

- (BOOL)mouseDownCanMoveWindow {
  return NO;
}

- (NSView*)hitTest:(NSPoint)aPoint {
  if (shellWindow_->use_system_drag())
    return nil;
  if (!shellWindow_->draggable_region() ||
      !shellWindow_->draggable_region()->contains(aPoint.x, aPoint.y)) {
    return nil;
  }
  return self;
}

- (void)mouseDown:(NSEvent*)event {
  shellWindow_->HandleMouseEvent(event);
}

- (void)mouseDragged:(NSEvent*)event {
  shellWindow_->HandleMouseEvent(event);
}

@end

// This is really a method on NSGrayFrame, so it should only be called on the
// view passed into -[NSWindow drawCustomFrameRect:forView:].
@interface NSView (PrivateMethods)
- (CGFloat)roundedCornerRadius;
@end

@interface ShellNSWindow : UnderlayOpenGLHostingWindow {
 @private
  content::Shell* shell_;
  bool is_transparent_;
}
- (void)setShell:(content::Shell*)shell;
- (void)showDevTools:(id)sender;
- (void)closeAllWindows:(id)sender;
- (void)setTransparent;
- (BOOL)getTransparent;
@end

@implementation ShellNSWindow

- (void)setShell:(content::Shell*)shell {
  shell_ = shell;
}

- (void)showDevTools:(id)sender {
  shell_->ShowDevTools();
}

- (void)closeAllWindows:(id)sender {
  api::App::CloseAllWindows();
}

- (void)setTransparent {
  is_transparent_ = true;
}

- (BOOL)getTransparent {
  return is_transparent_;
}

@end

@interface ShellFramelessNSWindow : ShellNSWindow

- (void)drawCustomFrameRect:(NSRect)rect forView:(NSView*)view;

@end

@implementation ShellFramelessNSWindow

- (void)drawCustomFrameRect:(NSRect)rect forView:(NSView*)view {
  [super drawCustomFrameRect:rect forView:view];

  [[NSBezierPath bezierPathWithRect:rect] addClip];
  [[NSColor clearColor] set];
  NSRectFill(rect);

  if(![self getTransparent])
  {
    // Set up our clip.
    CGFloat cornerRadius = 4.0;
    if ([view respondsToSelector:@selector(roundedCornerRadius)])
      cornerRadius = [view roundedCornerRadius];
    [[NSBezierPath bezierPathWithRoundedRect:[view bounds]
                                     xRadius:cornerRadius
                                     yRadius:cornerRadius] addClip];
    [[NSColor whiteColor] set];
    NSRectFill(rect);
  }
}

+ (NSRect)frameRectForContentRect:(NSRect)contentRect
                        styleMask:(NSUInteger)mask {
  return contentRect;
}

+ (NSRect)contentRectForFrameRect:(NSRect)frameRect
                        styleMask:(NSUInteger)mask {
  return frameRect;
}

- (NSRect)frameRectForContentRect:(NSRect)contentRect {
  return contentRect;
}

- (NSRect)contentRectForFrameRect:(NSRect)frameRect {
  return frameRect;
}

@end

namespace nw {

NativeWindowCocoa::NativeWindowCocoa(
    content::Shell* shell,
    base::DictionaryValue* manifest)
    : NativeWindow(shell, manifest),
      is_fullscreen_(false),
      is_kiosk_(false),
      is_transparent_(false),
      attention_request_id_(0),
      use_system_drag_(true) {
  int width, height;
  manifest->GetInteger(switches::kmWidth, &width);
  manifest->GetInteger(switches::kmHeight, &height);

  NSRect main_screen_rect = [[[NSScreen screens] objectAtIndex:0] frame];
  NSRect cocoa_bounds = NSMakeRect(
      (NSWidth(main_screen_rect) - width) / 2,
      (NSHeight(main_screen_rect) - height) / 2,
      width,
      height);
  NSUInteger style_mask = NSTitledWindowMask | NSClosableWindowMask |
                          NSMiniaturizableWindowMask | NSResizableWindowMask |
                          NSTexturedBackgroundWindowMask;
  ShellNSWindow* shell_window;
  if (has_frame_) {
    shell_window = [[ShellNSWindow alloc]
        initWithContentRect:cocoa_bounds
                  styleMask:style_mask
                    backing:NSBackingStoreBuffered
                      defer:NO];
  } else {
    shell_window = [[ShellFramelessNSWindow alloc]
        initWithContentRect:cocoa_bounds
                  styleMask:style_mask
                    backing:NSBackingStoreBuffered
                      defer:NO];
  }
  window_ = shell_window;
  [shell_window setShell:shell];
  [window() setDelegate:[[NativeWindowDelegate alloc] initWithShell:shell]];

  // Disable fullscreen button when 'fullscreen' is specified to false.
  bool fullscreen;
  if (!(manifest->GetBoolean(switches::kmFullscreen, &fullscreen) &&
        !fullscreen)) {
    NSUInteger collectionBehavior = [window() collectionBehavior];
    collectionBehavior |= NSWindowCollectionBehaviorFullScreenPrimary;
    [window() setCollectionBehavior:collectionBehavior];
  }

  if (base::mac::IsOSSnowLeopard() &&
      [window() respondsToSelector:@selector(setBottomCornerRounded:)])
    [window() setBottomCornerRounded:NO];

  NSView* view = web_contents()->GetView()->GetNativeView();
  [view setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];

  // By default, the whole frameless window is not draggable.
  if (!has_frame_) {
    gfx::Rect window_bounds(
        0, 0, NSWidth(cocoa_bounds), NSHeight(cocoa_bounds));
    system_drag_exclude_areas_.push_back(window_bounds);
  }

  InstallView();
}

NativeWindowCocoa::~NativeWindowCocoa() {
}

void NativeWindowCocoa::InstallView() {
  NSView* view = web_contents()->GetView()->GetNativeView();
  if (has_frame_) {
    [view setFrame:[[window() contentView] bounds]];
    [[window() contentView] addSubview:view];
  } else {
    // TODO(jeremya): find a cleaner way to send this information to the
    // WebContentsViewCocoa view.
    DCHECK([view
        respondsToSelector:@selector(setMouseDownCanMoveWindow:)]);
    [view setMouseDownCanMoveWindow:YES];

    NSView* frameView = [[window() contentView] superview];
    [view setFrame:[frameView bounds]];
    [frameView addSubview:view];

    [[window() standardWindowButton:NSWindowZoomButton] setHidden:YES];
    [[window() standardWindowButton:NSWindowMiniaturizeButton] setHidden:YES];
    [[window() standardWindowButton:NSWindowCloseButton] setHidden:YES];
    [[window() standardWindowButton:NSWindowFullScreenButton] setHidden:YES];

    InstallDraggableRegionViews();
  }
}

void NativeWindowCocoa::UninstallView() {
  NSView* view = web_contents()->GetView()->GetNativeView();
  [view removeFromSuperview];
}

void NativeWindowCocoa::Close() {
  [window() performClose:nil];
}

void NativeWindowCocoa::Move(const gfx::Rect& pos) {
  NSRect cocoa_bounds = NSMakeRect(pos.x(), 0,
                                   pos.width(),
                                   pos.height());
  // Flip coordinates based on the primary screen.
  NSScreen* screen = [[NSScreen screens] objectAtIndex:0];
  cocoa_bounds.origin.y =
      NSHeight([screen frame]) - pos.height() - pos.y();

  [window() setFrame:cocoa_bounds display:YES];
}

void NativeWindowCocoa::Focus(bool focus) {
  if (focus && [window() isVisible])
    [window() makeKeyAndOrderFront:nil];
  else
    [window() orderBack:nil];
}

void NativeWindowCocoa::Show() {
  [window() makeKeyAndOrderFront:nil];
}

void NativeWindowCocoa::Hide() {
  [window() orderOut:nil];
}

void NativeWindowCocoa::Maximize() {
  [window() zoom:nil];
}

void NativeWindowCocoa::Unmaximize() {
  [window() zoom:nil];
}

void NativeWindowCocoa::Minimize() {
  [window() miniaturize:nil];
}

void NativeWindowCocoa::Restore() {
  [window() deminiaturize:nil];
}

void NativeWindowCocoa::SetFullscreen(bool fullscreen) {
  if (fullscreen == is_fullscreen_)
    return;

  if (base::mac::IsOSLionOrLater()) {
    is_fullscreen_ = fullscreen;
    [window() toggleFullScreen:nil];
    return;
  }

  DCHECK(base::mac::IsOSSnowLeopard());

  SetNonLionFullscreen(fullscreen);
}

bool NativeWindowCocoa::IsFullscreen() {
  return is_fullscreen_;
}

void NativeWindowCocoa::SetTransparent() {
  is_transparent_ = true;
  if(base::mac::IsOSMountainLionOrLater()) {
    restored_bounds_ = [window() frame];
    [window() setStyleMask:(NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask)];
    [window() setFrame:[window()
                      frameRectForContentRect:[window() frame]]
             display:YES];
  }
  [window() setHasShadow:NO];
  ShellNSWindow* swin = (ShellNSWindow*)window();
  [swin setTransparent];
  [window() setOpaque:NO];
  [window() setBackgroundColor:[NSColor clearColor]];
}

bool NativeWindowCocoa::IsTransparent() {
  return is_transparent_;
}

void NativeWindowCocoa::SetNonLionFullscreen(bool fullscreen) {
  if (fullscreen == is_fullscreen_)
    return;

  is_fullscreen_ = fullscreen;

  // Fade to black.
  const CGDisplayReservationInterval kFadeDurationSeconds = 0.6;
  bool did_fade_out = false;
  CGDisplayFadeReservationToken token;
  if (CGAcquireDisplayFadeReservation(kFadeDurationSeconds, &token) ==
      kCGErrorSuccess) {
    did_fade_out = true;
    CGDisplayFade(token, kFadeDurationSeconds / 2, kCGDisplayBlendNormal,
        kCGDisplayBlendSolidColor, 0.0, 0.0, 0.0, /*synchronous=*/true);
  }

  // Since frameless windows insert the WebContentsView into the NSThemeFrame
  // ([[window contentView] superview]), and since that NSThemeFrame is
  // destroyed and recreated when we change the styleMask of the window, we
  // need to remove the view from the window when we change the style, and
  // add it back afterwards.
  UninstallView();
  if (fullscreen) {
    restored_bounds_ = [window() frame];
    [window() setStyleMask:NSBorderlessWindowMask];
    [window() setFrame:[window()
        frameRectForContentRect:[[window() screen] frame]]
               display:YES];
    base::mac::RequestFullScreen(base::mac::kFullScreenModeAutoHideAll);
  } else {
    base::mac::ReleaseFullScreen(base::mac::kFullScreenModeAutoHideAll);
    NSUInteger style_mask = NSTitledWindowMask | NSClosableWindowMask |
                            NSMiniaturizableWindowMask | NSResizableWindowMask |
                            NSTexturedBackgroundWindowMask;
    [window() setStyleMask:style_mask];
    [window() setFrame:restored_bounds_ display:YES];
  }
  InstallView();

  // Fade back in.
  if (did_fade_out) {
    CGDisplayFade(token, kFadeDurationSeconds / 2, kCGDisplayBlendSolidColor,
        kCGDisplayBlendNormal, 0.0, 0.0, 0.0, /*synchronous=*/false);
    CGReleaseDisplayFadeReservation(token);
  }

  is_fullscreen_ = fullscreen;
  if (fullscreen)
    shell()->SendEvent("enter-fullscreen");
  else
    shell()->SendEvent("leave-fullscreen");
}

void NativeWindowCocoa::SetSize(const gfx::Size& size) {
  NSRect frame = [window_ frame];
  frame.origin.y -= size.height() - frame.size.height;
  frame.size.width = size.width();
  frame.size.height = size.height();

  [window() setFrame:frame display:YES];
}

gfx::Size NativeWindowCocoa::GetSize() {
  NSRect frame = [window_ frame];
  return gfx::Size(frame.size.width, frame.size.height);
}

void NativeWindowCocoa::SetMinimumSize(int width, int height) {
  NSSize min_size = NSMakeSize(width, height);
  NSView* content = [window() contentView];
  [window() setContentMinSize:[content convertSize:min_size toView:nil]];
}

void NativeWindowCocoa::SetMaximumSize(int width, int height) {
  NSSize max_size = NSMakeSize(width, height);
  NSView* content = [window() contentView];
  [window() setContentMaxSize:[content convertSize:max_size toView:nil]];
}

void NativeWindowCocoa::SetResizable(bool resizable) {
  if (resizable) {
    [[window() standardWindowButton:NSWindowZoomButton] setEnabled:YES];
    [window() setStyleMask:window().styleMask | NSResizableWindowMask];
  } else {
    [[window() standardWindowButton:NSWindowZoomButton] setEnabled:NO];
    [window() setStyleMask:window().styleMask ^ NSResizableWindowMask];
  }
}

void NativeWindowCocoa::SetAlwaysOnTop(bool top) {
  [window() setLevel:(top ? NSFloatingWindowLevel : NSNormalWindowLevel)];
}

void NativeWindowCocoa::SetPosition(const std::string& position) {
  if (position == "center")
    [window() center];
}

void NativeWindowCocoa::SetPosition(const gfx::Point& position) {
  Move(gfx::Rect(position, GetSize()));
}

gfx::Point NativeWindowCocoa::GetPosition() {
  NSRect frame = [window_ frame];
  NSScreen* screen = [[NSScreen screens] objectAtIndex:0];

  return gfx::Point(frame.origin.x,
      NSHeight([screen frame]) - frame.origin.y - frame.size.height);
}

void NativeWindowCocoa::SetTitle(const std::string& title) {
  [window() setTitle:base::SysUTF8ToNSString(title)];
}

void NativeWindowCocoa::FlashFrame(bool flash) {
  if (flash) {
    attention_request_id_ = [NSApp requestUserAttention:NSInformationalRequest];
  } else {
    [NSApp cancelUserAttentionRequest:attention_request_id_];
    attention_request_id_ = 0;
  }
}

void NativeWindowCocoa::SetKiosk(bool kiosk) {
  if (kiosk) {
    NSApplicationPresentationOptions options =
        NSApplicationPresentationHideDock +
        NSApplicationPresentationHideMenuBar + 
        NSApplicationPresentationDisableAppleMenu +
        NSApplicationPresentationDisableProcessSwitching +
        NSApplicationPresentationDisableForceQuit +
        NSApplicationPresentationDisableSessionTermination +
        NSApplicationPresentationDisableHideApplication;
    [NSApp setPresentationOptions:options];
    is_kiosk_ = true;
    SetNonLionFullscreen(true);
  } else {
    [NSApp setPresentationOptions:[NSApp currentSystemPresentationOptions]];
    is_kiosk_  = false;
    SetNonLionFullscreen(false);
  }
}

bool NativeWindowCocoa::IsKiosk() {
  return is_kiosk_;
}

void NativeWindowCocoa::SetMenu(api::Menu* menu) {
  StandardMenusMac standard_menus(shell_->GetPackage()->GetName());
  [NSApp setMainMenu:menu->menu_];
  standard_menus.BuildAppleMenu();
  standard_menus.BuildEditMenu();
  standard_menus.BuildWindowMenu();
}

void NativeWindowCocoa::HandleMouseEvent(NSEvent* event) {
  if ([event type] == NSLeftMouseDown) {
    last_mouse_location_ =
        [window() convertBaseToScreen:[event locationInWindow]];
  } else if ([event type] == NSLeftMouseDragged) {
    NSPoint current_mouse_location =
        [window() convertBaseToScreen:[event locationInWindow]];
    NSPoint frame_origin = [window() frame].origin;
    frame_origin.x += current_mouse_location.x - last_mouse_location_.x;
    frame_origin.y += current_mouse_location.y - last_mouse_location_.y;
    [window() setFrameOrigin:frame_origin];
    last_mouse_location_ = current_mouse_location;
  }
}

void NativeWindowCocoa::AddToolbar() {
  if (!has_frame_)
    return;

  // create the toolbar object
  scoped_nsobject<NSToolbar> toolbar(
      [[NSToolbar alloc] initWithIdentifier:@"node-webkit toolbar"]);

  // set initial toolbar properties
  [toolbar setAllowsUserCustomization:NO];
  [toolbar setAutosavesConfiguration:NO];
  [toolbar setDisplayMode:NSToolbarDisplayModeIconOnly];
  [toolbar setSizeMode:NSToolbarSizeModeSmall];

  // set our controller as the toolbar delegate
  toolbar_delegate_.reset([[ShellToolbarDelegate alloc] initWithShell:shell()]);
  [toolbar setDelegate:toolbar_delegate_];

  // attach the toolbar to our window
  [window() setToolbar:toolbar];
}

void NativeWindowCocoa::SetToolbarButtonEnabled(TOOLBAR_BUTTON button_id,
                                                bool enabled) {
  if (toolbar_delegate_)
    [toolbar_delegate_ setEnabled:enabled forButton:button_id];
}

void NativeWindowCocoa::SetToolbarUrlEntry(const std::string& url) {
  if (toolbar_delegate_)
    [toolbar_delegate_ setUrl:base::SysUTF8ToNSString(url)];
}
  
void NativeWindowCocoa::SetToolbarIsLoading(bool loading) {
  if (toolbar_delegate_)
    [toolbar_delegate_ setIsLoading:loading];
}

void NativeWindowCocoa::UpdateDraggableRegions(
    const std::vector<extensions::DraggableRegion>& regions) {
  // Draggable region is not supported for non-frameless window.
  if (has_frame_)
    return;

  // To use system drag, the window has to be marked as draggable with
  // non-draggable areas being excluded via overlapping views.
  // 1) If no draggable area is provided, the window is not draggable at all.
  // 2) If only one draggable area is given, as this is the most common
  //    case, use the system drag. The non-draggable areas that are opposite of
  //    the draggable area are computed.
  // 3) Otherwise, use the custom drag. As such, we lose the capability to
  //    support some features like snapping into other space.

  // Determine how to perform the drag by counting the number of draggable
  // areas.
  const extensions::DraggableRegion* draggable_area = NULL;
  use_system_drag_ = true;
  for (std::vector<extensions::DraggableRegion>::const_iterator iter =
           regions.begin();
       iter != regions.end();
       ++iter) {
    if (iter->draggable) {
      // If more than one draggable area is found, use custom drag.
      if (draggable_area) {
        use_system_drag_ = false;
        break;
      }
      draggable_area = &(*iter);
    }
  }

  if (use_system_drag_)
    UpdateDraggableRegionsForSystemDrag(regions, draggable_area);
  else
    UpdateDraggableRegionsForCustomDrag(regions);

  InstallDraggableRegionViews();
}

void NativeWindowCocoa::HandleKeyboardEvent(
    const content::NativeWebKeyboardEvent& event) {
  if (event.skip_in_browser)
    return;

  // The event handling to get this strictly right is a tangle; cheat here a bit
  // by just letting the menus have a chance at it.
  if ([event.os_event type] == NSKeyDown)
    [[NSApp mainMenu] performKeyEquivalent:event.os_event];
}

void NativeWindowCocoa::UpdateDraggableRegionsForSystemDrag(
    const std::vector<extensions::DraggableRegion>& regions,
    const extensions::DraggableRegion* draggable_area) {
  NSView* web_view = web_contents()->GetView()->GetNativeView();
  NSInteger web_view_width = NSWidth([web_view bounds]);
  NSInteger web_view_height = NSHeight([web_view bounds]);

  system_drag_exclude_areas_.clear();

  // The whole window is not draggable if no draggable area is given.
  if (!draggable_area) {
    gfx::Rect window_bounds(0, 0, web_view_width, web_view_height);
    system_drag_exclude_areas_.push_back(window_bounds);
    return;
  }

  // Otherwise, there is only one draggable area. Compute non-draggable areas
  // that are the opposite of the given draggable area, combined with the
  // remaining provided non-draggable areas.

  // Copy all given non-draggable areas.
  for (std::vector<extensions::DraggableRegion>::const_iterator iter =
           regions.begin();
       iter != regions.end();
       ++iter) {
    if (!iter->draggable)
      system_drag_exclude_areas_.push_back(iter->bounds);
  }

  gfx::Rect draggable_bounds = draggable_area->bounds;
  gfx::Rect non_draggable_bounds;

  // Add the non-draggable area above the given draggable area.
  if (draggable_bounds.y() > 0) {
    non_draggable_bounds.SetRect(0,
                                 0,
                                 web_view_width,
                                 draggable_bounds.y() - 1);
    system_drag_exclude_areas_.push_back(non_draggable_bounds);
  }

  // Add the non-draggable area below the given draggable area.
  if (draggable_bounds.bottom() < web_view_height) {
    non_draggable_bounds.SetRect(0,
                                 draggable_bounds.bottom() + 1,
                                 web_view_width,
                                 web_view_height - draggable_bounds.bottom());
    system_drag_exclude_areas_.push_back(non_draggable_bounds);
  }

  // Add the non-draggable area to the left of the given draggable area.
  if (draggable_bounds.x() > 0) {
    non_draggable_bounds.SetRect(0,
                                 draggable_bounds.y(),
                                 draggable_bounds.x() - 1,
                                 draggable_bounds.height());
    system_drag_exclude_areas_.push_back(non_draggable_bounds);
  }

  // Add the non-draggable area to the right of the given draggable area.
  if (draggable_bounds.right() < web_view_width) {
    non_draggable_bounds.SetRect(draggable_bounds.right() + 1,
                                 draggable_bounds.y(),
                                 web_view_width - draggable_bounds.right(),
                                 draggable_bounds.height());
    system_drag_exclude_areas_.push_back(non_draggable_bounds);
  }
}

void NativeWindowCocoa::UpdateDraggableRegionsForCustomDrag(
    const std::vector<extensions::DraggableRegion>& regions) {
  // We still need one ControlRegionView to cover the whole window such that
  // mouse events could be captured.
  NSView* web_view = web_contents()->GetView()->GetNativeView();
  gfx::Rect window_bounds(
      0, 0, NSWidth([web_view bounds]), NSHeight([web_view bounds]));
  system_drag_exclude_areas_.clear();
  system_drag_exclude_areas_.push_back(window_bounds);

  // Aggregate the draggable areas and non-draggable areas such that hit test
  // could be performed easily.
  SkRegion* draggable_region = new SkRegion;
  for (std::vector<extensions::DraggableRegion>::const_iterator iter =
           regions.begin();
       iter != regions.end();
       ++iter) {
    const extensions::DraggableRegion& region = *iter;
    draggable_region->op(
        region.bounds.x(),
        region.bounds.y(),
        region.bounds.right(),
        region.bounds.bottom(),
        region.draggable ? SkRegion::kUnion_Op : SkRegion::kDifference_Op);
  }
  draggable_region_.reset(draggable_region);
}

void NativeWindowCocoa::InstallDraggableRegionViews() {
  DCHECK(!has_frame_);

  // All ControlRegionViews should be added as children of the WebContentsView,
  // because WebContentsView will be removed and re-added when entering and
  // leaving fullscreen mode.
  NSView* webView = web_contents()->GetView()->GetNativeView();
  NSInteger webViewHeight = NSHeight([webView bounds]);

  // Remove all ControlRegionViews that are added last time.
  // Note that [webView subviews] returns the view's mutable internal array and
  // it should be copied to avoid mutating the original array while enumerating
  // it.
  scoped_nsobject<NSArray> subviews([[webView subviews] copy]);
  for (NSView* subview in subviews.get())
    if ([subview isKindOfClass:[ControlRegionView class]])
      [subview removeFromSuperview];

  // Create and add ControlRegionView for each region that needs to be excluded
  // from the dragging.
  for (std::vector<gfx::Rect>::const_iterator iter =
           system_drag_exclude_areas_.begin();
       iter != system_drag_exclude_areas_.end();
       ++iter) {
    scoped_nsobject<NSView> controlRegion(
        [[ControlRegionView alloc] initWithShellWindow:this]);
    [controlRegion setFrame:NSMakeRect(iter->x(),
                                       webViewHeight - iter->bottom(),
                                       iter->width(),
                                       iter->height())];
    [webView addSubview:controlRegion];
  }
}

gfx::Point NativeWindowCocoa::GetMousePosition() {
  CGEventRef event = CGEventCreate(NULL);
  CGPoint cursor = CGEventGetLocation(event);
  CFRelease(event);
  return gfx::Point(cursor.x,cursor.y);
}

void NativeWindowCocoa::BeginOffclientMouseMove() {
  // Not implemented
}

void NativeWindowCocoa::EndOffclientMouseMove() {
  // Not implemented
}

NativeWindow* CreateNativeWindowCocoa(content::Shell* shell,
                                           base::DictionaryValue* manifest) {
  return new NativeWindowCocoa(shell, manifest);
}

}  // namespace nw
