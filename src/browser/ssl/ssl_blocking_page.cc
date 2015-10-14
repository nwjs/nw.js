// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/ssl/ssl_blocking_page.h"

#include "base/build_time.h"
#include "base/command_line.h"
#include "base/i18n/rtl.h"
#include "base/i18n/time_formatting.h"
#include "base/process/launch.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_piece.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/time/time.h"
#include "base/values.h"
#include "chrome/common/chrome_switches.h"
#include "components/google/core/browser/google_util.h"
#include "content/nw/src/browser/ssl/ssl_error_classification.h"
#include "content/nw/src/browser/ssl/ssl_error_info.h"
#include "content/public/browser/cert_store.h"
#include "content/public/browser/interstitial_page.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/ssl_status.h"
#include "grit/nw_resources.h"
#include "net/base/hash_value.h"
#include "net/base/net_errors.h"
#include "net/base/net_util.h"
#include "ui/base/l10n/l10n_util.h"

#if defined(OS_WIN)
#include "base/base_paths_win.h"
#include "base/path_service.h"
#include "base/strings/string16.h"
#include "base/win/windows_version.h"
#endif

using base::ASCIIToUTF16;
using base::TimeTicks;
using content::InterstitialPage;
using content::NavigationController;
using content::NavigationEntry;

namespace {

// URL for help page.
const char kHelpURL[] = "https://support.google.com/chrome/answer/4454607";

}  // namespace

const void* SSLBlockingPage::kTypeForTesting =
    &SSLBlockingPage::kTypeForTesting;

// Note that we always create a navigation entry with SSL errors.
// No error happening loading a sub-resource triggers an interstitial so far.
SSLBlockingPage::SSLBlockingPage(content::WebContents* web_contents,
                                 int cert_error,
                                 const net::SSLInfo& ssl_info,
                                 const GURL& request_url,
                                 int options_mask,
                                 const base::Callback<void(bool)>& callback)
    : SecurityInterstitialPage(web_contents, request_url),
      callback_(callback),
      cert_error_(cert_error),
      ssl_info_(ssl_info),
      overridable_(IsOptionsOverridable(options_mask)),
      danger_overridable_(true),
      strict_enforcement_((options_mask & STRICT_ENFORCEMENT) != 0),
      internal_(false),
      num_visits_(-1),
      expired_but_previously_allowed_(
          (options_mask & EXPIRED_BUT_PREVIOUSLY_ALLOWED) != 0) {

  ssl_error_classification_.reset(new SSLErrorClassification(
      web_contents,
      base::Time::NowFromSystemTime(),
      request_url,
      cert_error_,
      *ssl_info_.cert.get()));

  // Creating an interstitial without showing (e.g. from chrome://interstitials)
  // it leaks memory, so don't create it here.
}

bool SSLBlockingPage::ShouldCreateNewNavigation() const {
  return true;
}

const void* SSLBlockingPage::GetTypeForTesting() const {
  return SSLBlockingPage::kTypeForTesting;
}

SSLBlockingPage::~SSLBlockingPage() {
  if (!callback_.is_null()) {
    // The page is closed without the user having chosen what to do, default to
    // deny.
    NotifyDenyCertificate();
  }
}

void SSLBlockingPage::PopulateInterstitialStrings(
    base::DictionaryValue* load_time_data) {
  CHECK(load_time_data);
  base::string16 url(GetFormattedHostName());
  // Shared values for both the overridable and non-overridable versions.
  load_time_data->SetString("type", "SSL");

  // Shared UI configuration for all SSL interstitials.
  base::Time now = base::Time::NowFromSystemTime();

  load_time_data->SetString("errorCode", net::ErrorToString(cert_error_));
  load_time_data->SetString(
      "openDetails",
      l10n_util::GetStringUTF16(IDS_SSL_V2_OPEN_DETAILS_BUTTON));
  load_time_data->SetString(
      "closeDetails",
      l10n_util::GetStringUTF16(IDS_SSL_V2_CLOSE_DETAILS_BUTTON));

  load_time_data->SetBoolean("bad_clock", false);

  load_time_data->SetString(
      "tabTitle", l10n_util::GetStringUTF16(IDS_SSL_V2_TITLE));
  load_time_data->SetString(
      "heading", l10n_util::GetStringUTF16(IDS_SSL_V2_HEADING));
  load_time_data->SetString(
      "primaryParagraph",
      l10n_util::GetStringFUTF16(IDS_SSL_V2_PRIMARY_PARAGRAPH, url));

  if (overridable_) {
    load_time_data->SetBoolean("overridable", true);

    SSLErrorInfo error_info =
        SSLErrorInfo::CreateError(
            SSLErrorInfo::NetErrorToErrorType(cert_error_),
            ssl_info_.cert.get(),
            request_url());
    load_time_data->SetString("explanationParagraph", error_info.details());
    load_time_data->SetString(
        "primaryButtonText",
        l10n_util::GetStringUTF16(IDS_SSL_OVERRIDABLE_SAFETY_BUTTON));
    load_time_data->SetString(
        "finalParagraph",
        l10n_util::GetStringFUTF16(IDS_SSL_OVERRIDABLE_PROCEED_PARAGRAPH,
                                 url));
  } else {
    load_time_data->SetBoolean("overridable", false);

    SSLErrorInfo::ErrorType type =
        SSLErrorInfo::NetErrorToErrorType(cert_error_);
    if (type == SSLErrorInfo::CERT_INVALID && SSLErrorClassification::
        MaybeWindowsLacksSHA256Support()) {
      load_time_data->SetString(
          "explanationParagraph",
          l10n_util::GetStringFUTF16(
              IDS_SSL_NONOVERRIDABLE_MORE_INVALID_SP3, url));
    } else {
      load_time_data->SetString("explanationParagraph",
                               l10n_util::GetStringFUTF16(
                                   IDS_SSL_NONOVERRIDABLE_MORE, url));
    }
    load_time_data->SetString(
        "primaryButtonText",
        l10n_util::GetStringUTF16(IDS_SSL_RELOAD));
    // Customize the help link depending on the specific error type.
    // Only mark as HSTS if none of the more specific error types apply,
    // and use INVALID as a fallback if no other string is appropriate.
    load_time_data->SetInteger("errorType", type);
    int help_string = IDS_SSL_NONOVERRIDABLE_INVALID;
    switch (type) {
      case SSLErrorInfo::CERT_REVOKED:
        help_string = IDS_SSL_NONOVERRIDABLE_REVOKED;
        break;
      case SSLErrorInfo::CERT_PINNED_KEY_MISSING:
        help_string = IDS_SSL_NONOVERRIDABLE_PINNED;
        break;
      case SSLErrorInfo::CERT_INVALID:
        help_string = IDS_SSL_NONOVERRIDABLE_INVALID;
        break;
      default:
        if (strict_enforcement_)
          help_string = IDS_SSL_NONOVERRIDABLE_HSTS;
    }
    load_time_data->SetString(
        "finalParagraph", l10n_util::GetStringFUTF16(help_string, url));
  }

  // Set debugging information at the bottom of the warning.
  load_time_data->SetString(
      "subject", ssl_info_.cert->subject().GetDisplayName());
  load_time_data->SetString(
      "issuer", ssl_info_.cert->issuer().GetDisplayName());
  load_time_data->SetString(
      "expirationDate",
      base::TimeFormatShortDate(ssl_info_.cert->valid_expiry()));
  load_time_data->SetString(
      "currentDate", base::TimeFormatShortDate(now));
  std::vector<std::string> encoded_chain;
  ssl_info_.cert->GetPEMEncodedChain(&encoded_chain);
  load_time_data->SetString("pem", JoinString(encoded_chain, std::string()));
}

