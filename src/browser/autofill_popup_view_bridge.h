// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_COCOA_AUTOFILL_AUTOFILL_POPUP_VIEW_BRIDGE_H_
#define CHROME_BROWSER_UI_COCOA_AUTOFILL_AUTOFILL_POPUP_VIEW_BRIDGE_H_

#include <vector>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/mac/scoped_nsobject.h"
#include "chrome/browser/ui/autofill/autofill_popup_view.h"
#include "chrome/browser/ui/cocoa/autofill/autofill_popup_view_cocoa.h"

@class AutofillPopupViewCocoa;
@class NSWindow;

namespace autofill {

class AutofillPopupViewDelegate;

// Mac implementation of the AutofillPopupView interface.
// Serves as a bridge to an instance of the Objective-C class which actually
// implements the view.
class AutofillPopupViewBridge : public AutofillPopupView {
 public:
  explicit AutofillPopupViewBridge(AutofillPopupController* controller);

 private:
  ~AutofillPopupViewBridge() override;

  // AutofillPopupView implementation.
  void Hide() override;
  void Show() override;
  void InvalidateRow(size_t row) override;
  void UpdateBoundsAndRedrawPopup() override;

  // Set the initial bounds of the popup, including its placement.
  void SetInitialBounds();

  // The native Cocoa view.
  base::scoped_nsobject<AutofillPopupViewCocoa> view_;

  AutofillPopupController* controller_;  // Weak.

  DISALLOW_COPY_AND_ASSIGN(AutofillPopupViewBridge);
};

}  // namespace autofill

#endif  // CHROME_BROWSER_UI_COCOA_AUTOFILL_AUTOFILL_POPUP_VIEW_BRIDGE_H_
