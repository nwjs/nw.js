// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/nw_autofill_client.h"

#include "base/logging.h"
#include "base/prefs/pref_service.h"
#include "components/autofill/content/browser/content_autofill_driver.h"
#include "components/autofill/content/common/autofill_messages.h"
#include "components/autofill/core/common/autofill_pref_names.h"
#include "content/nw/src/browser/autofill_popup_controller_impl.h"
#include "content/nw/src/browser/nw_form_database_service.h"
#include "content/nw/src/shell_browser_context.h"
#include "content/public/browser/render_view_host.h"
#include "ui/gfx/geometry/rect.h"

#if defined(OS_ANDROID)
#include "chrome/browser/ui/android/autofill/autofill_logger_android.h"
#endif

DEFINE_WEB_CONTENTS_USER_DATA_KEY(autofill::NWAutofillClient);

namespace autofill {

NWAutofillClient::NWAutofillClient(content::WebContents* web_contents)
    : content::WebContentsObserver(web_contents), web_contents_(web_contents) {
  DCHECK(web_contents);
#if defined(OS_MAC) && !defined(OS_IOS)
  RegisterForKeystoneNotifications();
#endif  // defined(OS_MAC) && !defined(OS_IOS)
}

NWAutofillClient::~NWAutofillClient() {
  // NOTE: It is too late to clean up the autofill popup; that cleanup process
  // requires that the WebContents instance still be valid and it is not at
  // this point (in particular, the WebContentsImpl destructor has already
  // finished running and we are now in the base class destructor).
  DCHECK(!popup_controller_);
#if defined(OS_MAC) && !defined(OS_IOS)
  UnregisterFromKeystoneNotifications();
#endif  // defined(OS_MAC) && !defined(OS_IOS)
}

void NWAutofillClient::TabActivated() {
}

PersonalDataManager* NWAutofillClient::GetPersonalDataManager() {
  return NULL;
}

scoped_refptr<AutofillWebDataService> NWAutofillClient::GetDatabase() {
  nw::NwFormDatabaseService* service =
      static_cast<content::ShellBrowserContext*>(
          web_contents_->GetBrowserContext())->GetFormDatabaseService();
  return service->get_autofill_webdata_service();
}

PrefService* NWAutofillClient::GetPrefs() {
  return NULL;
}

void NWAutofillClient::ShowAutofillSettings() {
  NOTIMPLEMENTED();
}

void NWAutofillClient::ConfirmSaveCreditCard(
    const base::Closure& save_card_callback) {
  NOTIMPLEMENTED();
}

void NWAutofillClient::ShowRequestAutocompleteDialog(
    const FormData& form,
    content::RenderFrameHost* render_frame_host,
    const ResultCallback& callback) {
  NOTIMPLEMENTED();
}

void NWAutofillClient::ShowAutofillPopup(
    const gfx::RectF& element_bounds,
    base::i18n::TextDirection text_direction,
    const std::vector<autofill::Suggestion>& suggestions,
    base::WeakPtr<AutofillPopupDelegate> delegate) {
  // Convert element_bounds to be in screen space.
  gfx::Rect client_area = web_contents_->GetContainerBounds();
  gfx::RectF element_bounds_in_screen_space =
      element_bounds + client_area.OffsetFromOrigin();

  // Will delete or reuse the old |popup_controller_|.
  popup_controller_ =
      AutofillPopupControllerImpl::GetOrCreate(popup_controller_,
                                               delegate,
                                               web_contents(),
                                               web_contents()->GetNativeView(),
                                               element_bounds_in_screen_space,
                                               text_direction);

  popup_controller_->Show(suggestions);
}

void NWAutofillClient::UpdateAutofillPopupDataListValues(
    const std::vector<base::string16>& values,
    const std::vector<base::string16>& labels) {
  if (popup_controller_.get())
    popup_controller_->UpdateDataListValues(values, labels);
}

void NWAutofillClient::HideAutofillPopup() {
  if (popup_controller_.get())
    popup_controller_->Hide();
}

bool NWAutofillClient::IsAutocompleteEnabled() {
  return true;
}

void NWAutofillClient::HideRequestAutocompleteDialog() {
  NOTIMPLEMENTED();
}

void NWAutofillClient::WebContentsDestroyed() {
  HideAutofillPopup();
}

void NWAutofillClient::DetectAccountCreationForms(
      content::RenderFrameHost* rfh,
      const std::vector<autofill::FormStructure*>& forms) {
  NOTIMPLEMENTED();
}

void NWAutofillClient::DidFillOrPreviewField(
    const base::string16& autofilled_value,
    const base::string16& profile_full_name) {
}

bool NWAutofillClient::HasCreditCardScanFeature() {
  return false;
}

void NWAutofillClient::ScanCreditCard(const CreditCardScanCallback& callback) {
  NOTIMPLEMENTED();
}

void NWAutofillClient::ShowUnmaskPrompt(
    const autofill::CreditCard& card,
    base::WeakPtr<autofill::CardUnmaskDelegate> delegate) {
  NOTIMPLEMENTED();
}

void NWAutofillClient::OnUnmaskVerificationResult(bool success) {
  NOTIMPLEMENTED();
}

void NWAutofillClient::OnFirstUserGestureObserved() {
  //NOTIMPLEMENTED();
}

}  // namespace autofill
