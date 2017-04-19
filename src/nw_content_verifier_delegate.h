// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NW_CONTENT_VERIFIER_DELEGATE_H_
#define NW_CONTENT_VERIFIER_DELEGATE_H_

#include "extensions/browser/content_verifier_delegate.h"

namespace content {
class BrowserContext;
}

namespace extensions {

class NWContentVerifierDelegate : public ContentVerifierDelegate {
 public:
  static Mode GetDefaultMode();

  explicit NWContentVerifierDelegate(content::BrowserContext* context);

  ~NWContentVerifierDelegate() override;

  // ContentVerifierDelegate:
  Mode ShouldBeVerified(const Extension& extension) override;
  ContentVerifierKey GetPublicKey() override;
  GURL GetSignatureFetchUrl(const std::string& extension_id,
                            const base::Version& version) override;
  std::set<base::FilePath> GetBrowserImagePaths(
      const extensions::Extension* extension) override;
  void VerifyFailed(const std::string& extension_id,
                    const base::FilePath& relative_path,
                    ContentVerifyJob::FailureReason reason) override;
  void Shutdown() override;
  content::BrowserContext* context_;
  ContentVerifierDelegate::Mode default_mode_;

  std::set<std::string> would_be_disabled_ids_;

  DISALLOW_COPY_AND_ASSIGN(NWContentVerifierDelegate);
};

}  // namespace extensions

#endif  // NW_CONTENT_VERIFIER_DELEGATE_H_
