// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_AUTOFILL_AUTOFILL_POPUP_VIEW_DELEGATE_H_
#define CHROME_BROWSER_UI_AUTOFILL_AUTOFILL_POPUP_VIEW_DELEGATE_H_

#include "ui/gfx/native_widget_types.h"

namespace gfx {
class Point;
class Rect;
class RectF;
}

namespace autofill {

// Base class for Controllers of Autofill-style popups. This interface is
// used by the relevant views to communicate with the controller.
class AutofillPopupViewDelegate {
 public:
  // Called when the popup should be hidden. Controller will be deleted after
  // the view has been hidden and destroyed.
  virtual void Hide() = 0;

  // Called whent the popup view was destroyed.
  virtual void ViewDestroyed() = 0;

  // The user has selected |point|, e.g. by hovering the mouse cursor. |point|
  // must be in popup coordinates.
  virtual void SetSelectionAtPoint(const gfx::Point& point) = 0;

  // The user has accepted the currently selected line. Returns whether there
  // was a selection to accept.
  virtual bool AcceptSelectedLine() = 0;

  // The user cleared the current selection, e.g. by moving the mouse cursor
  // out of the popup bounds.
  virtual void SelectionCleared() = 0;

  // The actual bounds of the popup.
  virtual const gfx::Rect& popup_bounds() const = 0;

  // The view that the form field element sits in.
  virtual gfx::NativeView container_view() = 0;

  // The bounds of the form field element (screen coordinates).
  virtual const gfx::RectF& element_bounds() const = 0;

  // If the current popup should be displayed in RTL mode.
  virtual bool IsRTL() const = 0;

 protected:
  virtual ~AutofillPopupViewDelegate() {}
};

}  // namespace autofill

#endif  // CHROME_BROWSER_UI_AUTOFILL_AUTOFILL_POPUP_VIEW_DELEGATE_H_