void SSLBlockingPage::OverrideEntry(NavigationEntry* entry) {
  int cert_id = content::CertStore::GetInstance()->StoreCert(
      ssl_info_.cert.get(), web_contents()->GetRenderProcessHost()->GetID());
  DCHECK(cert_id);

  entry->GetSSL().security_style =
      content::SECURITY_STYLE_AUTHENTICATION_BROKEN;
  entry->GetSSL().cert_id = cert_id;
  entry->GetSSL().cert_status = ssl_info_.cert_status;
  entry->GetSSL().security_bits = ssl_info_.security_bits;
}

// This handles the commands sent from the interstitial JavaScript. They are
// defined in chrome/browser/resources/ssl/ssl_errors_common.js.
// DO NOT reorder or change this logic without also changing the JavaScript!
void SSLBlockingPage::CommandReceived(const std::string& command) {
  int cmd = 0;
  bool retval = base::StringToInt(command, &cmd);
  DCHECK(retval);
  switch (cmd) {
    case CMD_DONT_PROCEED: {
      interstitial_page()->DontProceed();
      break;
    }
    case CMD_PROCEED: {
      if (danger_overridable_) {
        interstitial_page()->Proceed();
      }
      break;
    }
    case CMD_MORE: {
      break;
    }
    case CMD_RELOAD: {
      // The interstitial can't refresh itself.
      web_contents()->GetController().Reload(true);
      break;
    }
    case CMD_HELP: {
      // content::NavigationController::LoadURLParams help_page_params(
      //     google_util::AppendGoogleLocaleParam(
      //         GURL(kHelpURL), g_browser_process->GetApplicationLocale()));
      // web_contents()->GetController().LoadURLWithParams(help_page_params);
      break;
    }
    case CMD_CLOCK: {
      break;
    }
    default: {
      NOTREACHED();
    }
  }
}

void SSLBlockingPage::OverrideRendererPrefs(
      content::RendererPreferences* prefs) {
}

void SSLBlockingPage::OnProceed() {
  // Accepting the certificate resumes the loading of the page.
  NotifyAllowCertificate();
}

void SSLBlockingPage::OnDontProceed() {
  NotifyDenyCertificate();
}

void SSLBlockingPage::NotifyDenyCertificate() {
  // It's possible that callback_ may not exist if the user clicks "Proceed"
  // followed by pressing the back button before the interstitial is hidden.
  // In that case the certificate will still be treated as allowed.
  if (callback_.is_null())
    return;

  callback_.Run(false);
  callback_.Reset();
}

void SSLBlockingPage::NotifyAllowCertificate() {
  DCHECK(!callback_.is_null());

  callback_.Run(true);
  callback_.Reset();
}

// static
void SSLBlockingPage::SetExtraInfo(
    base::DictionaryValue* strings,
    const std::vector<base::string16>& extra_info) {
  DCHECK_LT(extra_info.size(), 5U);  // We allow 5 paragraphs max.
  const char* keys[5] = {
      "moreInfo1", "moreInfo2", "moreInfo3", "moreInfo4", "moreInfo5"
  };
  int i;
  for (i = 0; i < static_cast<int>(extra_info.size()); i++) {
    strings->SetString(keys[i], extra_info[i]);
  }
  for (; i < 5; i++) {
    strings->SetString(keys[i], std::string());
  }
}

// static
bool SSLBlockingPage::IsOptionsOverridable(int options_mask) {
  return (options_mask & SSLBlockingPage::OVERRIDABLE) &&
         !(options_mask & SSLBlockingPage::STRICT_ENFORCEMENT);
}

void SSLBlockingPage::OnGotHistoryCount(bool success,
                                        int num_visits,
                                        base::Time first_visit) {
  num_visits_ = num_visits;
}
