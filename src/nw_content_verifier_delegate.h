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
  enum Mode {
    // Do not try to fetch content hashes if they are missing, and do
    // not
    // enforce them if they are present.
    NONE = 0,

    // If content hashes are missing, try to fetch them, but do not
    // enforce.
    BOOTSTRAP,

    // If hashes are present, enforce them. If they are missing, try
    // to fetch
    // them.
    ENFORCE,

    // Treat the absence of hashes the same as a verification failure.
        ENFORCE_STRICT
  };

  static Mode GetDefaultMode();

  explicit NWContentVerifierDelegate(content::BrowserContext* context);

  ~NWContentVerifierDelegate() override;

  // ContentVerifierDelegate:
  VerifierSourceType GetVerifierSourceType(const Extension& extension) override;
  ContentVerifierKey GetPublicKey() override;
  GURL GetSignatureFetchUrl(const std::string& extension_id,
                            const base::Version& version) override;
  std::set<base::FilePath> GetBrowserImagePaths(
      const extensions::Extension* extension) override;
  void VerifyFailed(const std::string& extension_id,
                    const base::FilePath& relative_path,
                    ContentVerifyJob::FailureReason reason,
                    scoped_refptr<ContentVerifyJob> verify_job) override;
  void Shutdown() override;
  content::BrowserContext* context_;
  Mode default_mode_;

  std::set<std::string> would_be_disabled_ids_;

  DISALLOW_COPY_AND_ASSIGN(NWContentVerifierDelegate);
};

}  // namespace extensions

#endif  // NW_CONTENT_VERIFIER_DELEGATE_H_
