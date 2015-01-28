// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/autofill/autofill_popup_controller_impl.h"

#include <algorithm>
#include <utility>

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/ui/autofill/autofill_popup_view.h"
#include "chrome/browser/ui/autofill/popup_constants.h"
#include "components/autofill/core/browser/autofill_popup_delegate.h"
#include "components/autofill/core/browser/popup_item_ids.h"
#include "components/autofill/core/browser/suggestion.h"
#include "content/public/browser/native_web_keyboard_event.h"
#include "grit/components_scaled_resources.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/events/event.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/geometry/vector2d.h"
#include "ui/gfx/screen.h"
#include "ui/gfx/text_elider.h"
#include "ui/gfx/text_utils.h"

using base::WeakPtr;

namespace autofill {
namespace {

// Used to indicate that no line is currently selected by the user.
const int kNoSelection = -1;

// The vertical height of each row in pixels.
const size_t kRowHeight = 24;

// The vertical height of a separator in pixels.
const size_t kSeparatorHeight = 1;

#if !defined(OS_ANDROID)
// Size difference between name and label in pixels.
const int kLabelFontSizeDelta = -2;

const size_t kNamePadding = AutofillPopupView::kNamePadding;
const size_t kIconPadding = AutofillPopupView::kIconPadding;
const size_t kEndPadding = AutofillPopupView::kEndPadding;
#endif

struct DataResource {
  const char* name;
  int id;
};

const DataResource kDataResources[] = {
  { "americanExpressCC", IDR_AUTOFILL_CC_AMEX },
  { "dinersCC", IDR_AUTOFILL_CC_GENERIC },
  { "discoverCC", IDR_AUTOFILL_CC_DISCOVER },
  { "genericCC", IDR_AUTOFILL_CC_GENERIC },
  { "jcbCC", IDR_AUTOFILL_CC_GENERIC },
  { "masterCardCC", IDR_AUTOFILL_CC_MASTERCARD },
  { "visaCC", IDR_AUTOFILL_CC_VISA },
  { "scanCreditCardIcon", IDR_AUTOFILL_CC_SCAN_NEW },
#if defined(OS_MACOSX) && !defined(OS_IOS)
  { "macContactsIcon", IDR_AUTOFILL_MAC_CONTACTS_ICON },
#endif  // defined(OS_MACOSX) && !defined(OS_IOS)
};

}  // namespace

// static
WeakPtr<AutofillPopupControllerImpl> AutofillPopupControllerImpl::GetOrCreate(
    WeakPtr<AutofillPopupControllerImpl> previous,
    WeakPtr<AutofillPopupDelegate> delegate,
    content::WebContents* web_contents,
    gfx::NativeView container_view,
    const gfx::RectF& element_bounds,
    base::i18n::TextDirection text_direction) {
  if (previous.get() && previous->web_contents() == web_contents &&
      previous->delegate_.get() == delegate.get() &&
      previous->container_view() == container_view &&
      previous->element_bounds() == element_bounds) {
    previous->ClearState();
    return previous;
  }

  if (previous.get())
    previous->Hide();

  AutofillPopupControllerImpl* controller =
      new AutofillPopupControllerImpl(
          delegate, web_contents, container_view, element_bounds,
          text_direction);
  return controller->GetWeakPtr();
}

AutofillPopupControllerImpl::AutofillPopupControllerImpl(
    base::WeakPtr<AutofillPopupDelegate> delegate,
    content::WebContents* web_contents,
    gfx::NativeView container_view,
    const gfx::RectF& element_bounds,
    base::i18n::TextDirection text_direction)
    : controller_common_(new PopupControllerCommon(element_bounds,
                                                   container_view,
                                                   web_contents)),
      view_(NULL),
      delegate_(delegate),
      text_direction_(text_direction),
      weak_ptr_factory_(this) {
  ClearState();
  controller_common_->SetKeyPressCallback(
      base::Bind(&AutofillPopupControllerImpl::HandleKeyPressEvent,
                 base::Unretained(this)));
#if !defined(OS_ANDROID)
  label_font_list_ = value_font_list_.DeriveWithSizeDelta(kLabelFontSizeDelta);
  title_font_list_ = value_font_list_.DeriveWithStyle(gfx::Font::BOLD);
#if defined(OS_MACOSX)
  // There is no italic version of the system font.
  warning_font_list_ = value_font_list_;
#else
  warning_font_list_ = value_font_list_.DeriveWithStyle(gfx::Font::ITALIC);
#endif
#endif
}

AutofillPopupControllerImpl::~AutofillPopupControllerImpl() {}

void AutofillPopupControllerImpl::Show(
    const std::vector<autofill::Suggestion>& suggestions) {
  SetValues(suggestions);
  DCHECK_EQ(suggestions_.size(), elided_values_.size());
  DCHECK_EQ(suggestions_.size(), elided_labels_.size());

#if !defined(OS_ANDROID)
  // Android displays the long text with ellipsis using the view attributes.

  UpdatePopupBounds();
  int popup_width = popup_bounds().width();

  // Elide the name and label strings so that the popup fits in the available
  // space.
  for (size_t i = 0; i < suggestions_.size(); ++i) {
    int value_width =
        gfx::GetStringWidth(suggestions_[i].value, GetValueFontListForRow(i));
    int label_width =
        gfx::GetStringWidth(suggestions_[i].label, GetLabelFontList());
    int total_text_length = value_width + label_width;

    // The line can have no strings if it represents a UI element, such as
    // a separator line.
    if (total_text_length == 0)
      continue;

    int available_width = popup_width - RowWidthWithoutText(i);

    // Each field receives space in proportion to its length.
    int value_size = available_width * value_width / total_text_length;
    elided_values_[i] = gfx::ElideText(suggestions_[i].value,
                                       GetValueFontListForRow(i),
                                       value_size, gfx::ELIDE_TAIL);

    int label_size = available_width * label_width / total_text_length;
    elided_labels_[i] = gfx::ElideText(suggestions_[i].label,
                                       GetLabelFontList(),
                                       label_size, gfx::ELIDE_TAIL);
  }
#endif

  if (!view_) {
    view_ = AutofillPopupView::Create(this);

    // It is possible to fail to create the popup, in this case
    // treat the popup as hiding right away.
    if (!view_) {
      Hide();
      return;
    }

    ShowView();
  } else {
    UpdateBoundsAndRedrawPopup();
  }

  controller_common_->RegisterKeyPressCallback();
  delegate_->OnPopupShown();

  DCHECK_EQ(suggestions_.size(), elided_values_.size());
  DCHECK_EQ(suggestions_.size(), elided_labels_.size());
}

void AutofillPopupControllerImpl::UpdateDataListValues(
    const std::vector<base::string16>& values,
    const std::vector<base::string16>& labels) {
  DCHECK_EQ(suggestions_.size(), elided_values_.size());
  DCHECK_EQ(suggestions_.size(), elided_labels_.size());

  // Remove all the old data list values, which should always be at the top of
  // the list if they are present.
  while (!suggestions_.empty() &&
         suggestions_[0].frontend_id == POPUP_ITEM_ID_DATALIST_ENTRY) {
    suggestions_.erase(suggestions_.begin());
    elided_values_.erase(elided_values_.begin());
    elided_labels_.erase(elided_labels_.begin());
  }

  // If there are no new data list values, exit (clearing the separator if there
  // is one).
  if (values.empty()) {
    if (!suggestions_.empty() &&
        suggestions_[0].frontend_id == POPUP_ITEM_ID_SEPARATOR) {
      suggestions_.erase(suggestions_.begin());
      elided_values_.erase(elided_values_.begin());
      elided_labels_.erase(elided_labels_.begin());
    }

     // The popup contents have changed, so either update the bounds or hide it.
    if (HasSuggestions())
      UpdateBoundsAndRedrawPopup();
    else
      Hide();

    return;
  }

  // Add a separator if there are any other values.
  if (!suggestions_.empty() &&
      suggestions_[0].frontend_id != POPUP_ITEM_ID_SEPARATOR) {
    suggestions_.insert(suggestions_.begin(), autofill::Suggestion());
    suggestions_[0].frontend_id = POPUP_ITEM_ID_SEPARATOR;
    elided_values_.insert(elided_values_.begin(), base::string16());
    elided_labels_.insert(elided_labels_.begin(), base::string16());
  }

  // Prepend the parameters to the suggestions we already have.
  suggestions_.insert(suggestions_.begin(), values.size(), Suggestion());
  elided_values_.insert(elided_values_.begin(), values.size(),
                        base::string16());
  elided_labels_.insert(elided_labels_.begin(), values.size(),
                        base::string16());
  for (size_t i = 0; i < values.size(); i++) {
    suggestions_[i].value = values[i];
    suggestions_[i].label = labels[i];
    suggestions_[i].frontend_id = POPUP_ITEM_ID_DATALIST_ENTRY;

    // TODO(brettw) it looks like these should be elided.
    elided_values_[i] = values[i];
    elided_labels_[i] = labels[i];
  }

  UpdateBoundsAndRedrawPopup();
  DCHECK_EQ(suggestions_.size(), elided_values_.size());
  DCHECK_EQ(suggestions_.size(), elided_labels_.size());
}

void AutofillPopupControllerImpl::Hide() {
  controller_common_->RemoveKeyPressCallback();
  if (delegate_)
    delegate_->OnPopupHidden();

  if (view_)
    view_->Hide();

  delete this;
}

void AutofillPopupControllerImpl::ViewDestroyed() {
  // The view has already been destroyed so clear the reference to it.
  view_ = NULL;

  Hide();
}

bool AutofillPopupControllerImpl::HandleKeyPressEvent(
    const content::NativeWebKeyboardEvent& event) {
  switch (event.windowsKeyCode) {
    case ui::VKEY_UP:
      SelectPreviousLine();
      return true;
    case ui::VKEY_DOWN:
      SelectNextLine();
      return true;
    case ui::VKEY_PRIOR:  // Page up.
      // Set no line and then select the next line in case the first line is not
      // selectable.
      SetSelectedLine(kNoSelection);
      SelectNextLine();
      return true;
    case ui::VKEY_NEXT:  // Page down.
      SetSelectedLine(GetLineCount() - 1);
      return true;
    case ui::VKEY_ESCAPE:
      Hide();
      return true;
    case ui::VKEY_DELETE:
      return (event.modifiers & content::NativeWebKeyboardEvent::ShiftKey) &&
             RemoveSelectedLine();
    case ui::VKEY_TAB:
      // A tab press should cause the selected line to be accepted, but still
      // return false so the tab key press propagates and changes the cursor
      // location.
      AcceptSelectedLine();
      return false;
    case ui::VKEY_RETURN:
      return AcceptSelectedLine();
    default:
      return false;
  }
}

void AutofillPopupControllerImpl::UpdateBoundsAndRedrawPopup() {
#if !defined(OS_ANDROID)
  // TODO(csharp): Since UpdatePopupBounds can change the position of the popup,
  // the popup could end up jumping from above the element to below it.
  // It is unclear if it is better to keep the popup where it was, or if it
  // should try and move to its desired position.
  UpdatePopupBounds();
#endif

  view_->UpdateBoundsAndRedrawPopup();
}

void AutofillPopupControllerImpl::SetSelectionAtPoint(const gfx::Point& point) {
  SetSelectedLine(LineFromY(point.y()));
}

bool AutofillPopupControllerImpl::AcceptSelectedLine() {
  if (selected_line_ == kNoSelection)
    return false;

  DCHECK_GE(selected_line_, 0);
  DCHECK_LT(selected_line_, static_cast<int>(GetLineCount()));

  if (!CanAccept(suggestions_[selected_line_].frontend_id))
    return false;

  AcceptSuggestion(selected_line_);
  return true;
}

void AutofillPopupControllerImpl::SelectionCleared() {
  SetSelectedLine(kNoSelection);
}

void AutofillPopupControllerImpl::AcceptSuggestion(size_t index) {
  const autofill::Suggestion& suggestion = suggestions_[index];
  delegate_->DidAcceptSuggestion(suggestion.value, suggestion.frontend_id);
}

int AutofillPopupControllerImpl::GetIconResourceID(
    const base::string16& resource_name) const {
  for (size_t i = 0; i < arraysize(kDataResources); ++i) {
    if (resource_name == base::ASCIIToUTF16(kDataResources[i].name))
      return kDataResources[i].id;
  }

  return -1;
}

bool AutofillPopupControllerImpl::CanDelete(size_t index) const {
  // TODO(isherman): Native AddressBook suggestions on Mac and Android should
  // not be considered to be deleteable.
  int id = suggestions_[index].frontend_id;
  return id > 0 || id == POPUP_ITEM_ID_AUTOCOMPLETE_ENTRY ||
         id == POPUP_ITEM_ID_PASSWORD_ENTRY;
}

bool AutofillPopupControllerImpl::IsWarning(size_t index) const {
  return suggestions_[index].frontend_id == POPUP_ITEM_ID_WARNING_MESSAGE;
}

gfx::Rect AutofillPopupControllerImpl::GetRowBounds(size_t index) {
  int top = kPopupBorderThickness;
  for (size_t i = 0; i < index; ++i) {
    top += GetRowHeightFromId(suggestions_[i].frontend_id);
  }

  return gfx::Rect(
      kPopupBorderThickness,
      top,
      popup_bounds_.width() - 2 * kPopupBorderThickness,
      GetRowHeightFromId(suggestions_[index].frontend_id));
}

void AutofillPopupControllerImpl::SetPopupBounds(const gfx::Rect& bounds) {
  popup_bounds_ = bounds;
  UpdateBoundsAndRedrawPopup();
}

const gfx::Rect& AutofillPopupControllerImpl::popup_bounds() const {
  return popup_bounds_;
}

content::WebContents* AutofillPopupControllerImpl::web_contents() {
  return controller_common_->web_contents();
}

gfx::NativeView AutofillPopupControllerImpl::container_view() {
  return controller_common_->container_view();
}

const gfx::RectF& AutofillPopupControllerImpl::element_bounds() const {
  return controller_common_->element_bounds();
}

bool AutofillPopupControllerImpl::IsRTL() const {
  return text_direction_ == base::i18n::RIGHT_TO_LEFT;
}

size_t AutofillPopupControllerImpl::GetLineCount() const {
  return suggestions_.size();
}

const autofill::Suggestion& AutofillPopupControllerImpl::GetSuggestionAt(
    size_t row) const {
  return suggestions_[row];
}

const base::string16& AutofillPopupControllerImpl::GetElidedValueAt(
    size_t row) const {
  return elided_values_[row];
}

const base::string16& AutofillPopupControllerImpl::GetElidedLabelAt(
    size_t row) const {
  return elided_labels_[row];
}

#if !defined(OS_ANDROID)
const gfx::FontList& AutofillPopupControllerImpl::GetValueFontListForRow(
    size_t index) const {
  if (suggestions_[index].frontend_id == POPUP_ITEM_ID_WARNING_MESSAGE)
    return warning_font_list_;

  if (suggestions_[index].frontend_id == POPUP_ITEM_ID_TITLE)
    return title_font_list_;

  return value_font_list_;
}

const gfx::FontList& AutofillPopupControllerImpl::GetLabelFontList() const {
  return label_font_list_;
}
#endif

int AutofillPopupControllerImpl::selected_line() const {
  return selected_line_;
}

void AutofillPopupControllerImpl::SetSelectedLine(int selected_line) {
  if (selected_line_ == selected_line)
    return;

  if (selected_line_ != kNoSelection &&
      static_cast<size_t>(selected_line_) < suggestions_.size())
    InvalidateRow(selected_line_);

  if (selected_line != kNoSelection) {
    InvalidateRow(selected_line);

    if (!CanAccept(suggestions_[selected_line].frontend_id))
      selected_line = kNoSelection;
  }

  selected_line_ = selected_line;

  if (selected_line_ != kNoSelection) {
    delegate_->DidSelectSuggestion(elided_values_[selected_line_],
                                   suggestions_[selected_line_].frontend_id);
  } else {
    delegate_->ClearPreviewedForm();
  }
}

void AutofillPopupControllerImpl::SelectNextLine() {
  int new_selected_line = selected_line_ + 1;

  // Skip over any lines that can't be selected.
  while (static_cast<size_t>(new_selected_line) < GetLineCount() &&
         !CanAccept(suggestions_[new_selected_line].frontend_id)) {
    ++new_selected_line;
  }

  if (new_selected_line >= static_cast<int>(GetLineCount()))
    new_selected_line = 0;

  SetSelectedLine(new_selected_line);
}

void AutofillPopupControllerImpl::SelectPreviousLine() {
  int new_selected_line = selected_line_ - 1;

  // Skip over any lines that can't be selected.
  while (new_selected_line > kNoSelection &&
         !CanAccept(GetSuggestionAt(new_selected_line).frontend_id)) {
    --new_selected_line;
  }

  if (new_selected_line <= kNoSelection)
    new_selected_line = GetLineCount() - 1;

  SetSelectedLine(new_selected_line);
}

bool AutofillPopupControllerImpl::RemoveSelectedLine() {
  if (selected_line_ == kNoSelection)
    return false;

  DCHECK_GE(selected_line_, 0);
  DCHECK_LT(selected_line_, static_cast<int>(GetLineCount()));

  if (!CanDelete(selected_line_))
    return false;

  delegate_->RemoveSuggestion(suggestions_[selected_line_].value,
                              suggestions_[selected_line_].frontend_id);

  // Remove the deleted element.
  suggestions_.erase(suggestions_.begin() + selected_line_);
  elided_values_.erase(elided_values_.begin() + selected_line_);
  elided_labels_.erase(elided_labels_.begin() + selected_line_);

  SetSelectedLine(kNoSelection);

  if (HasSuggestions()) {
    delegate_->ClearPreviewedForm();
    UpdateBoundsAndRedrawPopup();
  } else {
    Hide();
  }

  return true;
}

int AutofillPopupControllerImpl::LineFromY(int y) {
  int current_height = kPopupBorderThickness;

  for (size_t i = 0; i < suggestions_.size(); ++i) {
    current_height += GetRowHeightFromId(suggestions_[i].frontend_id);

    if (y <= current_height)
      return i;
  }

  // The y value goes beyond the popup so stop the selection at the last line.
  return GetLineCount() - 1;
}

int AutofillPopupControllerImpl::GetRowHeightFromId(int identifier) const {
  if (identifier == POPUP_ITEM_ID_SEPARATOR)
    return kSeparatorHeight;

  return kRowHeight;
}

bool AutofillPopupControllerImpl::CanAccept(int id) {
  return id != POPUP_ITEM_ID_SEPARATOR && id != POPUP_ITEM_ID_WARNING_MESSAGE &&
         id != POPUP_ITEM_ID_TITLE;
}

bool AutofillPopupControllerImpl::HasSuggestions() {
  if (suggestions_.empty())
    return false;
  int id = suggestions_[0].frontend_id;
  return id > 0 ||
         id == POPUP_ITEM_ID_AUTOCOMPLETE_ENTRY ||
         id == POPUP_ITEM_ID_PASSWORD_ENTRY ||
         id == POPUP_ITEM_ID_DATALIST_ENTRY ||
         id == POPUP_ITEM_ID_MAC_ACCESS_CONTACTS ||
         id == POPUP_ITEM_ID_SCAN_CREDIT_CARD;
}

void AutofillPopupControllerImpl::SetValues(
    const std::vector<autofill::Suggestion>& suggestions) {
  suggestions_ = suggestions;
  elided_values_.resize(suggestions.size());
  elided_labels_.resize(suggestions.size());
  for (size_t i = 0; i < suggestions.size(); i++) {
    elided_values_[i] = suggestions[i].value;
    elided_labels_[i] = suggestions[i].label;
  }
}

void AutofillPopupControllerImpl::ShowView() {
  view_->Show();
}

void AutofillPopupControllerImpl::InvalidateRow(size_t row) {
  DCHECK(0 <= row);
  DCHECK(row < suggestions_.size());
  view_->InvalidateRow(row);
}

#if !defined(OS_ANDROID)
int AutofillPopupControllerImpl::GetDesiredPopupWidth() const {
  int popup_width = controller_common_->RoundedElementBounds().width();
  for (size_t i = 0; i < GetLineCount(); ++i) {
    int row_size =
        gfx::GetStringWidth(GetElidedValueAt(i), value_font_list_) +
        gfx::GetStringWidth(GetElidedLabelAt(i), label_font_list_) +
        RowWidthWithoutText(i);

    popup_width = std::max(popup_width, row_size);
  }

  return popup_width;
}

int AutofillPopupControllerImpl::GetDesiredPopupHeight() const {
  int popup_height = 2 * kPopupBorderThickness;

  for (size_t i = 0; i < suggestions_.size(); ++i) {
    popup_height += GetRowHeightFromId(suggestions_[i].frontend_id);
  }

  return popup_height;
}

int AutofillPopupControllerImpl::RowWidthWithoutText(int row) const {
  int row_size = kEndPadding;

  if (!elided_labels_[row].empty())
    row_size += kNamePadding;

  // Add the Autofill icon size, if required.
  const base::string16& icon = suggestions_[row].icon;
  if (!icon.empty()) {
    int icon_width = ui::ResourceBundle::GetSharedInstance().GetImageNamed(
        GetIconResourceID(icon)).Width();
    row_size += icon_width + kIconPadding;
  }

  // Add the padding at the end.
  row_size += kEndPadding;

  // Add room for the popup border.
  row_size += 2 * kPopupBorderThickness;

  return row_size;
}

void AutofillPopupControllerImpl::UpdatePopupBounds() {
  int popup_width = GetDesiredPopupWidth();
  int popup_height = GetDesiredPopupHeight();

  popup_bounds_ = controller_common_->GetPopupBounds(popup_width, popup_height);
}
#endif  // !defined(OS_ANDROID)

WeakPtr<AutofillPopupControllerImpl> AutofillPopupControllerImpl::GetWeakPtr() {
  return weak_ptr_factory_.GetWeakPtr();
}

void AutofillPopupControllerImpl::ClearState() {
  // Don't clear view_, because otherwise the popup will have to get regenerated
  // and this will cause flickering.

  popup_bounds_ = gfx::Rect();

  suggestions_.clear();
  elided_values_.clear();
  elided_labels_.clear();

  selected_line_ = kNoSelection;
}

}  // namespace autofill
