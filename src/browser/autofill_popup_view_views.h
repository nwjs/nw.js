// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_AUTOFILL_AUTOFILL_POPUP_VIEW_VIEWS_H_
#define CHROME_BROWSER_UI_VIEWS_AUTOFILL_AUTOFILL_POPUP_VIEW_VIEWS_H_

#include "chrome/browser/ui/autofill/autofill_popup_view.h"
#include "chrome/browser/ui/views/autofill/autofill_popup_base_view.h"

class AutofillPopupController;

namespace autofill {

// Views toolkit implementation for AutofillPopupView.
class AutofillPopupViewViews : public AutofillPopupBaseView,
                               public AutofillPopupView {
 public:
  // The observing widget should be the top level widget for the native
  // view, which we need to listen to for several signals that indicate the
  // popup should be closed.
  AutofillPopupViewViews(AutofillPopupController* controller,
                         views::Widget* observing_widget);

 private:
  ~AutofillPopupViewViews() override;

  // AutofillPopupView implementation.
  void Show() override;
  void Hide() override;
  void InvalidateRow(size_t row) override;
  void UpdateBoundsAndRedrawPopup() override;

  // views::Views implementation
  void OnPaint(gfx::Canvas* canvas) override;

  // Draw the given autofill entry in |entry_rect|.
  void DrawAutofillEntry(gfx::Canvas* canvas,
                         int index,
                         const gfx::Rect& entry_rect);

  AutofillPopupController* controller_;  // Weak reference.

  DISALLOW_COPY_AND_ASSIGN(AutofillPopupViewViews);
};

}  // namespace autofill

#endif  // CHROME_BROWSER_UI_VIEWS_AUTOFILL_AUTOFILL_POPUP_VIEW_VIEWS_H_
