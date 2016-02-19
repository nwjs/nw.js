// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NW_BROWSER_SSL_CHROME_SSL_HOST_STATE_DELEGATE_H_
#define NW_BROWSER_SSL_CHROME_SSL_HOST_STATE_DELEGATE_H_

#include "base/gtest_prod_util.h"
#include "base/memory/scoped_ptr.h"
#include "base/prefs/json_pref_store.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/ssl_host_state_delegate.h"

namespace base {
class DictionaryValue;
}  //  namespace base

// Tracks whether the user has allowed a certificate error exception for a
// specific site, SSL fingerprint, and error. Based on command-line flags and
// experimental group, remembers this decision either until end-of-session or
// for a particular length of time.
class NWSSLHostStateDelegate : public content::SSLHostStateDelegate {
 public:
  explicit NWSSLHostStateDelegate(content::BrowserContext *browser_context);
  ~NWSSLHostStateDelegate() override;

  // SSLHostStateDelegate:
  void AllowCert(const std::string& host,
                 const net::X509Certificate& cert,
                 net::CertStatus error) override;
  void Clear() override;
  CertJudgment QueryPolicy(const std::string& host,
                           const net::X509Certificate& cert,
                           net::CertStatus error,
                           bool* expired_previous_decision) override;
  void HostRanInsecureContent(const std::string& host, int pid) override;
  bool DidHostRunInsecureContent(const std::string& host,
                                 int pid) const override;

 private:

  // Used to specify whether new content setting entries should be created if
  // they don't already exist when querying the user's settings.
  enum CreateDictionaryEntriesDisposition {
    CREATE_DICTIONARY_ENTRIES,
    DO_NOT_CREATE_DICTIONARY_ENTRIES
  };

  // Specifies whether user SSL error decisions should be forgetten at the end
  // of this current session (the old style of remembering decisions), or
  // whether they should be remembered across session restarts for a specified
  // length of time, deteremined by
  // |default_ssl_cert_decision_expiration_delta_|.
  enum RememberSSLExceptionDecisionsDisposition {
    FORGET_SSL_EXCEPTION_DECISIONS_AT_SESSION_END,
    REMEMBER_SSL_EXCEPTION_DECISIONS_FOR_DELTA
  };

  // Returns a dictionary of certificate fingerprints and errors that have been
  // allowed as exceptions by the user.
  //
  // |dict| specifies the user's full exceptions dictionary for a specific site
  // in their content settings. Must be retrieved directly from a website
  // setting in the the profile's HostContentSettingsMap.
  //
  // If |create_entries| specifies CreateDictionaryEntries, then
  // GetValidCertDecisionsDict will create a new set of entries within the
  // dictionary if they do not already exist. Otherwise will fail and return if
  // NULL if they do not exist.
  base::DictionaryValue* GetValidCertDecisionsDict(
      base::DictionaryValue* dict,
      CreateDictionaryEntriesDisposition create_entries);

  RememberSSLExceptionDecisionsDisposition should_remember_ssl_decisions_;


  // A BrokenHostEntry is a pair of (host, process_id) that indicates the host
  // contains insecure content in that renderer process.
  typedef std::pair<std::string, int> BrokenHostEntry;

  // Hosts which have been contaminated with insecure content in the
  // specified process.  Note that insecure content can travel between
  // same-origin frames in one processs but cannot jump between processes.
  std::set<BrokenHostEntry> ran_insecure_content_hosts_;

  JsonPrefStore *ssl_prefs_store_;

  DISALLOW_COPY_AND_ASSIGN(NWSSLHostStateDelegate);
};

#endif  // NW_BROWSER_SSL_CHROME_SSL_HOST_STATE_DELEGATE_H_
