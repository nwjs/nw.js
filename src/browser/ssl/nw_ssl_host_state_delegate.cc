// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/ssl/nw_ssl_host_state_delegate.h"

#include <set>

#include "base/base64.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/guid.h"
#include "base/logging.h"
#include "base/prefs/pref_filter.h"
#include "base/strings/string_number_conversions.h"
#include "base/values.h"
#include "chrome/common/chrome_switches.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/content_settings/core/common/content_settings_types.h"
#include "components/variations/variations_associated_data.h"
#include "content/public/browser/browser_thread.h"
#include "net/base/hash_value.h"
#include "net/cert/x509_certificate.h"
#include "net/http/http_transaction_factory.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_getter.h"
#include "url/gurl.h"

namespace {

// Keys for the per-site error + certificate finger to judgment content
// settings map.
const char kSSLCertDecisionsKey[] = "ssl_cert_decisions";
const char kSSLCertDecisionCertErrorMapKey[] = "cert_exceptions_map";
const char kSSLCertDecisionVersionKey[] = "version";

const int kDefaultSSLCertDecisionVersion = 1;

// All SSL decisions are per host (and are shared arcoss schemes), so this
// canonicalizes all hosts into a secure scheme GURL to use with content
// settings. The returned GURL will be the passed in host with an empty path and
// https:// as the scheme.
GURL GetSecureGURLForHost(const std::string& host) {
  std::string url = "https://" + host;
  return GURL(url);
}

std::string GetKey(const net::X509Certificate& cert, net::CertStatus error) {
  // Since a security decision will be made based on the fingerprint, Chrome
  // should use the SHA-256 fingerprint for the certificate.
  net::SHA256HashValue fingerprint =
      net::X509Certificate::CalculateChainFingerprint256(
          cert.os_cert_handle(), cert.GetIntermediateCertificates());
  std::string base64_fingerprint;
  base::Base64Encode(
      base::StringPiece(reinterpret_cast<const char*>(fingerprint.data),
                        sizeof(fingerprint.data)),
      &base64_fingerprint);
  return base::UintToString(error) + base64_fingerprint;
}

}  // namespace

// This helper function gets the dictionary of certificate fingerprints to
// errors of certificates that have been accepted by the user from the content
// dictionary that has been passed in. The returned pointer is owned by the the
// argument dict that is passed in.
//
// If create_entries is set to |DO_NOT_CREATE_DICTIONARY_ENTRIES|,
// GetValidCertDecisionsDict will return NULL if there is anything invalid about
// the setting, such as an invalid version or invalid value types (in addition
// to there not being any values in the dictionary). If create_entries is set to
// |CREATE_DICTIONARY_ENTRIES|, if no dictionary is found or the decisions are
// expired, a new dictionary will be created.
base::DictionaryValue* NWSSLHostStateDelegate::GetValidCertDecisionsDict(
    base::DictionaryValue* dict,
    CreateDictionaryEntriesDisposition create_entries) {

  // Extract the version of the certificate decision structure from the content
  // setting.
  int version;
  bool success = dict->GetInteger(kSSLCertDecisionVersionKey, &version);
  if (!success) {
    if (create_entries == DO_NOT_CREATE_DICTIONARY_ENTRIES)
      return NULL;

    dict->SetInteger(kSSLCertDecisionVersionKey,
                     kDefaultSSLCertDecisionVersion);
    version = kDefaultSSLCertDecisionVersion;
  }

  // If the version is somehow a newer version than Chrome can handle, there's
  // really nothing to do other than fail silently and pretend it doesn't exist
  // (or is malformed).
  if (version > kDefaultSSLCertDecisionVersion) {
    LOG(ERROR) << "Failed to parse a certificate error exception that is in a "
               << "newer version format (" << version << ") than is supported ("
               << kDefaultSSLCertDecisionVersion << ")";
    return NULL;
  }

  // Extract the map of certificate fingerprints to errors from the setting.
  base::DictionaryValue* cert_error_dict = NULL;  // Will be owned by dict
  if (!dict->GetDictionary(kSSLCertDecisionCertErrorMapKey, &cert_error_dict)) {
    if (create_entries == DO_NOT_CREATE_DICTIONARY_ENTRIES)
      return NULL;

    cert_error_dict = new base::DictionaryValue();
    // dict takes ownership of cert_error_dict
    dict->Set(kSSLCertDecisionCertErrorMapKey, cert_error_dict);
  }

  return cert_error_dict;
}

