// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/autofill/tab_autofill_manager_delegate.h"

#include "base/logging.h"
#include "base/prefs/pref_service.h"
#include "chrome/browser/ui/autofill/autofill_popup_controller_impl.h"
#include "components/autofill/content/browser/content_autofill_driver.h"
#include "components/autofill/content/common/autofill_messages.h"
#include "components/autofill/core/common/autofill_pref_names.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents_view.h"
#include "content/nw/src/shell_browser_context.h"
#include "content/nw/src/browser/nw_form_database_service.h"
#include "ui/gfx/rect.h"

#if defined(OS_ANDROID)
#include "chrome/browser/ui/android/autofill/autofill_logger_android.h"
#endif

DEFINE_WEB_CONTENTS_USER_DATA_KEY(autofill::TabAutofillManagerDelegate);

namespace autofill {

TabAutofillManagerDelegate::TabAutofillManagerDelegate(
    content::WebContents* web_contents)
    : content::WebContentsObserver(web_contents),
      web_contents_(web_contents) {
  DCHECK(web_contents);
}

TabAutofillManagerDelegate::~TabAutofillManagerDelegate() {
  // NOTE: It is too late to clean up the autofill popup; that cleanup process
  // requires that the WebContents instance still be valid and it is not at
  // this point (in particular, the WebContentsImpl destructor has already
  // finished running and we are now in the base class destructor).
  DCHECK(!popup_controller_);
}

void TabAutofillManagerDelegate::TabActivated() {
}

PersonalDataManager* TabAutofillManagerDelegate::GetPersonalDataManager() {
  return NULL;
}

scoped_refptr<AutofillWebDataService>
    TabAutofillManagerDelegate::GetDatabase() {
  nw::NwFormDatabaseService* service =
      static_cast<content::ShellBrowserContext*>(
          web_contents_->GetBrowserContext())->GetFormDatabaseService();
  return service->get_autofill_webdata_service();
}

PrefService* TabAutofillManagerDelegate::GetPrefs() {
  return NULL;
}

void TabAutofillManagerDelegate::ShowAutofillSettings() {
  NOTIMPLEMENTED();
}

void TabAutofillManagerDelegate::ConfirmSaveCreditCard(
    const AutofillMetrics& metric_logger,
    const base::Closure& save_card_callback) {
  NOTIMPLEMENTED();
}

void TabAutofillManagerDelegate::ShowRequestAutocompleteDialog(
    const FormData& form,
    const GURL& source_url,
    const base::Callback<void(const FormStructure*)>& callback) {
  NOTIMPLEMENTED();
}

void TabAutofillManagerDelegate::ShowAutofillPopup(
    const gfx::RectF& element_bounds,
    base::i18n::TextDirection text_direction,
    const std::vector<base::string16>& values,
    const std::vector<base::string16>& labels,
    const std::vector<base::string16>& icons,
    const std::vector<int>& identifiers,
    base::WeakPtr<AutofillPopupDelegate> delegate) {
  // Convert element_bounds to be in screen space.
  gfx::Rect client_area;
  web_contents_->GetView()->GetContainerBounds(&client_area);
  gfx::RectF element_bounds_in_screen_space =
      element_bounds + client_area.OffsetFromOrigin();

  // Will delete or reuse the old |popup_controller_|.
  popup_controller_ = AutofillPopupControllerImpl::GetOrCreate(
      popup_controller_,
      delegate,
      web_contents(),
      web_contents()->GetView()->GetNativeView(),
      element_bounds_in_screen_space,
      text_direction);

  popup_controller_->Show(values, labels, icons, identifiers);
}

void TabAutofillManagerDelegate::UpdateAutofillPopupDataListValues(
    const std::vector<base::string16>& values,
    const std::vector<base::string16>& labels) {
  if (popup_controller_.get())
    popup_controller_->UpdateDataListValues(values, labels);
}

void TabAutofillManagerDelegate::HideAutofillPopup() {
  if (popup_controller_.get())
    popup_controller_->Hide();

}

bool TabAutofillManagerDelegate::IsAutocompleteEnabled() {
  return true;
}

void TabAutofillManagerDelegate::HideRequestAutocompleteDialog() {
  NOTIMPLEMENTED();
}

void TabAutofillManagerDelegate::WebContentsDestroyed(
    content::WebContents* web_contents) {
  HideAutofillPopup();
}

void TabAutofillManagerDelegate::DetectAccountCreationForms(
    const std::vector<autofill::FormStructure*>& forms) {
  NOTIMPLEMENTED();
}

void TabAutofillManagerDelegate::DidFillOrPreviewField(
    const base::string16& autofilled_value,
    const base::string16& profile_full_name) {
}

}  // namespace autofill
