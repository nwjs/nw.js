// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/autofill_popup_controller_impl.h"

#include <algorithm>
#include <utility>

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "content/nw/src/browser/autofill_popup_view.h"
#include "chrome/browser/ui/autofill/popup_constants.h"
#include "components/autofill/core/browser/autofill_popup_delegate.h"
#include "components/autofill/core/browser/popup_item_ids.h"
#include "content/public/browser/native_web_keyboard_event.h"
#include "grit/component_scaled_resources.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/events/event.h"
#include "ui/gfx/rect_conversions.h"
#include "ui/gfx/screen.h"
#include "ui/gfx/text_elider.h"
#include "ui/gfx/text_utils.h"
#include "ui/gfx/vector2d.h"

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
// Size difference between name and subtext in pixels.
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
  { "dinersCC", IDR_AUTOFILL_CC_DINERS },
  { "discoverCC", IDR_AUTOFILL_CC_DISCOVER },
  { "genericCC", IDR_AUTOFILL_CC_GENERIC },
  { "jcbCC", IDR_AUTOFILL_CC_JCB },
  { "masterCardCC", IDR_AUTOFILL_CC_MASTERCARD },
  { "visaCC", IDR_AUTOFILL_CC_VISA },
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
  DCHECK(!previous.get() || previous->delegate_.get() == delegate.get());

  if (previous.get() && previous->web_contents() == web_contents &&
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
      hide_on_outside_click_(false),
      weak_ptr_factory_(this) {
  ClearState();
  controller_common_->SetKeyPressCallback(
      base::Bind(&AutofillPopupControllerImpl::HandleKeyPressEvent,
                 base::Unretained(this)));
#if !defined(OS_ANDROID)
  subtext_font_list_ = name_font_list_.DeriveWithSizeDelta(kLabelFontSizeDelta);
#if defined(OS_MACOSX)
  // There is no italic version of the system font.
  warning_font_list_ = name_font_list_;
#else
  warning_font_list_ = name_font_list_.DeriveWithStyle(gfx::Font::ITALIC);
#endif
#endif
}

AutofillPopupControllerImpl::~AutofillPopupControllerImpl() {}

void AutofillPopupControllerImpl::Show(
    const std::vector<base::string16>& names,
    const std::vector<base::string16>& subtexts,
    const std::vector<base::string16>& icons,
    const std::vector<int>& identifiers) {
  SetValues(names, subtexts, icons, identifiers);

#if !defined(OS_ANDROID)
  // Android displays the long text with ellipsis using the view attributes.

  UpdatePopupBounds();
  int popup_width = popup_bounds().width();

  // Elide the name and subtext strings so that the popup fits in the available
  // space.
  for (size_t i = 0; i < names_.size(); ++i) {
    int name_width = gfx::GetStringWidth(names_[i], GetNameFontListForRow(i));
    int subtext_width = gfx::GetStringWidth(subtexts_[i], subtext_font_list());
    int total_text_length = name_width + subtext_width;

    // The line can have no strings if it represents a UI element, such as
    // a separator line.
    if (total_text_length == 0)
      continue;

    int available_width = popup_width - RowWidthWithoutText(i);

    // Each field recieves space in proportion to its length.
    int name_size = available_width * name_width / total_text_length;
    names_[i] = gfx::ElideText(names_[i],
                               GetNameFontListForRow(i),
                               name_size,
                               gfx::ELIDE_TAIL);

    int subtext_size = available_width * subtext_width / total_text_length;
    subtexts_[i] = gfx::ElideText(subtexts_[i],
                                  subtext_font_list(),
                                  subtext_size,
                                  gfx::ELIDE_TAIL);
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

  delegate_->OnPopupShown();
  controller_common_->RegisterKeyPressCallback();
}

void AutofillPopupControllerImpl::UpdateDataListValues(
    const std::vector<base::string16>& values,
    const std::vector<base::string16>& labels) {
  // Remove all the old data list values, which should always be at the top of
  // the list if they are present.
  while (!identifiers_.empty() &&
         identifiers_[0] == POPUP_ITEM_ID_DATALIST_ENTRY) {
    names_.erase(names_.begin());
    subtexts_.erase(subtexts_.begin());
    icons_.erase(icons_.begin());
    identifiers_.erase(identifiers_.begin());
  }

  // If there are no new data list values, exit (clearing the separator if there
  // is one).
  if (values.empty()) {
    if (!identifiers_.empty() && identifiers_[0] == POPUP_ITEM_ID_SEPARATOR) {
      names_.erase(names_.begin());
      subtexts_.erase(subtexts_.begin());
      icons_.erase(icons_.begin());
      identifiers_.erase(identifiers_.begin());
    }

     // The popup contents have changed, so either update the bounds or hide it.
    if (HasSuggestions())
      UpdateBoundsAndRedrawPopup();
    else
      Hide();

    return;
  }

  // Add a separator if there are any other values.
  if (!identifiers_.empty() && identifiers_[0] != POPUP_ITEM_ID_SEPARATOR) {
    names_.insert(names_.begin(), base::string16());
    subtexts_.insert(subtexts_.begin(), base::string16());
    icons_.insert(icons_.begin(), base::string16());
    identifiers_.insert(identifiers_.begin(), POPUP_ITEM_ID_SEPARATOR);
  }


  names_.insert(names_.begin(), values.begin(), values.end());
  subtexts_.insert(subtexts_.begin(), labels.begin(), labels.end());

  // Add the values that are the same for all data list elements.
  icons_.insert(icons_.begin(), values.size(), base::string16());
  identifiers_.insert(
      identifiers_.begin(), values.size(), POPUP_ITEM_ID_DATALIST_ENTRY);

  UpdateBoundsAndRedrawPopup();
}

void AutofillPopupControllerImpl::Hide() {
  controller_common_->RemoveKeyPressCallback();
  if (delegate_.get())
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
      SetSelectedLine(0);
      return true;
    case ui::VKEY_NEXT:  // Page down.
      SetSelectedLine(names().size() - 1);
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
  DCHECK_LT(selected_line_, static_cast<int>(names_.size()));

  if (!CanAccept(identifiers_[selected_line_]))
    return false;

  AcceptSuggestion(selected_line_);
  return true;
}

void AutofillPopupControllerImpl::SelectionCleared() {
  SetSelectedLine(kNoSelection);
}

void AutofillPopupControllerImpl::AcceptSuggestion(size_t index) {
  delegate_->DidAcceptSuggestion(full_names_[index], identifiers_[index]);
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
  int id = identifiers_[index];
  return id > 0 || id == POPUP_ITEM_ID_AUTOCOMPLETE_ENTRY ||
         id == POPUP_ITEM_ID_PASSWORD_ENTRY;
}

bool AutofillPopupControllerImpl::IsWarning(size_t index) const {
  return identifiers_[index] == POPUP_ITEM_ID_WARNING_MESSAGE;
}

gfx::Rect AutofillPopupControllerImpl::GetRowBounds(size_t index) {
  int top = kPopupBorderThickness;
  for (size_t i = 0; i < index; ++i) {
    top += GetRowHeightFromId(identifiers()[i]);
  }

  return gfx::Rect(
      kPopupBorderThickness,
      top,
      popup_bounds_.width() - 2 * kPopupBorderThickness,
      GetRowHeightFromId(identifiers()[index]));
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

const std::vector<base::string16>& AutofillPopupControllerImpl::names() const {
  return names_;
}

const std::vector<base::string16>& AutofillPopupControllerImpl::subtexts()
    const {
  return subtexts_;
}

const std::vector<base::string16>& AutofillPopupControllerImpl::icons() const {
  return icons_;
}

const std::vector<int>& AutofillPopupControllerImpl::identifiers() const {
  return identifiers_;
}

#if !defined(OS_ANDROID)
const gfx::FontList& AutofillPopupControllerImpl::GetNameFontListForRow(
    size_t index) const {
  if (identifiers_[index] == POPUP_ITEM_ID_WARNING_MESSAGE)
    return warning_font_list_;

  return name_font_list_;
}

const gfx::FontList& AutofillPopupControllerImpl::subtext_font_list() const {
  return subtext_font_list_;
}
#endif

int AutofillPopupControllerImpl::selected_line() const {
  return selected_line_;
}

void AutofillPopupControllerImpl::set_hide_on_outside_click(
    bool hide_on_outside_click) {
  hide_on_outside_click_ = hide_on_outside_click;
}

void AutofillPopupControllerImpl::SetSelectedLine(int selected_line) {
  if (selected_line_ == selected_line)
    return;

  if (selected_line_ != kNoSelection &&
      static_cast<size_t>(selected_line_) < identifiers_.size())
    InvalidateRow(selected_line_);

  if (selected_line != kNoSelection)
    InvalidateRow(selected_line);

  selected_line_ = selected_line;

  if (selected_line_ != kNoSelection) {
    delegate_->DidSelectSuggestion(names_[selected_line_],
                                   identifiers_[selected_line_]);
  } else {
    delegate_->ClearPreviewedForm();
  }
}

void AutofillPopupControllerImpl::SelectNextLine() {
  int new_selected_line = selected_line_ + 1;

  // Skip over any lines that can't be selected.
  while (static_cast<size_t>(new_selected_line) < names_.size() &&
         !CanAccept(identifiers()[new_selected_line])) {
    ++new_selected_line;
  }

  if (new_selected_line >= static_cast<int>(names_.size()))
    new_selected_line = 0;

  SetSelectedLine(new_selected_line);
}

void AutofillPopupControllerImpl::SelectPreviousLine() {
  int new_selected_line = selected_line_ - 1;

  // Skip over any lines that can't be selected.
  while (new_selected_line > kNoSelection &&
         !CanAccept(identifiers()[new_selected_line])) {
    --new_selected_line;
  }

  if (new_selected_line <= kNoSelection)
    new_selected_line = names_.size() - 1;

  SetSelectedLine(new_selected_line);
}

bool AutofillPopupControllerImpl::RemoveSelectedLine() {
  if (selected_line_ == kNoSelection)
    return false;

  DCHECK_GE(selected_line_, 0);
  DCHECK_LT(selected_line_, static_cast<int>(names_.size()));

  if (!CanDelete(selected_line_))
    return false;

  delegate_->RemoveSuggestion(full_names_[selected_line_],
                              identifiers_[selected_line_]);

  // Remove the deleted element.
  names_.erase(names_.begin() + selected_line_);
  full_names_.erase(full_names_.begin() + selected_line_);
  subtexts_.erase(subtexts_.begin() + selected_line_);
  icons_.erase(icons_.begin() + selected_line_);
  identifiers_.erase(identifiers_.begin() + selected_line_);

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

  for (size_t i = 0; i < identifiers().size(); ++i) {
    current_height += GetRowHeightFromId(identifiers()[i]);

    if (y <= current_height)
      return i;
  }

  // The y value goes beyond the popup so stop the selection at the last line.
  return identifiers().size() - 1;
}

int AutofillPopupControllerImpl::GetRowHeightFromId(int identifier) const {
  if (identifier == POPUP_ITEM_ID_SEPARATOR)
    return kSeparatorHeight;

  return kRowHeight;
}

bool AutofillPopupControllerImpl::CanAccept(int id) {
  return id != POPUP_ITEM_ID_SEPARATOR && id != POPUP_ITEM_ID_WARNING_MESSAGE;
}

bool AutofillPopupControllerImpl::HasSuggestions() {
  return identifiers_.size() != 0 &&
         (identifiers_[0] > 0 ||
          identifiers_[0] == POPUP_ITEM_ID_AUTOCOMPLETE_ENTRY ||
          identifiers_[0] == POPUP_ITEM_ID_PASSWORD_ENTRY ||
          identifiers_[0] == POPUP_ITEM_ID_DATALIST_ENTRY);
}

void AutofillPopupControllerImpl::SetValues(
    const std::vector<base::string16>& names,
    const std::vector<base::string16>& subtexts,
    const std::vector<base::string16>& icons,
    const std::vector<int>& identifiers) {
  names_ = names;
  full_names_ = names;
  subtexts_ = subtexts;
  icons_ = icons;
  identifiers_ = identifiers;
}

void AutofillPopupControllerImpl::ShowView() {
  view_->Show();
}

void AutofillPopupControllerImpl::InvalidateRow(size_t row) {
  DCHECK(0 <= row);
  DCHECK(row < identifiers_.size());
  view_->InvalidateRow(row);
}

#if !defined(OS_ANDROID)
int AutofillPopupControllerImpl::GetDesiredPopupWidth() const {
  int popup_width = controller_common_->RoundedElementBounds().width();
  DCHECK_EQ(names().size(), subtexts().size());
  for (size_t i = 0; i < names().size(); ++i) {
    int row_size =
        gfx::GetStringWidth(names()[i], name_font_list_) +
        gfx::GetStringWidth(subtexts()[i], subtext_font_list_) +
        RowWidthWithoutText(i);

    popup_width = std::max(popup_width, row_size);
  }

  return popup_width;
}

int AutofillPopupControllerImpl::GetDesiredPopupHeight() const {
  int popup_height = 2 * kPopupBorderThickness;

  for (size_t i = 0; i < identifiers().size(); ++i) {
    popup_height += GetRowHeightFromId(identifiers()[i]);
  }

  return popup_height;
}

int AutofillPopupControllerImpl::RowWidthWithoutText(int row) const {
  int row_size = kEndPadding;

  if (!subtexts_[row].empty())
    row_size += kNamePadding;

  // Add the Autofill icon size, if required.
  if (!icons_[row].empty()) {
    int icon_width = ui::ResourceBundle::GetSharedInstance().GetImageNamed(
        GetIconResourceID(icons_[row])).Width();
    row_size += icon_width + kIconPadding;
  }

  // Add the padding at the end.
  row_size += kEndPadding;

  // Add room for the popup border.
  row_size += 2 * kPopupBorderThickness;

  return row_size;
}

void AutofillPopupControllerImpl::UpdatePopupBounds() {
  int popup_required_width = GetDesiredPopupWidth();
  int popup_height = GetDesiredPopupHeight();

  popup_bounds_ = controller_common_->GetPopupBounds(popup_height,
                                                     popup_required_width);
}
#endif  // !defined(OS_ANDROID)

WeakPtr<AutofillPopupControllerImpl> AutofillPopupControllerImpl::GetWeakPtr() {
  return weak_ptr_factory_.GetWeakPtr();
}

void AutofillPopupControllerImpl::ClearState() {
  // Don't clear view_, because otherwise the popup will have to get regenerated
  // and this will cause flickering.

  popup_bounds_ = gfx::Rect();

  names_.clear();
  subtexts_.clear();
  icons_.clear();
  identifiers_.clear();
  full_names_.clear();

  selected_line_ = kNoSelection;
}

}  // namespace autofill
