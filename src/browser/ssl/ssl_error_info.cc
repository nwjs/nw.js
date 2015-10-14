// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/ssl/ssl_error_info.h"

#include "base/i18n/time_formatting.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "grit/nw_resources.h"
#include "grit/nw_strings.h"
#include "content/public/browser/cert_store.h"
#include "net/base/escape.h"
#include "net/base/net_errors.h"
#include "net/cert/cert_status_flags.h"
#include "net/ssl/ssl_info.h"
#include "ui/base/l10n/l10n_util.h"
#include "url/gurl.h"

using base::UTF8ToUTF16;

SSLErrorInfo::SSLErrorInfo(const base::string16& details,
                           const base::string16& short_description)
    : details_(details),
      short_description_(short_description) {
}

// static
SSLErrorInfo SSLErrorInfo::CreateError(ErrorType error_type,
                                       net::X509Certificate* cert,
                                       const GURL& request_url) {
  base::string16 details, short_description;
  switch (error_type) {
    case CERT_COMMON_NAME_INVALID: {
      // If the certificate contains multiple DNS names, we choose the most
      // representative one -- either the DNS name that's also in the subject
      // field, or the first one.  If this heuristic turns out to be
      // inadequate, we can consider choosing the DNS name that is the
      // "closest match" to the host name in the request URL, or listing all
      // the DNS names with an HTML <ul>.
      std::vector<std::string> dns_names;
      cert->GetDNSNames(&dns_names);
      DCHECK(!dns_names.empty());
      size_t i = 0;
      for (; i < dns_names.size(); ++i) {
        if (dns_names[i] == cert->subject().common_name)
          break;
      }
      if (i == dns_names.size())
        i = 0;
      details =
          l10n_util::GetStringFUTF16(IDS_CERT_ERROR_COMMON_NAME_INVALID_DETAILS,
                                     UTF8ToUTF16(request_url.host()),
                                     net::EscapeForHTML(
                                         UTF8ToUTF16(dns_names[i])));
      short_description = l10n_util::GetStringUTF16(
          IDS_CERT_ERROR_COMMON_NAME_INVALID_DESCRIPTION);
      break;
    }
    case CERT_DATE_INVALID:
      if (cert->HasExpired()) {
        details = l10n_util::GetStringFUTF16(
            IDS_CERT_ERROR_EXPIRED_DETAILS,
            UTF8ToUTF16(request_url.host()),
            base::IntToString16(
                (base::Time::Now() - cert->valid_expiry()).InDays()),
            base::TimeFormatFriendlyDate(base::Time::Now()));
        short_description =
            l10n_util::GetStringUTF16(IDS_CERT_ERROR_EXPIRED_DESCRIPTION);
      } else if (base::Time::Now() < cert->valid_start()) {
        details = l10n_util::GetStringFUTF16(
            IDS_CERT_ERROR_NOT_YET_VALID_DETAILS,
            UTF8ToUTF16(request_url.host()),
            base::IntToString16(
                (cert->valid_start() - base::Time::Now()).InDays()));
        short_description =
            l10n_util::GetStringUTF16(IDS_CERT_ERROR_NOT_YET_VALID_DESCRIPTION);
      } else {
        // Two possibilities: (1) an intermediate or root certificate has
        // expired, or (2) the certificate has become valid since the error
        // occurred. Since (1) is more likely, assume that's the case.
        details = l10n_util::GetStringFUTF16(
            IDS_CERT_ERROR_CHAIN_EXPIRED_DETAILS,
            UTF8ToUTF16(request_url.host()),
            base::TimeFormatFriendlyDate(base::Time::Now()));
        short_description =
            l10n_util::GetStringUTF16(IDS_CERT_ERROR_CHAIN_EXPIRED_DESCRIPTION);
      }
      break;
    case CERT_AUTHORITY_INVALID:
      details = l10n_util::GetStringFUTF16(
          IDS_CERT_ERROR_AUTHORITY_INVALID_DETAILS,
          UTF8ToUTF16(request_url.host()));
      short_description = l10n_util::GetStringUTF16(
          IDS_CERT_ERROR_AUTHORITY_INVALID_DESCRIPTION);
      break;
    case CERT_CONTAINS_ERRORS:
      details = l10n_util::GetStringFUTF16(
          IDS_CERT_ERROR_CONTAINS_ERRORS_DETAILS,
          UTF8ToUTF16(request_url.host()));
      short_description =
          l10n_util::GetStringUTF16(IDS_CERT_ERROR_CONTAINS_ERRORS_DESCRIPTION);
      break;
    case CERT_NO_REVOCATION_MECHANISM:
      details = l10n_util::GetStringUTF16(
          IDS_CERT_ERROR_NO_REVOCATION_MECHANISM_DETAILS);
      short_description = l10n_util::GetStringUTF16(
          IDS_CERT_ERROR_NO_REVOCATION_MECHANISM_DESCRIPTION);
      break;
    case CERT_REVOKED:
      details = l10n_util::GetStringFUTF16(IDS_CERT_ERROR_REVOKED_CERT_DETAILS,
                                           UTF8ToUTF16(request_url.host()));
      short_description =
          l10n_util::GetStringUTF16(IDS_CERT_ERROR_REVOKED_CERT_DESCRIPTION);
      break;
    case CERT_INVALID:
      details = l10n_util::GetStringFUTF16(
          IDS_CERT_ERROR_INVALID_CERT_DETAILS,
          UTF8ToUTF16(request_url.host()));
      short_description =
          l10n_util::GetStringUTF16(IDS_CERT_ERROR_INVALID_CERT_DESCRIPTION);
      break;
    case CERT_WEAK_SIGNATURE_ALGORITHM:
      details = l10n_util::GetStringFUTF16(
          IDS_CERT_ERROR_WEAK_SIGNATURE_ALGORITHM_DETAILS,
          UTF8ToUTF16(request_url.host()));
      short_description = l10n_util::GetStringUTF16(
          IDS_CERT_ERROR_WEAK_SIGNATURE_ALGORITHM_DESCRIPTION);
      break;
    case CERT_WEAK_KEY:
      details = l10n_util::GetStringFUTF16(
          IDS_CERT_ERROR_WEAK_KEY_DETAILS, UTF8ToUTF16(request_url.host()));
      short_description = l10n_util::GetStringUTF16(
          IDS_CERT_ERROR_WEAK_KEY_DESCRIPTION);
      break;
    case CERT_WEAK_KEY_DH:
      details = l10n_util::GetStringFUTF16(
          IDS_CERT_ERROR_WEAK_KEY_DETAILS, UTF8ToUTF16(request_url.host()));
      short_description = l10n_util::GetStringUTF16(
          IDS_CERT_ERROR_WEAK_KEY_DESCRIPTION);
    case CERT_NAME_CONSTRAINT_VIOLATION:
      details = l10n_util::GetStringFUTF16(
          IDS_CERT_ERROR_NAME_CONSTRAINT_VIOLATION_DETAILS,
          UTF8ToUTF16(request_url.host()));
      short_description = l10n_util::GetStringUTF16(
          IDS_CERT_ERROR_NAME_CONSTRAINT_VIOLATION_DESCRIPTION);
      break;
    case CERT_PINNED_KEY_MISSING:
      details = l10n_util::GetStringUTF16(
          IDS_ERRORPAGES_SUMMARY_PINNING_FAILURE);
      short_description = l10n_util::GetStringUTF16(
          IDS_ERRORPAGES_DETAILS_PINNING_FAILURE);
      break;
    case UNKNOWN:
      details = l10n_util::GetStringUTF16(IDS_CERT_ERROR_UNKNOWN_ERROR_DETAILS);
      short_description =
          l10n_util::GetStringUTF16(IDS_CERT_ERROR_UNKNOWN_ERROR_DESCRIPTION);
      break;
    case CERT_UNABLE_TO_CHECK_REVOCATION:  // Deprecated.
    default:
      NOTREACHED();
  }
  return SSLErrorInfo(details, short_description);
}

