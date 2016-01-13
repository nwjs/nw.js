// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NW_BROWSER_CONTENT_VERIFIER_H_
#define NW_BROWSER_CONTENT_VERIFIER_H_

#include <set>
#include <string>

#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/scoped_observer.h"
#include "base/version.h"
#include "base/files/file_path.h"
#include "content/nw/src/browser/content_verifier_delegate.h"
#include "content/nw/src/browser/content_verify_job.h"

namespace base {
class FilePath;
}

namespace content {
class BrowserContext;
}

namespace nw {

class ContentHashFetcher;
class ContentVerifierIOData;

// Used for managing overall content verification - both fetching content
// hashes as needed, and supplying job objects to verify file contents as they
// are read.
class ContentVerifier : public base::RefCountedThreadSafe<ContentVerifier> {
 public:
  // Takes ownership of |delegate|.
  ContentVerifier(content::BrowserContext* context,
                  ContentVerifierDelegate* delegate,
                  const base::FilePath& package_path);
  void Start();
  void Shutdown();

  // Call this before reading a file within an extension. The caller owns the
  // returned job.
  ContentVerifyJob* CreateJobFor(const std::string& extension_id,
                                 const base::FilePath& extension_root,
                                 const base::FilePath& relative_path);

  // Called (typically by a verification job) to indicate that verification
  // failed while reading some file in |extension_id|.
  void VerifyFailed(const std::string& extension_id,
                    const base::FilePath& relative_path,
                    ContentVerifyJob::FailureReason reason,
                    ContentVerifyJob* verify_job);
  void OnHashReady(const std::string& extension_id,
                   const base::FilePath& extension_root,
                   const base::FilePath& relative_path,
                   ContentVerifyJob* verify_job);
  void BytesRead(const base::FilePath& extension_root,
                 const base::FilePath& relative_path,
                 ContentVerifyJob* job);
  void ReadFile(const base::FilePath& extension_root,
                const base::FilePath& relative_path,
                ContentVerifyJob* job);
  void OpenFile(const base::FilePath& extension_root,
                const base::FilePath& relative_path,
                ContentVerifyJob* job);
  void OnFileReady(const base::FilePath& extension_root,
                   const base::FilePath& relative_path,
                   ContentVerifyJob* job);

  void OnExtensionLoaded(content::BrowserContext* browser_context);
  void OnExtensionUnloaded(content::BrowserContext* browser_context);

 private:
  DISALLOW_COPY_AND_ASSIGN(ContentVerifier);

  friend class base::RefCountedThreadSafe<ContentVerifier>;
  ~ContentVerifier();

  void OnFetchComplete(
                       bool success,
                       bool was_force_check,
                       const std::set<base::FilePath>& hash_mismatch_paths);

  void OnFetchCompleteHelper(const std::string& extension_id,
                             bool shouldVerifyAnyPathsResult);

  // Returns true if any of the paths in |relative_paths| *should* have their
  // contents verified. (Some files get transcoded during the install process,
  // so we don't want to verify their contents because they are expected not
  // to match).
  bool ShouldVerifyAnyPaths(const std::string& extension_id,
                            const base::FilePath& extension_root,
                            const std::set<base::FilePath>& relative_paths);

  // Set to true once we've begun shutting down.
  bool shutdown_;

  content::BrowserContext* context_;

  scoped_ptr<ContentVerifierDelegate> delegate_;

  // For fetching content hash signatures.
  scoped_ptr<ContentHashFetcher> fetcher_;

  // Data that should only be used on the IO thread.
  scoped_refptr<ContentVerifierIOData> io_data_;
  base::FilePath package_path_;
};

}  // namespace nw

#endif  // NW_BROWSER_CONTENT_VERIFIER_H_
