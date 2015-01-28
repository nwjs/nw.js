// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/popup_controller_common.h"

#include <algorithm>
#include <utility>

#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "ui/gfx/display.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/screen.h"
#include "ui/gfx/geometry/vector2d.h"

namespace autofill {

PopupControllerCommon::PopupControllerCommon(
    const gfx::RectF& element_bounds,
    const gfx::NativeView container_view,
    content::WebContents* web_contents)
    : element_bounds_(element_bounds),
      container_view_(container_view),
      web_contents_(web_contents),
      key_press_event_target_(NULL) {}
PopupControllerCommon::~PopupControllerCommon() {}

void PopupControllerCommon::SetKeyPressCallback(
    content::RenderWidgetHost::KeyPressEventCallback callback) {
  DCHECK(key_press_event_callback_.is_null());
  key_press_event_callback_ = callback;
}

void PopupControllerCommon::RegisterKeyPressCallback() {
  if (web_contents_ && !key_press_event_target_) {
    key_press_event_target_ = web_contents_->GetRenderViewHost();
    key_press_event_target_->AddKeyPressEventCallback(
        key_press_event_callback_);
  }
}

void PopupControllerCommon::RemoveKeyPressCallback() {
  if (web_contents_ && (!web_contents_->IsBeingDestroyed()) &&
      key_press_event_target_ == web_contents_->GetRenderViewHost()) {
    web_contents_->GetRenderViewHost()->RemoveKeyPressEventCallback(
        key_press_event_callback_);
  }
  key_press_event_target_ = NULL;
}

gfx::Display PopupControllerCommon::GetDisplayNearestPoint(
    const gfx::Point& point) const {
  return gfx::Screen::GetScreenFor(container_view_)->GetDisplayNearestPoint(
      point);
}

const gfx::Rect PopupControllerCommon::RoundedElementBounds() const {
  return gfx::ToEnclosingRect(element_bounds_);
}

std::pair<int, int> PopupControllerCommon::CalculatePopupXAndWidth(
    const gfx::Display& left_display,
    const gfx::Display& right_display,
    int popup_required_width) const {
  int leftmost_display_x = left_display.bounds().x();
  int rightmost_display_x =
      right_display.GetSizeInPixel().width() + right_display.bounds().x();

  // Calculate the start coordinates for the popup if it is growing right or
  // the end position if it is growing to the left, capped to screen space.
  int right_growth_start = std::max(leftmost_display_x,
                                    std::min(rightmost_display_x,
                                             RoundedElementBounds().x()));
  int left_growth_end = std::max(leftmost_display_x,
                                 std::min(rightmost_display_x,
                                          RoundedElementBounds().right()));

  int right_available = rightmost_display_x - right_growth_start;
  int left_available = left_growth_end - leftmost_display_x;

  int popup_width = std::min(popup_required_width,
                             std::max(right_available, left_available));

  // If there is enough space for the popup on the right, show it there,
  // otherwise choose the larger size.
  if (right_available >= popup_width || right_available >= left_available)
    return std::make_pair(right_growth_start, popup_width);
  else
    return std::make_pair(left_growth_end - popup_width, popup_width);
}

std::pair<int,int> PopupControllerCommon::CalculatePopupYAndHeight(
    const gfx::Display& top_display,
    const gfx::Display& bottom_display,
    int popup_required_height) const {
  int topmost_display_y = top_display.bounds().y();
  int bottommost_display_y =
      bottom_display.GetSizeInPixel().height() + bottom_display.bounds().y();

  // Calculate the start coordinates for the popup if it is growing down or
  // the end position if it is growing up, capped to screen space.
  int top_growth_end = std::max(topmost_display_y,
                                std::min(bottommost_display_y,
                                         RoundedElementBounds().y()));
  int bottom_growth_start = std::max(topmost_display_y,
                                     std::min(bottommost_display_y,
                                              RoundedElementBounds().bottom()));

  int top_available = bottom_growth_start - topmost_display_y;
  int bottom_available = bottommost_display_y - top_growth_end;

  // TODO(csharp): Restrict the popup height to what is available.
  if (bottom_available >= popup_required_height ||
      bottom_available >= top_available) {
    // The popup can appear below the field.
    return std::make_pair(bottom_growth_start, popup_required_height);
  } else {
    // The popup must appear above the field.
    return std::make_pair(top_growth_end - popup_required_height,
                          popup_required_height);
  }
}

gfx::Rect PopupControllerCommon::GetPopupBounds(int desired_width,
                                                int desired_height) const {
  // This is the top left point of the popup if the popup is above the element
  // and grows to the left (since that is the highest and furthest left the
  // popup go could).
  gfx::Point top_left_corner_of_popup = RoundedElementBounds().origin() +
      gfx::Vector2d(RoundedElementBounds().width() - desired_width,
                    -desired_height);

  // This is the bottom right point of the popup if the popup is below the
  // element and grows to the right (since the is the lowest and furthest right
  // the popup could go).
  gfx::Point bottom_right_corner_of_popup = RoundedElementBounds().origin() +
      gfx::Vector2d(desired_width,
                    RoundedElementBounds().height() + desired_height);

  gfx::Display top_left_display = GetDisplayNearestPoint(
      top_left_corner_of_popup);
  gfx::Display bottom_right_display = GetDisplayNearestPoint(
      bottom_right_corner_of_popup);

  std::pair<int, int> popup_x_and_width =
      CalculatePopupXAndWidth(top_left_display,
                              bottom_right_display,
                              desired_width);
  std::pair<int, int> popup_y_and_height =
      CalculatePopupYAndHeight(top_left_display,
                               bottom_right_display,
                               desired_height);

  return gfx::Rect(popup_x_and_width.first,
                   popup_y_and_height.first,
                   popup_x_and_width.second,
                   popup_y_and_height.second);
}

}  // namespace autofill