SSLErrorInfo::~SSLErrorInfo() {
}

// static
SSLErrorInfo::ErrorType SSLErrorInfo::NetErrorToErrorType(int net_error) {
  switch (net_error) {
    case net::ERR_CERT_COMMON_NAME_INVALID:
      return CERT_COMMON_NAME_INVALID;
    case net::ERR_CERT_DATE_INVALID:
      return CERT_DATE_INVALID;
    case net::ERR_CERT_AUTHORITY_INVALID:
      return CERT_AUTHORITY_INVALID;
    case net::ERR_CERT_CONTAINS_ERRORS:
      return CERT_CONTAINS_ERRORS;
    case net::ERR_CERT_NO_REVOCATION_MECHANISM:
      return CERT_NO_REVOCATION_MECHANISM;
    case net::ERR_CERT_UNABLE_TO_CHECK_REVOCATION:
      return CERT_UNABLE_TO_CHECK_REVOCATION;
    case net::ERR_CERT_REVOKED:
      return CERT_REVOKED;
    case net::ERR_CERT_INVALID:
      return CERT_INVALID;
    case net::ERR_CERT_WEAK_SIGNATURE_ALGORITHM:
      return CERT_WEAK_SIGNATURE_ALGORITHM;
    case net::ERR_CERT_WEAK_KEY:
      return CERT_WEAK_KEY;
    case net::ERR_CERT_NAME_CONSTRAINT_VIOLATION:
      return CERT_NAME_CONSTRAINT_VIOLATION;
    case net::ERR_SSL_WEAK_SERVER_EPHEMERAL_DH_KEY:
      return CERT_WEAK_KEY_DH;
    case net::ERR_SSL_PINNED_KEY_NOT_IN_CERT_CHAIN:
      return CERT_PINNED_KEY_MISSING;
    default:
      NOTREACHED();
      return UNKNOWN;
    }
}

