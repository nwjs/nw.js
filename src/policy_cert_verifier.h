// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NW_POLICY_POLICY_CERT_VERIFIER_H_
#define NW_POLICY_POLICY_CERT_VERIFIER_H_

#include <vector>

#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "net/base/completion_callback.h"
#include "net/cert/cert_trust_anchor_provider.h"
#include "net/cert/cert_verifier.h"

namespace net {
class CertVerifyProc;
class CertVerifyResult;
class X509Certificate;
typedef std::vector<scoped_refptr<X509Certificate> > CertificateList;
}

namespace nw {

// Wraps a MultiThreadedCertVerifier to make it use the additional trust anchors
// configured by the ONC user policy.
class PolicyCertVerifier : public net::CertVerifier,
                           public net::CertTrustAnchorProvider {
 public:
  // Except for tests, PolicyCertVerifier should only be created by
  // PolicyCertService, which is the counterpart of this class on the UI thread.
  // Except of the constructor, all methods and the destructor must be called on
  // the IO thread. Calls |anchor_used_callback| on the IO thread everytime a
  // certificate from the additional trust anchors (set with SetTrustAnchors) is
  // used.
  explicit PolicyCertVerifier();
  ~PolicyCertVerifier() override;

  void InitializeOnIOThread(
      const scoped_refptr<net::CertVerifyProc>& verify_proc);

  // Sets the additional trust anchors.
  void SetTrustAnchors(const net::CertificateList& trust_anchors);

  // CertVerifier:
  // Note: |callback| can be null.
  int Verify(net::X509Certificate* cert,
             const std::string& hostname,
             const std::string& ocsp_response,
             int flags,
             net::CRLSet* crl_set,
             net::CertVerifyResult* verify_result,
             const net::CompletionCallback& callback,
             scoped_ptr<Request>* out_req,
             const net::BoundNetLog& net_log) override;

  // CertTrustAnchorProvider:
  const net::CertificateList& GetAdditionalTrustAnchors() override;

 private:
  net::CertificateList trust_anchors_;
  scoped_ptr<CertVerifier> delegate_;

  DISALLOW_COPY_AND_ASSIGN(PolicyCertVerifier);
};

}  // namespace nw

#endif  // NW_POLICY_POLICY_CERT_VERIFIER_H_
