// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NW_BROWSER_CONTENT_VERIFIER_DELEGATE_H_
#define NW_BROWSER_CONTENT_VERIFIER_DELEGATE_H_

#include <set>

#include "content/nw/src/browser/content_verify_job.h"
#include "url/gurl.h"

namespace base {
class FilePath;
class Version;
}

namespace nw {

// A pointer to the bytes of a public key, and the number of bytes.
struct ContentVerifierKey {
  const uint8* data;
  int size;

  ContentVerifierKey() : data(NULL), size(0) {}

  ContentVerifierKey(const uint8* data, int size) {
    this->data = data;
    this->size = size;
  }
};

// This is an interface for clients that want to use a ContentVerifier.
class ContentVerifierDelegate {
 public:
  // Note that it is important for these to appear in increasing "severity"
  // order, because we use this to let command line flags increase, but not
  // decrease, the mode you're running in compared to the experiment group.
  enum Mode {
    // Do not try to fetch content hashes if they are missing, and do not
    // enforce them if they are present.
    NONE = 0,

    // If content hashes are missing, try to fetch them, but do not enforce.
    BOOTSTRAP,

    // If hashes are present, enforce them. If they are missing, try to fetch
    // them.
    ENFORCE,

    // Treat the absence of hashes the same as a verification failure.
    ENFORCE_STRICT
  };

  virtual ~ContentVerifierDelegate() {}

  // This should return what verification mode is appropriate for the given
  // extension, if any.
  virtual Mode ShouldBeVerified() = 0;

  // Should return the public key to use for validating signatures via the two
  // out parameters. NOTE: the pointer returned *must* remain valid for the
  // lifetime of this object.
  virtual const ContentVerifierKey& PublicKey() = 0;

  // This should return a URL that can be used to fetch the
  // verified_contents.json containing signatures for the given extension
  // id/version pair.
  virtual GURL GetSignatureFetchUrl() = 0;

  // This should return the set of file paths for images used within the
  // browser process. (These may get transcoded during the install process).
  virtual std::set<base::FilePath> GetBrowserImagePaths() = 0;

  // Called when the content verifier detects that a read of a file inside
  // an extension did not match its expected hash.
  virtual void VerifyFailed(const std::string& extension_id,
                            const base::FilePath& relative_path,
                            ContentVerifyJob::FailureReason reason) = 0;
};

}  // namespace nw

#endif  // NW_BROWSER_CONTENT_VERIFIER_DELEGATE_H_