NWSSLHostStateDelegate::NWSSLHostStateDelegate(
    content::BrowserContext *browser_context) {
  base::FilePath path = browser_context->GetPath().Append("HostContentSettings");

  scoped_refptr<base::SequencedTaskRunner> sequenced_task_runner =
      JsonPrefStore::GetTaskRunnerForFile(path,
                                          content::BrowserThread::GetBlockingPool());

    ssl_prefs_store_ = new JsonPrefStore(
        path,
        sequenced_task_runner,
        NULL
      );

    //bad bad bad I think? What thread are we going to block?
    //It will block a lot as it's using the blocking IO pool.
    ssl_prefs_store_->ReadPrefs();
}

NWSSLHostStateDelegate::~NWSSLHostStateDelegate() {
}

HostContentSettingsMap* GetHostContentSettingsMap() {
  return NULL;
}

void NWSSLHostStateDelegate::AllowCert(const std::string& host,
                                       const net::X509Certificate& cert,
                                       net::CertStatus error) {
  GURL url = GetSecureGURLForHost(host);

  LOG(ERROR) << "url.GetOrigin(): " << url.GetOrigin();

  const base::Value *valuePtr;
  ssl_prefs_store_->GetValue(kSSLCertDecisionsKey, &valuePtr);

  scoped_ptr<base::Value> value;

  if (!value.get() || !value->IsType(base::Value::TYPE_DICTIONARY)) {
    value.reset(new base::DictionaryValue());
  } else {
    value.reset(valuePtr->DeepCopy());
  }

  base::DictionaryValue* dict;
  bool success = value->GetAsDictionary(&dict);
  DCHECK(success);


  //^ var value = file["ssl_cert_decisions"] || {};

  base::DictionaryValue* entry;

  if(!dict->GetDictionaryWithoutPathExpansion(url.GetOrigin().spec(), &entry)) {
    entry = new base::DictionaryValue();
  }

  //^ var entry = value[hostname] || {};

  base::DictionaryValue* cert_dict = GetValidCertDecisionsDict(
      entry,
      CREATE_DICTIONARY_ENTRIES);
  // If a a valid certificate dictionary cannot be extracted from the content
  // setting, that means it's in an unknown format. Unfortunately, there's
  // nothing to be done in that case, so a silent fail is the only option.
  if (!cert_dict)
    return;

  entry->SetIntegerWithoutPathExpansion(kSSLCertDecisionVersionKey,
                                       kDefaultSSLCertDecisionVersion);
  cert_dict->SetIntegerWithoutPathExpansion(GetKey(cert, error), ALLOWED);

  // The map takes ownership of the value, so it is released in the call to
  // SetWebsiteSetting.
  //map->SetWebsiteSetting(pattern, value.release());

  dict->SetWithoutPathExpansion(url.spec(), entry);

  ssl_prefs_store_->SetValue(kSSLCertDecisionsKey, value.release());

  ssl_prefs_store_->CommitPendingWrite();

  //^ file["ssl_cert_decisions"] = value;
}

void NWSSLHostStateDelegate::Clear() {

}

content::SSLHostStateDelegate::CertJudgment
NWSSLHostStateDelegate::QueryPolicy(const std::string& host,
                                    const net::X509Certificate& cert,
                                    net::CertStatus error,
                                    bool* expired_previous_decision) {
  return DENIED;

  GURL url = GetSecureGURLForHost(host);

  const base::Value *valuePtr;

  ssl_prefs_store_->GetValue(url.spec(), &valuePtr);

  scoped_ptr<base::Value> value(valuePtr->DeepCopy());

  if (!value.get() || !value->IsType(base::Value::TYPE_DICTIONARY))
    return DENIED;

  base::DictionaryValue* dict;  // Owned by value
  int policy_decision;
  bool success = value->GetAsDictionary(&dict);
  DCHECK(success);

  base::DictionaryValue* cert_error_dict;  // Owned by value
  cert_error_dict = GetValidCertDecisionsDict(
      dict,
      DO_NOT_CREATE_DICTIONARY_ENTRIES);
  if (!cert_error_dict) {
    return DENIED;
  }

  success = cert_error_dict->GetIntegerWithoutPathExpansion(GetKey(cert, error),
                                                            &policy_decision);

  // If a policy decision was successfully retrieved and it's a valid value of
  // ALLOWED, return the valid value. Otherwise, return DENIED.
  if (success && policy_decision == ALLOWED)
    return ALLOWED;

  return DENIED;
}

void NWSSLHostStateDelegate::HostRanInsecureContent(const std::string& host,
                                                        int pid) {
  ran_insecure_content_hosts_.insert(BrokenHostEntry(host, pid));
}

bool NWSSLHostStateDelegate::DidHostRunInsecureContent(
    const std::string& host,
    int pid) const {
  return !!ran_insecure_content_hosts_.count(BrokenHostEntry(host, pid));
}
