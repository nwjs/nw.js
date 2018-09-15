// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/policy_cert_verifier.h"

#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "chrome/browser/browser_process.h"
#include "content/public/browser/browser_thread.h"
#include "net/base/net_errors.h"
#include "net/cert/caching_cert_verifier.h"
#include "net/cert/cert_verify_proc.h"
#include "net/cert/multi_threaded_cert_verifier.h"

namespace nw {

namespace {

void MaybeSignalAnchorUse(int error,
                          const base::Closure& anchor_used_callback,
                          const net::CertVerifyResult& verify_result) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::IO);
  if (error != net::OK || !verify_result.is_issued_by_additional_trust_anchor ||
      anchor_used_callback.is_null()) {
    return;
  }
  anchor_used_callback.Run();
}

void CompleteAndSignalAnchorUse(
    const base::Closure& anchor_used_callback,
    net::CompletionOnceCallback completion_callback,
    const net::CertVerifyResult* verify_result,
    int error) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::IO);
  MaybeSignalAnchorUse(error, anchor_used_callback, *verify_result);
  std::move(completion_callback).Run(error);
}

net::CertVerifier::Config ExtendTrustAnchors(
    const net::CertVerifier::Config& config,
    const net::CertificateList& trust_anchors) {
  net::CertVerifier::Config new_config = config;
  new_config.additional_trust_anchors.insert(
      new_config.additional_trust_anchors.begin(), trust_anchors.begin(),
      trust_anchors.end());
  return new_config;
}

}  // namespace

PolicyCertVerifier::PolicyCertVerifier(
    const base::Closure& anchor_used_callback)
    : anchor_used_callback_(anchor_used_callback) {
  //DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
}

PolicyCertVerifier::~PolicyCertVerifier() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::IO);
}

void PolicyCertVerifier::InitializeOnIOThread(
    const scoped_refptr<net::CertVerifyProc>& verify_proc) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::IO);
  if (!verify_proc->SupportsAdditionalTrustAnchors()) {
    LOG(WARNING)
        << "Additional trust anchors not supported on the current platform!";
  }
  delegate_ = std::make_unique<net::CachingCertVerifier>(
      std::make_unique<net::MultiThreadedCertVerifier>(verify_proc.get()));
  delegate_->SetConfig(ExtendTrustAnchors(orig_config_, trust_anchors_));
}

void PolicyCertVerifier::SetTrustAnchors(
    const net::CertificateList& trust_anchors) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::IO);
  if (trust_anchors == trust_anchors_)
    return;
  trust_anchors_ = trust_anchors;
  if (!delegate_)
    return;
  delegate_->SetConfig(ExtendTrustAnchors(orig_config_, trust_anchors_));
}

int PolicyCertVerifier::Verify(
    const RequestParams& params,
    net::CertVerifyResult* verify_result,
    net::CompletionOnceCallback completion_callback,
    std::unique_ptr<Request>* out_req,
    const net::NetLogWithSource& net_log) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::IO);
  DCHECK(delegate_);
  net::CompletionOnceCallback wrapped_callback =
      base::BindOnce(&CompleteAndSignalAnchorUse,
                 anchor_used_callback_,
                 std::move(completion_callback),
                 verify_result);

  int error = delegate_->Verify(params, verify_result,
                                std::move(wrapped_callback), out_req, net_log);
  MaybeSignalAnchorUse(error, anchor_used_callback_, *verify_result);
  return error;
}

void PolicyCertVerifier::SetConfig(const Config& config) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::IO);
  orig_config_ = config;
  delegate_->SetConfig(ExtendTrustAnchors(orig_config_, trust_anchors_));
}
}  // namespace policy
