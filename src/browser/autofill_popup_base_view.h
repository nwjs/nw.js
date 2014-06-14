// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_AUTOFILL_AUTOFILL_POPUP_BASE_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_AUTOFILL_AUTOFILL_POPUP_BASE_VIEW_H_

#include "base/memory/weak_ptr.h"
#include "content/nw/src/browser/autofill_popup_view_delegate.h"
#include "ui/views/widget/widget_delegate.h"
#include "ui/views/widget/widget_observer.h"

namespace content {
class WebContents;
}

namespace gfx {
class Point;
}

namespace autofill {

// Class that deals with the event handling for Autofill-style popups. This
// class should only be instantiated by sub-classes.
class AutofillPopupBaseView : public views::WidgetDelegateView,
                              public views::WidgetObserver {
 protected:
  explicit AutofillPopupBaseView(AutofillPopupViewDelegate* delegate,
                                 views::Widget* observing_widget);
  virtual ~AutofillPopupBaseView();

  // Show this popup. Idempotent.
  void DoShow();

  // Hide the widget and delete |this|.
  void DoHide();

  // Update size of popup and paint.
  void DoUpdateBoundsAndRedrawPopup();

  static const SkColor kBorderColor;
  static const SkColor kHoveredBackgroundColor;
  static const SkColor kItemTextColor;
  static const SkColor kPopupBackground;
  static const SkColor kValueTextColor;
  static const SkColor kWarningTextColor;

 private:
  friend class AutofillPopupBaseViewTest;

  // views::Views implementation.
  virtual void OnMouseCaptureLost() OVERRIDE;
  virtual bool OnMouseDragged(const ui::MouseEvent& event) OVERRIDE;
  virtual void OnMouseExited(const ui::MouseEvent& event) OVERRIDE;
  virtual void OnMouseMoved(const ui::MouseEvent& event) OVERRIDE;
  virtual bool OnMousePressed(const ui::MouseEvent& event) OVERRIDE;
  virtual void OnMouseReleased(const ui::MouseEvent& event) OVERRIDE;
  virtual void OnGestureEvent(ui::GestureEvent* event) OVERRIDE;
  virtual bool AcceleratorPressed(const ui::Accelerator& accelerator) OVERRIDE;

  // views::WidgetObserver implementation.
  virtual void OnWidgetBoundsChanged(views::Widget* widget,
                                     const gfx::Rect& new_bounds) OVERRIDE;

  // Stop observing the |observing_widget_|.
  void RemoveObserver();

  void SetSelection(const gfx::Point& point);
  void AcceptSelection(const gfx::Point& point);
  void ClearSelection();

  // If the popup should be hidden if the user clicks outside it's bounds.
  bool ShouldHideOnOutsideClick();

  // Hide the controller of this view. This assumes that doing so will
  // eventually hide this view in the process.
  void HideController();

  // Returns true if this event should be passed along.
  bool ShouldRepostEvent(const ui::MouseEvent& event);

  // Must return the container view for this popup.
  gfx::NativeView container_view();

  // Controller for this popup. Weak reference.
  AutofillPopupViewDelegate* delegate_;

  // The widget that |this| observes. Weak reference.
  views::Widget* observing_widget_;

  base::WeakPtrFactory<AutofillPopupBaseView> weak_ptr_factory_;

  DISALLOW_COPY_AND_ASSIGN(AutofillPopupBaseView);
};

}  // namespace autofill

#endif  // CHROME_BROWSER_UI_VIEWS_AUTOFILL_AUTOFILL_POPUP_BASE_VIEW_H_