// static
int SSLErrorInfo::GetErrorsForCertStatus(int cert_id,
                                         net::CertStatus cert_status,
                                         const GURL& url,
                                         std::vector<SSLErrorInfo>* errors) {
  const net::CertStatus kErrorFlags[] = {
    net::CERT_STATUS_COMMON_NAME_INVALID,
    net::CERT_STATUS_DATE_INVALID,
    net::CERT_STATUS_AUTHORITY_INVALID,
    net::CERT_STATUS_NO_REVOCATION_MECHANISM,
    net::CERT_STATUS_UNABLE_TO_CHECK_REVOCATION,
    net::CERT_STATUS_REVOKED,
    net::CERT_STATUS_INVALID,
    net::CERT_STATUS_WEAK_SIGNATURE_ALGORITHM,
    net::CERT_STATUS_WEAK_KEY,
    net::CERT_STATUS_NAME_CONSTRAINT_VIOLATION,
  };

  const ErrorType kErrorTypes[] = {
    CERT_COMMON_NAME_INVALID,
    CERT_DATE_INVALID,
    CERT_AUTHORITY_INVALID,
    CERT_NO_REVOCATION_MECHANISM,
    CERT_UNABLE_TO_CHECK_REVOCATION,
    CERT_REVOKED,
    CERT_INVALID,
    CERT_WEAK_SIGNATURE_ALGORITHM,
    CERT_WEAK_KEY,
    CERT_NAME_CONSTRAINT_VIOLATION,
  };
  DCHECK(arraysize(kErrorFlags) == arraysize(kErrorTypes));

  scoped_refptr<net::X509Certificate> cert = NULL;
  int count = 0;
  for (size_t i = 0; i < arraysize(kErrorFlags); ++i) {
    if (cert_status & kErrorFlags[i]) {
      count++;
      if (!cert.get()) {
        bool r = content::CertStore::GetInstance()->RetrieveCert(
            cert_id, &cert);
        DCHECK(r);
      }
      if (errors)
        errors->push_back(
            SSLErrorInfo::CreateError(kErrorTypes[i], cert.get(), url));
    }
  }
  return count;
}
