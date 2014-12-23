// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "chrome/browser/ui/cocoa/autofill/autofill_popup_base_view_cocoa.h"

#include "chrome/browser/ui/autofill/autofill_popup_view_delegate.h"
#include "chrome/browser/ui/autofill/popup_constants.h"
#include "ui/base/cocoa/window_size_constants.h"

@implementation AutofillPopupBaseViewCocoa

#pragma mark -
#pragma mark Colors

- (NSColor*)backgroundColor {
  return [NSColor whiteColor];
}

- (NSColor*)borderColor {
  return [NSColor colorForControlTint:[NSColor currentControlTint]];
}

- (NSColor*)highlightColor {
  return [NSColor selectedControlColor];
}

- (NSColor*)nameColor {
  return [NSColor blackColor];
}

- (NSColor*)separatorColor {
  return [NSColor colorWithCalibratedWhite:220 / 255.0 alpha:1];
}

- (NSColor*)subtextColor {
  return [NSColor grayColor];
}

- (NSColor*)warningColor {
  return [NSColor grayColor];
}

#pragma mark -
#pragma mark Public methods

- (id)initWithDelegate:(autofill::AutofillPopupViewDelegate*)delegate
                 frame:(NSRect)frame {
  self = [super initWithFrame:frame];
  if (self)
    delegate_ = delegate;

  return self;
}

- (void)delegateDestroyed {
  delegate_ = NULL;
}

- (void)drawSeparatorWithBounds:(NSRect)bounds {
  [[self separatorColor] set];
  [NSBezierPath fillRect:bounds];
}

// A slight optimization for drawing:
// https://developer.apple.com/library/mac/#documentation/Cocoa/Conceptual/CocoaViewsGuide/Optimizing/Optimizing.html
- (BOOL)isOpaque {
  return YES;
}

- (BOOL)isFlipped {
  // Flipped so that it's easier to share controller logic with other OSes.
  return YES;
}

- (void)drawBackgroundAndBorder {
  // The inset is needed since the border is centered on the |path|.
  // TODO(isherman): We should consider using asset-based drawing for the
  // border, creating simple bitmaps for the view's border and background, and
  // drawing them using NSDrawNinePartImage().
  CGFloat inset = autofill::kPopupBorderThickness / 2.0;
  NSRect borderRect = NSInsetRect([self bounds], inset, inset);
  NSBezierPath* path = [NSBezierPath bezierPathWithRect:borderRect];
  [[self backgroundColor] setFill];
  [path fill];
  [path setLineWidth:autofill::kPopupBorderThickness];
  [[self borderColor] setStroke];
  [path stroke];
}

- (void)mouseUp:(NSEvent*)theEvent {
  // If the view is in the process of being destroyed, abort.
  if (!delegate_)
    return;

  // Only accept single-click.
  if ([theEvent clickCount] > 1)
    return;

  NSPoint location = [self convertPoint:[theEvent locationInWindow]
                               fromView:nil];

  if (NSPointInRect(location, [self bounds])) {
    delegate_->SetSelectionAtPoint(gfx::Point(NSPointToCGPoint(location)));
    delegate_->AcceptSelectedLine();
  }
}

- (void)mouseMoved:(NSEvent*)theEvent {
  // If the view is in the process of being destroyed, abort.
  if (!delegate_)
    return;

  NSPoint location = [self convertPoint:[theEvent locationInWindow]
                               fromView:nil];

  delegate_->SetSelectionAtPoint(gfx::Point(NSPointToCGPoint(location)));
}

- (void)mouseDragged:(NSEvent*)theEvent {
  [self mouseMoved:theEvent];
}

- (void)mouseExited:(NSEvent*)theEvent {
  // If the view is in the process of being destroyed, abort.
  if (!delegate_)
    return;

  delegate_->SelectionCleared();
}

#pragma mark -
#pragma mark Messages from AutofillPopupViewBridge:

- (void)updateBoundsAndRedrawPopup {
  NSRect frame = NSRectFromCGRect(delegate_->popup_bounds().ToCGRect());

  // Flip coordinates back into Cocoa-land.  The controller's platform-neutral
  // coordinate space places the origin at the top-left of the first screen,
  // whereas Cocoa's coordinate space expects the origin to be at the
  // bottom-left of this same screen.
  NSScreen* screen = [[NSScreen screens] objectAtIndex:0];
  frame.origin.y = NSMaxY([screen frame]) - NSMaxY(frame);

  // TODO(isherman): The view should support scrolling if the popup gets too
  // big to fit on the screen.
  [[self window] setFrame:frame display:YES];
  [self setNeedsDisplay:YES];
}

- (void)showPopup {
  NSWindow* window =
      [[NSWindow alloc] initWithContentRect:ui::kWindowSizeDeterminedLater
                                  styleMask:NSBorderlessWindowMask
                                    backing:NSBackingStoreBuffered
                                      defer:YES];
  [window setContentView:self];

  // Telling Cocoa that the window is opaque enables some drawing optimizations.
  [window setOpaque:YES];

  [self updateBoundsAndRedrawPopup];
  [[delegate_->container_view() window] addChildWindow:window
                                               ordered:NSWindowAbove];
}

- (void)hidePopup {
  // Remove the child window before closing, otherwise it can mess up
  // display ordering.
  NSWindow* window = [self window];
  [[window parentWindow] removeChildWindow:window];
  [window close];
}

@end
