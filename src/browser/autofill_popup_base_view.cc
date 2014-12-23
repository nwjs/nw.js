// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/autofill_popup_base_view.h"

#include "base/bind.h"
#include "base/location.h"
#include "base/message_loop/message_loop.h"
#include "chrome/browser/ui/autofill/popup_constants.h"
#include "ui/views/border.h"
#include "ui/views/widget/widget.h"

namespace autofill {

const SkColor AutofillPopupBaseView::kBorderColor =
    SkColorSetARGB(0xFF, 0xC7, 0xCA, 0xCE);
const SkColor AutofillPopupBaseView::kHoveredBackgroundColor =
    SkColorSetARGB(0xFF, 0xCD, 0xCD, 0xCD);
const SkColor AutofillPopupBaseView::kItemTextColor =
    SkColorSetARGB(0xFF, 0x7F, 0x7F, 0x7F);
const SkColor AutofillPopupBaseView::kPopupBackground =
    SkColorSetARGB(0xFF, 0xFF, 0xFF, 0xFF);
const SkColor AutofillPopupBaseView::kValueTextColor =
    SkColorSetARGB(0xFF, 0x00, 0x00, 0x00);
const SkColor AutofillPopupBaseView::kWarningTextColor =
    SkColorSetARGB(0xFF, 0x7F, 0x7F, 0x7F);

AutofillPopupBaseView::AutofillPopupBaseView(
    AutofillPopupViewDelegate* delegate,
    views::Widget* observing_widget)
    : delegate_(delegate),
      observing_widget_(observing_widget),
      weak_ptr_factory_(this) {}

AutofillPopupBaseView::~AutofillPopupBaseView() {
  if (delegate_) {
    delegate_->ViewDestroyed();

    RemoveObserver();
  }
}

void AutofillPopupBaseView::DoShow() {
  const bool initialize_widget = !GetWidget();
  if (initialize_widget) {
    observing_widget_->AddObserver(this);

    views::FocusManager* focus_manager = observing_widget_->GetFocusManager();
    focus_manager->RegisterAccelerator(
        ui::Accelerator(ui::VKEY_RETURN, ui::EF_NONE),
        ui::AcceleratorManager::kNormalPriority,
        this);
    focus_manager->RegisterAccelerator(
        ui::Accelerator(ui::VKEY_ESCAPE, ui::EF_NONE),
        ui::AcceleratorManager::kNormalPriority,
        this);

    // The widget is destroyed by the corresponding NativeWidget, so we use
    // a weak pointer to hold the reference and don't have to worry about
    // deletion.
    views::Widget* widget = new views::Widget;
    views::Widget::InitParams params(views::Widget::InitParams::TYPE_POPUP);
    params.delegate = this;
    params.parent = container_view();
    widget->Init(params);
    widget->SetContentsView(this);

    // No animation for popup appearance (too distracting).
    widget->SetVisibilityAnimationTransition(views::Widget::ANIMATE_HIDE);
  }

  SetBorder(views::Border::CreateSolidBorder(kPopupBorderThickness,
                                             kBorderColor));

  DoUpdateBoundsAndRedrawPopup();
  GetWidget()->Show();

  // Showing the widget can change native focus (which would result in an
  // immediate hiding of the popup). Only start observing after shown.
  if (initialize_widget)
    views::WidgetFocusManager::GetInstance()->AddFocusChangeListener(this);
}

void AutofillPopupBaseView::DoHide() {
  // The controller is no longer valid after it hides us.
  delegate_ = NULL;

  RemoveObserver();

  if (GetWidget()) {
    // Don't call CloseNow() because some of the functions higher up the stack
    // assume the the widget is still valid after this point.
    // http://crbug.com/229224
    // NOTE: This deletes |this|.
    GetWidget()->Close();
  } else {
    delete this;
  }
}

void AutofillPopupBaseView::RemoveObserver() {
  observing_widget_->GetFocusManager()->UnregisterAccelerators(this);
  observing_widget_->RemoveObserver(this);
  views::WidgetFocusManager::GetInstance()->RemoveFocusChangeListener(this);
}

void AutofillPopupBaseView::DoUpdateBoundsAndRedrawPopup() {
  GetWidget()->SetBounds(delegate_->popup_bounds());
  SchedulePaint();
}

void AutofillPopupBaseView::OnNativeFocusChange(
    gfx::NativeView focused_before,
    gfx::NativeView focused_now) {
  if (GetWidget() && GetWidget()->GetNativeView() != focused_now)
    HideController();
}

void AutofillPopupBaseView::OnWidgetBoundsChanged(views::Widget* widget,
                                                  const gfx::Rect& new_bounds) {
  DCHECK_EQ(widget, observing_widget_);
  HideController();
}

void AutofillPopupBaseView::OnMouseCaptureLost() {
  ClearSelection();
}

bool AutofillPopupBaseView::OnMouseDragged(const ui::MouseEvent& event) {
  if (HitTestPoint(event.location())) {
    SetSelection(event.location());

    // We must return true in order to get future OnMouseDragged and
    // OnMouseReleased events.
    return true;
  }

  // If we move off of the popup, we lose the selection.
  ClearSelection();
  return false;
}

void AutofillPopupBaseView::OnMouseExited(const ui::MouseEvent& event) {
  // Pressing return causes the cursor to hide, which will generate an
  // OnMouseExited event. Pressing return should activate the current selection
  // via AcceleratorPressed, so we need to let that run first.
  base::MessageLoop::current()->PostTask(
      FROM_HERE,
      base::Bind(&AutofillPopupBaseView::ClearSelection,
                 weak_ptr_factory_.GetWeakPtr()));
}

void AutofillPopupBaseView::OnMouseMoved(const ui::MouseEvent& event) {
  if (HitTestPoint(event.location()))
    SetSelection(event.location());
  else
    ClearSelection();
}

bool AutofillPopupBaseView::OnMousePressed(const ui::MouseEvent& event) {
  return event.GetClickCount() == 1;
}

void AutofillPopupBaseView::OnMouseReleased(const ui::MouseEvent& event) {
  // We only care about the left click.
  if (event.IsOnlyLeftMouseButton() && HitTestPoint(event.location()))
    AcceptSelection(event.location());
}

void AutofillPopupBaseView::OnGestureEvent(ui::GestureEvent* event) {
  switch (event->type()) {
    case ui::ET_GESTURE_TAP_DOWN:
    case ui::ET_GESTURE_SCROLL_BEGIN:
    case ui::ET_GESTURE_SCROLL_UPDATE:
      if (HitTestPoint(event->location()))
        SetSelection(event->location());
      else
        ClearSelection();
      break;
    case ui::ET_GESTURE_TAP:
    case ui::ET_GESTURE_SCROLL_END:
      if (HitTestPoint(event->location()))
        AcceptSelection(event->location());
      else
        ClearSelection();
      break;
    case ui::ET_GESTURE_TAP_CANCEL:
    case ui::ET_SCROLL_FLING_START:
      ClearSelection();
      break;
    default:
      return;
  }
  event->SetHandled();
}

bool AutofillPopupBaseView::AcceleratorPressed(
    const ui::Accelerator& accelerator) {
  DCHECK_EQ(accelerator.modifiers(), ui::EF_NONE);

  if (accelerator.key_code() == ui::VKEY_ESCAPE) {
    HideController();
    return true;
  }

  if (accelerator.key_code() == ui::VKEY_RETURN)
    return delegate_->AcceptSelectedLine();

  NOTREACHED();
  return false;
}

void AutofillPopupBaseView::SetSelection(const gfx::Point& point) {
  if (delegate_)
    delegate_->SetSelectionAtPoint(point);
}

void AutofillPopupBaseView::AcceptSelection(const gfx::Point& point) {
  if (!delegate_)
    return;

  delegate_->SetSelectionAtPoint(point);
  delegate_->AcceptSelectedLine();
}

void AutofillPopupBaseView::ClearSelection() {
  if (delegate_)
    delegate_->SelectionCleared();
}

void AutofillPopupBaseView::HideController() {
  if (delegate_)
    delegate_->Hide();
}

gfx::NativeView AutofillPopupBaseView::container_view() {
  return delegate_->container_view();
}

}  // namespace autofill
