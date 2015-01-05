// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_AUTOFILL_AUTOFILL_POPUP_VIEW_H_
#define CHROME_BROWSER_UI_AUTOFILL_AUTOFILL_POPUP_VIEW_H_

#include "ui/gfx/native_widget_types.h"

namespace gfx {
class Rect;
}

namespace ui {
class KeyEvent;
}

namespace autofill {

class AutofillPopupController;

// The interface for creating and controlling a platform-dependent
// AutofillPopupView.
class AutofillPopupView {
 public:
  // The minimum amount of padding between the Autofill name and subtext,
  // in pixels.
  static const size_t kNamePadding = 15;

  // The amount of padding between icons in pixels.
  static const int kIconPadding = 5;

  // The amount of padding at the end of the popup in pixels.
  static const int kEndPadding = 3;

  // Height of the delete icon in pixels.
  static const int kDeleteIconHeight = 16;

  // Width of the delete icon in pixels.
  static const int kDeleteIconWidth = 16;

  // Displays the Autofill popup and fills it in with data from the controller.
  virtual void Show() = 0;

  // Hides the popup from view. This will cause the popup to be deleted.
  virtual void Hide() = 0;

  // Invalidates the given row and redraw it.
  virtual void InvalidateRow(size_t row) = 0;

  // Refreshes the position of the popup.
  virtual void UpdateBoundsAndRedrawPopup() = 0;

  // Factory function for creating the view.
  static AutofillPopupView* Create(AutofillPopupController* controller);

 protected:
  virtual ~AutofillPopupView() {}
};

}  // namespace autofill

#endif  // CHROME_BROWSER_UI_AUTOFILL_AUTOFILL_POPUP_VIEW_H_
