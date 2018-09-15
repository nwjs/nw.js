// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_CHROMEOS_POLICY_POLICY_CERT_VERIFIER_H_
#define CHROME_BROWSER_CHROMEOS_POLICY_POLICY_CERT_VERIFIER_H_

#include <memory>
#include <vector>

#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "net/base/completion_callback.h"
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
class PolicyCertVerifier : public net::CertVerifier {
 public:
  // Except for tests, PolicyCertVerifier should only be created by
  // PolicyCertService, which is the counterpart of this class on the UI thread.
  // Except of the constructor, all methods and the destructor must be called on
  // the IO thread. Calls |anchor_used_callback| on the IO thread everytime a
  // certificate from the additional trust anchors (set with SetTrustAnchors) is
  // used.
  explicit PolicyCertVerifier(const base::Closure& anchor_used_callback);
  ~PolicyCertVerifier() override;

  void InitializeOnIOThread(
      const scoped_refptr<net::CertVerifyProc>& verify_proc);

  // Sets the additional trust anchors.
  void SetTrustAnchors(const net::CertificateList& trust_anchors);

  // CertVerifier:
  // Note: |callback| can be null.
  int Verify(const RequestParams& params,
             net::CertVerifyResult* verify_result,
             net::CompletionOnceCallback callback,
             std::unique_ptr<Request>* out_req,
             const net::NetLogWithSource& net_log) override;
  void SetConfig(const Config& config) override;

 private:
  net::CertVerifier::Config orig_config_;
  net::CertificateList trust_anchors_;
  base::Closure anchor_used_callback_;
  std::unique_ptr<CertVerifier> delegate_;

  DISALLOW_COPY_AND_ASSIGN(PolicyCertVerifier);
};

}  // namespace policy

#endif  // CHROME_BROWSER_CHROMEOS_POLICY_POLICY_CERT_VERIFIER_H_
