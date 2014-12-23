// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_COCOA_AUTOFILL_AUTOFILL_POPUP_VIEW_COCOA_H_
#define CHROME_BROWSER_UI_COCOA_AUTOFILL_AUTOFILL_POPUP_VIEW_COCOA_H_

#import <Cocoa/Cocoa.h>

#import "chrome/browser/ui/cocoa/autofill/autofill_popup_base_view_cocoa.h"

namespace autofill {
class AutofillPopupController;
}  // namespace autofill

// Draws the native Autofill popup view on Mac.
@interface AutofillPopupViewCocoa : AutofillPopupBaseViewCocoa {
 @private
  // The cross-platform controller for this view.
  __weak autofill::AutofillPopupController* controller_;
}

// Designated initializer.
- (id)initWithController:(autofill::AutofillPopupController*)controller
                   frame:(NSRect)frame;

// Informs the view that its controller has been (or will imminently be)
// destroyed.
- (void)controllerDestroyed;

- (void)invalidateRow:(size_t)row;

@end

#endif  // CHROME_BROWSER_UI_COCOA_AUTOFILL_AUTOFILL_POPUP_VIEW_COCOA_H_
