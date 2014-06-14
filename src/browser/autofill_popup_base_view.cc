// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/autofill_popup_base_view.h"

#include "base/bind.h"
#include "base/location.h"
#include "base/message_loop/message_loop.h"
#include "chrome/browser/ui/autofill/popup_constants.h"
#include "ui/gfx/point.h"
#include "ui/gfx/screen.h"
#include "ui/views/border.h"
#include "ui/views/event_utils.h"
#include "ui/views/widget/widget.h"

#if defined(USE_AURA)
#include "ui/wm/core/window_animations.h"
#endif

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
  if (!GetWidget()) {
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
#if defined(USE_AURA)
    // No animation for popup appearance (too distracting).
    wm::SetWindowVisibilityAnimationTransition(
        widget->GetNativeView(), wm::ANIMATE_HIDE);
#endif
  }

  SetBorder(views::Border::CreateSolidBorder(kPopupBorderThickness,
                                             kBorderColor));

  DoUpdateBoundsAndRedrawPopup();
  GetWidget()->Show();

  if (ShouldHideOnOutsideClick())
    GetWidget()->SetCapture(this);
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
}

void AutofillPopupBaseView::DoUpdateBoundsAndRedrawPopup() {
  GetWidget()->SetBounds(delegate_->popup_bounds());
  SchedulePaint();
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
  if (HitTestPoint(event.location()))
    return true;

  if (ShouldHideOnOutsideClick()) {
    GetWidget()->ReleaseCapture();

    gfx::Point screen_loc = event.location();
    views::View::ConvertPointToScreen(this, &screen_loc);

    ui::MouseEvent mouse_event = event;
    mouse_event.set_location(screen_loc);

    if (ShouldRepostEvent(mouse_event)) {
      gfx::NativeView native_view = GetWidget()->GetNativeView();
      gfx::Screen* screen = gfx::Screen::GetScreenFor(native_view);
      gfx::NativeWindow window = screen->GetWindowAtScreenPoint(screen_loc);
      views::RepostLocatedEvent(window, mouse_event);
    }

    HideController();
    // |this| is now deleted.
  }

  return false;
}

void AutofillPopupBaseView::OnMouseReleased(const ui::MouseEvent& event) {
  // Because this view can can be shown in response to a mouse press, it can
  // receive an OnMouseReleased event just after showing. This breaks the mouse
  // capture, so restart capturing here.
  if (ShouldHideOnOutsideClick() && GetWidget())
    GetWidget()->SetCapture(this);

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

bool AutofillPopupBaseView::ShouldHideOnOutsideClick() {
  if (delegate_)
    return delegate_->ShouldHideOnOutsideClick();

  // |this| instance should be in the process of being destroyed, so the return
  // value shouldn't matter.
  return false;
}

void AutofillPopupBaseView::HideController() {
  if (delegate_)
    delegate_->Hide();
}

bool AutofillPopupBaseView::ShouldRepostEvent(const ui::MouseEvent& event) {
  return delegate_->ShouldRepostEvent(event);
}

gfx::NativeView AutofillPopupBaseView::container_view() {
  return delegate_->container_view();
}


}  // namespace autofill
