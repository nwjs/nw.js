// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_COCOA_AUTOFILL_AUTOFILL_POPUP_VIEW_BRIDGE_H_
#define CHROME_BROWSER_UI_COCOA_AUTOFILL_AUTOFILL_POPUP_VIEW_BRIDGE_H_

#include <vector>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "chrome/browser/ui/autofill/autofill_popup_view.h"

@class AutofillPopupViewCocoa;
@class NSWindow;

namespace autofill {

class AutofillPopupController;

// Mac implementation for AutofillPopupView interface.
// Serves as a bridge to the Objective-C class AutofillPopupViewCocoa which
// actually implements the view.
class AutofillPopupViewBridge : public AutofillPopupView {
 public:
  explicit AutofillPopupViewBridge(AutofillPopupController* controller);

 private:
  virtual ~AutofillPopupViewBridge();

  // AutofillPopupView implementation.
  virtual void Hide() OVERRIDE;
  virtual void Show() OVERRIDE;
  virtual void InvalidateRow(size_t row) OVERRIDE;
  virtual void UpdateBoundsAndRedrawPopup() OVERRIDE;

  // Set the initial bounds of the popup to show, including the placement
  // of it.
  void SetInitialBounds();

  // The controller for this view.
  AutofillPopupController* controller_;  // Weak reference.

  // The native Cocoa window and view.
  NSWindow* window_;  // Weak reference, owns itself.
  AutofillPopupViewCocoa* view_;  // Weak reference, owned by the |window_|.

  DISALLOW_COPY_AND_ASSIGN(AutofillPopupViewBridge);
};

}  // namespace autofill

#endif  // CHROME_BROWSER_UI_COCOA_AUTOFILL_AUTOFILL_POPUP_VIEW_BRIDGE_H_
