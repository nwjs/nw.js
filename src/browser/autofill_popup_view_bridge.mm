// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import <Cocoa/Cocoa.h>

#include "content/nw/src/browser/autofill_popup_view_bridge.h"

#include "base/logging.h"
#include "content/nw/src/browser/autofill_popup_controller.h"
#import "content/nw/src/browser/autofill_popup_view_cocoa.h"
#include "ui/base/cocoa/window_size_constants.h"
#include "ui/gfx/rect.h"

namespace autofill {

AutofillPopupViewBridge::AutofillPopupViewBridge(
    AutofillPopupController* controller)
    : controller_(controller) {
  window_ =
      [[NSWindow alloc] initWithContentRect:ui::kWindowSizeDeterminedLater
                                  styleMask:NSBorderlessWindowMask
                                    backing:NSBackingStoreBuffered
                                      defer:YES];
  // Telling Cocoa that the window is opaque enables some drawing optimizations.
  [window_ setOpaque:YES];

  view_ = [[[AutofillPopupViewCocoa alloc]
             initWithController:controller_
                          frame:NSZeroRect] autorelease];
  [window_ setContentView:view_];
}

AutofillPopupViewBridge::~AutofillPopupViewBridge() {
  [view_ controllerDestroyed];

  // Remove the child window before closing, otherwise it can mess up
  // display ordering.
  [[window_ parentWindow] removeChildWindow:window_];

  [window_ close];
}

void AutofillPopupViewBridge::Hide() {
  delete this;
}

void AutofillPopupViewBridge::Show() {
  UpdateBoundsAndRedrawPopup();
  [[controller_->container_view() window] addChildWindow:window_
                                                 ordered:NSWindowAbove];
}

void AutofillPopupViewBridge::InvalidateRow(size_t row) {
  NSRect dirty_rect =
      NSRectFromCGRect(controller_->GetRowBounds(row).ToCGRect());
  [view_ setNeedsDisplayInRect:dirty_rect];
}

void AutofillPopupViewBridge::UpdateBoundsAndRedrawPopup() {
  NSRect frame = NSRectFromCGRect(controller_->popup_bounds().ToCGRect());

  // Flip coordinates back into Cocoa-land.  The controller's platform-neutral
  // coordinate space places the origin at the top-left of the first screen,
  // whereas Cocoa's coordinate space expects the origin to be at the
  // bottom-left of this same screen.
  NSScreen* screen = [[NSScreen screens] objectAtIndex:0];
  frame.origin.y = NSMaxY([screen frame]) - NSMaxY(frame);

  // TODO(isherman): The view should support scrolling if the popup gets too
  // big to fit on the screen.
  [window_ setFrame:frame display:YES];
  [view_ setNeedsDisplay:YES];
}

AutofillPopupView* AutofillPopupView::Create(
    AutofillPopupController* controller) {
  return new AutofillPopupViewBridge(controller);
}

}  // namespace autofill
