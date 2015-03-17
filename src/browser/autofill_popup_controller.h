// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_AUTOFILL_AUTOFILL_POPUP_CONTROLLER_H_
#define CHROME_BROWSER_UI_AUTOFILL_AUTOFILL_POPUP_CONTROLLER_H_

#include <vector>

#include "base/compiler_specific.h"
#include "base/strings/string16.h"
#include "chrome/browser/ui/autofill/autofill_popup_view_delegate.h"

namespace gfx {
class FontList;
class Point;
class Rect;
}

namespace autofill {

struct Suggestion;

// This interface provides data to an AutofillPopupView.
class AutofillPopupController : public AutofillPopupViewDelegate {
 public:
  // Recalculates the height and width of the popup and triggers a redraw.
  virtual void UpdateBoundsAndRedrawPopup() = 0;

  // Accepts the suggestion at |index|.
  virtual void AcceptSuggestion(size_t index) = 0;

  // Gets the resource value for the given resource, returning -1 if the
  // resource isn't recognized.
  virtual int GetIconResourceID(const base::string16& resource_name) const = 0;

  // Returns true if the given index refers to an element that can be deleted.
  virtual bool CanDelete(size_t index) const = 0;

  // Returns true if the given index refers to an element that is a warning
  // rather than an Autofill suggestion.
  virtual bool IsWarning(size_t index) const = 0;

  // Updates the bounds of the popup and initiates a redraw.
  virtual void SetPopupBounds(const gfx::Rect& bounds) = 0;

  // Returns the bounds of the item at |index| in the popup, relative to
  // the top left of the popup.
  virtual gfx::Rect GetRowBounds(size_t index) = 0;

  // Returns the number of lines of data that there are.
  virtual size_t GetLineCount() const = 0;

  // Returns the suggestion or pre-elided string at the given row index.
  virtual const autofill::Suggestion& GetSuggestionAt(size_t row) const = 0;
  virtual const base::string16& GetElidedValueAt(size_t row) const = 0;
  virtual const base::string16& GetElidedLabelAt(size_t row) const = 0;

#if !defined(OS_ANDROID)
  // The same font can vary based on the type of data it is showing,
  // so we need to know the row.
  virtual const gfx::FontList& GetValueFontListForRow(size_t index) const = 0;
  virtual const gfx::FontList& GetLabelFontList() const = 0;
#endif

  // Returns the index of the selected line. A line is "selected" when it is
  // hovered or has keyboard focus.
  virtual int selected_line() const = 0;

 protected:
  ~AutofillPopupController() override {}
};

}  // namespace autofill

#endif  // CHROME_BROWSER_UI_AUTOFILL_AUTOFILL_POPUP_CONTROLLER_H_
