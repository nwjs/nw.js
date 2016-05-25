// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/policy_cert_verifier.h"

#include "base/logging.h"
#include "chrome/browser/browser_process.h"
#include "content/public/browser/browser_thread.h"
#include "net/base/net_errors.h"
#include "net/cert/cert_verify_proc.h"
#include "net/cert/multi_threaded_cert_verifier.h"

namespace nw {

PolicyCertVerifier::PolicyCertVerifier() {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::IO));
  InitializeOnIOThread(net::CertVerifyProc::CreateDefault());
}

PolicyCertVerifier::~PolicyCertVerifier() {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::IO));
}

void PolicyCertVerifier::InitializeOnIOThread(
    const scoped_refptr<net::CertVerifyProc>& verify_proc) {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::IO));
  if (!verify_proc->SupportsAdditionalTrustAnchors()) {
    LOG(WARNING)
        << "Additional trust anchors not supported on the current platform!";
  }
  net::MultiThreadedCertVerifier* verifier =
      new net::MultiThreadedCertVerifier(verify_proc.get());
  verifier->SetCertTrustAnchorProvider(this);
  delegate_.reset(verifier);
}

void PolicyCertVerifier::SetTrustAnchors(
    const net::CertificateList& trust_anchors) {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::IO));
  trust_anchors_ = trust_anchors;
}

int PolicyCertVerifier::Verify(
    net::X509Certificate* cert,
    const std::string& hostname,
    const std::string& ocsp_response,
    int flags,
    net::CRLSet* crl_set,
    net::CertVerifyResult* verify_result,
    const net::CompletionCallback& completion_callback,
    std::unique_ptr<Request>* out_req,
    const net::BoundNetLog& net_log) {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::IO));
  DCHECK(delegate_);
  int error =
      delegate_->Verify(cert, hostname, ocsp_response, flags, crl_set,
                        verify_result, completion_callback, out_req, net_log);
  return error;
}

const net::CertificateList& PolicyCertVerifier::GetAdditionalTrustAnchors() {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::IO));
  return trust_anchors_;
}

}  // namespace nw
