// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_COCOA_AUTOFILL_AUTOFILL_POPUP_BASE_VIEW_COCOA_H_
#define CHROME_BROWSER_UI_COCOA_AUTOFILL_AUTOFILL_POPUP_BASE_VIEW_COCOA_H_

#import <Cocoa/Cocoa.h>

#import "ui/base/cocoa/base_view.h"

namespace autofill {
class AutofillPopupViewDelegate;
}

@interface AutofillPopupBaseViewCocoa : BaseView {
 @private
   __weak autofill::AutofillPopupViewDelegate* delegate_;
}

- (NSColor*)backgroundColor;
- (NSColor*)borderColor;
- (NSColor*)highlightColor;
- (NSColor*)nameColor;
- (NSColor*)separatorColor;
- (NSColor*)subtextColor;
- (NSColor*)warningColor;

- (id)initWithDelegate:(autofill::AutofillPopupViewDelegate*)delegate
                 frame:(NSRect)frame;

// Informs the view that its delegate has been (or will imminently be)
// destroyed.
- (void)delegateDestroyed;

// Draw the popup's background and border.
- (void)drawBackgroundAndBorder;

// Draws a thin separator in the popup UI.
- (void)drawSeparatorWithBounds:(NSRect)bounds;

// Messages from AutofillPopupViewBridge:
- (void)updateBoundsAndRedrawPopup;
- (void)showPopup;
- (void)hidePopup;
@end

#endif  // CHROME_BROWSER_UI_COCOA_AUTOFILL_AUTOFILL_POPUP_BASE_VIEW_COCOA_H_
