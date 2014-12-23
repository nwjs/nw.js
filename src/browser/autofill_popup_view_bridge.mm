// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import <Cocoa/Cocoa.h>

#include "chrome/browser/ui/cocoa/autofill/autofill_popup_view_bridge.h"

#include "base/logging.h"
#include "chrome/browser/ui/autofill/autofill_popup_controller.h"
#include "chrome/browser/ui/autofill/autofill_popup_view_delegate.h"
#import "chrome/browser/ui/cocoa/autofill/autofill_popup_view_cocoa.h"
#include "ui/gfx/rect.h"

namespace autofill {

AutofillPopupViewBridge::AutofillPopupViewBridge(
    AutofillPopupController* controller)
    : controller_(controller) {
  view_.reset(
      [[AutofillPopupViewCocoa alloc] initWithController:controller
                                                   frame:NSZeroRect]);
}

AutofillPopupViewBridge::~AutofillPopupViewBridge() {
  [view_ controllerDestroyed];
  [view_ hidePopup];
}

void AutofillPopupViewBridge::Hide() {
  delete this;
}

void AutofillPopupViewBridge::Show() {
  [view_ showPopup];
}

void AutofillPopupViewBridge::InvalidateRow(size_t row) {
  [view_ invalidateRow:row];
}

void AutofillPopupViewBridge::UpdateBoundsAndRedrawPopup() {
  [view_ updateBoundsAndRedrawPopup];
}

AutofillPopupView* AutofillPopupView::Create(
    AutofillPopupController* controller) {
  return new AutofillPopupViewBridge(controller);
}

}  // namespace autofill
