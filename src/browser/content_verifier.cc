// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/content_verifier.h"

#include <algorithm>

#include "base/files/file_path.h"
#include "base/stl_util.h"
#include "base/strings/string_util.h"
#include "content/public/browser/browser_thread.h"
#include "content/nw/src/browser/content_hash_fetcher.h"
#include "content/nw/src/browser/content_hash_reader.h"
#include "content/nw/src/browser/content_verifier_delegate.h"
#include "content/nw/src/browser/content_verifier_io_data.h"
//#include "content/nw/src/browser/extension_registry.h"
//#include "extensions/common/constants.h"
//#include "extensions/common/extension_l10n_util.h"

#include "base/files/file_util.h"
#include "base/threading/thread_restrictions.h"

namespace nw {

ContentVerifier::ContentVerifier(content::BrowserContext* context,
                                 ContentVerifierDelegate* delegate,
                                 const base::FilePath& package_path)
    : shutdown_(false),
      context_(context),
      delegate_(delegate),
      fetcher_(new ContentHashFetcher(
          context,
          delegate,
          base::Bind(&ContentVerifier::OnFetchComplete, this))),
      io_data_(new ContentVerifierIOData),
      package_path_(package_path)
{
}

ContentVerifier::~ContentVerifier() {
}

void ContentVerifier::Start() {
}

void ContentVerifier::Shutdown() {
  shutdown_ = true;
  content::BrowserThread::PostTask(
      content::BrowserThread::IO,
      FROM_HERE,
      base::Bind(&ContentVerifierIOData::Clear, io_data_));
  fetcher_.reset();
}

ContentVerifyJob* ContentVerifier::CreateJobFor(
    const std::string& extension_id,
    const base::FilePath& extension_root,
    const base::FilePath& relative_path) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::IO);

  const ContentVerifierIOData::ExtensionData* data =
      io_data_->GetData(extension_id);
  if (!data)
    return NULL;

  std::set<base::FilePath> paths;
  paths.insert(relative_path);
  if (!ShouldVerifyAnyPaths(extension_id, extension_root, paths))
    return NULL;

  // TODO(asargent) - we can probably get some good performance wins by having
  // a cache of ContentHashReader's that we hold onto past the end of each job.
  return new ContentVerifyJob(
      new ContentHashReader(extension_id,
                            extension_root,
                            relative_path,
                            delegate_->PublicKey()),
      base::Bind(&ContentVerifier::VerifyFailed, this, extension_id, relative_path),
      base::Bind(&ContentVerifier::OnHashReady, this, extension_id, extension_root, relative_path));
}

void ContentVerifier::OnHashReady(const std::string& extension_id,
                                  const base::FilePath& extension_root,
                                  const base::FilePath& relative_path,
                                  ContentVerifyJob* verify_job) {
  content::BrowserThread::GetBlockingPool()->PostTaskAndReply(
      FROM_HERE,
      base::Bind(&ContentVerifier::OpenFile, this, extension_root, relative_path, make_scoped_refptr(verify_job)),
      base::Bind(&ContentVerifier::OnFileReady, this, extension_root, relative_path, make_scoped_refptr(verify_job)));
}

void ContentVerifier::OpenFile(const base::FilePath& extension_root,
                               const base::FilePath& relative_path,
                               ContentVerifyJob* job) {
  job->file_.Initialize(extension_root.Append(relative_path), base::File::FLAG_OPEN | base::File::FLAG_READ);
}

void ContentVerifier::OnFileReady(const base::FilePath& extension_root,
                                  const base::FilePath& relative_path,
                                  ContentVerifyJob* job) {
  if (!job->file_.IsValid())
    job->DoneReading();

  content::BrowserThread::GetBlockingPool()->PostTaskAndReply(
      FROM_HERE,
      base::Bind(&ContentVerifier::ReadFile, this, extension_root, relative_path, make_scoped_refptr(job)),
      base::Bind(&ContentVerifier::BytesRead, this, extension_root, relative_path, make_scoped_refptr(job)));
}

void ContentVerifier::ReadFile(const base::FilePath& extension_root,
                               const base::FilePath& relative_path,
                               ContentVerifyJob* job) {
  job->len_ = job->file_.ReadAtCurrentPos(job->buf_, 32768);
  if (job->len_ <= 0)
    job->file_.Close();
}

void ContentVerifier::BytesRead(const base::FilePath& extension_root,
                                const base::FilePath& relative_path,
                                ContentVerifyJob* job) {
  if (job->len_ <= 0) {
    job->DoneReading();
  } else {
    job->BytesRead(job->len_, job->buf_);
    content::BrowserThread::GetBlockingPool()->PostTaskAndReply(
      FROM_HERE,
      base::Bind(&ContentVerifier::ReadFile, this, extension_root, relative_path, make_scoped_refptr(job)),
      base::Bind(&ContentVerifier::BytesRead, this, extension_root, relative_path, make_scoped_refptr(job)));
  }
}

void ContentVerifier::VerifyFailed(const std::string& extension_id,
                                   const base::FilePath& relative_path,
                                   ContentVerifyJob::FailureReason reason,
                                   ContentVerifyJob* verify_job) {
  if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    content::BrowserThread::PostTask(
        content::BrowserThread::UI,
        FROM_HERE,
        base::Bind(&ContentVerifier::VerifyFailed, this, extension_id, relative_path, reason, make_scoped_refptr(verify_job)));
    return;
  }
  if (shutdown_)
    return;

  VLOG(1) << "VerifyFailed " << extension_id << " reason:" << reason;

  ContentVerifierDelegate::Mode mode = delegate_->ShouldBeVerified();
  if (mode < ContentVerifierDelegate::ENFORCE) {
    if (!verify_job->success_callback().is_null())
      verify_job->success_callback().Run();
  }

  if (reason == ContentVerifyJob::MISSING_ALL_HASHES) {
    // If we failed because there were no hashes yet for this extension, just
    // request some.
    fetcher_->DoFetch(true /* force */);
  } else {
    delegate_->VerifyFailed(extension_id, relative_path, reason);
  }
}

static base::FilePath MakeImagePathRelative(const base::FilePath& path) {
  if (path.ReferencesParent())
    return base::FilePath();

  std::vector<base::FilePath::StringType> parts;
  path.GetComponents(&parts);
  if (parts.empty())
    return base::FilePath();

  // Remove the first component if it is '.' or '/' or '//'.
  const base::FilePath::StringType separators(
      base::FilePath::kSeparators, base::FilePath::kSeparatorsLength);
  if (!parts[0].empty() &&
      (parts[0] == base::FilePath::kCurrentDirectory ||
       parts[0].find_first_not_of(separators) == std::string::npos))
    parts.erase(parts.begin());

  // Note that elsewhere we always normalize path separators to '/' so this
  // should work for all platforms.
  return base::FilePath(JoinString(parts, '/'));
}

void ContentVerifier::OnExtensionLoaded(
    content::BrowserContext* browser_context) {
  if (shutdown_)
    return;

  ContentVerifierDelegate::Mode mode = delegate_->ShouldBeVerified();
  if (mode != ContentVerifierDelegate::NONE) {
    // The browser image paths from the extension may not be relative (eg
    // they might have leading '/' or './'), so we strip those to make
    // comparing to actual relative paths work later on.
    std::set<base::FilePath> original_image_paths =
        delegate_->GetBrowserImagePaths();

    scoped_ptr<std::set<base::FilePath>> image_paths(
        new std::set<base::FilePath>);
    for (const auto& path : original_image_paths) {
      image_paths->insert(MakeImagePathRelative(path));
    }

    scoped_ptr<ContentVerifierIOData::ExtensionData> data(
        new ContentVerifierIOData::ExtensionData(
            image_paths.Pass(),
            base::Version()));
    content::BrowserThread::PostTask(content::BrowserThread::IO,
                                     FROM_HERE,
                                     base::Bind(&ContentVerifierIOData::AddData,
                                                io_data_,
                                                "",
                                                base::Passed(&data)));
    fetcher_->ExtensionLoaded();
  }
}

void ContentVerifier::OnExtensionUnloaded(content::BrowserContext* browser_context) {
  if (shutdown_)
    return;
  content::BrowserThread::PostTask(
      content::BrowserThread::IO,
      FROM_HERE,
      base::Bind(
          &ContentVerifierIOData::RemoveData, io_data_, ""));
  if (fetcher_)
    fetcher_->ExtensionUnloaded();
}

void ContentVerifier::OnFetchCompleteHelper(const std::string& extension_id,
                                            bool shouldVerifyAnyPathsResult) {
  if (shouldVerifyAnyPathsResult)
    delegate_->VerifyFailed(extension_id, base::FilePath(), ContentVerifyJob::MISSING_ALL_HASHES);
}

void ContentVerifier::OnFetchComplete(
    bool success,
    bool was_force_check,
    const std::set<base::FilePath>& hash_mismatch_paths) {
  if (shutdown_)
    return;

  VLOG(1) << "OnFetchComplete " << " success:" << success;

  if (!delegate_)
    return;

  ContentVerifierDelegate::Mode mode = delegate_->ShouldBeVerified();
  if (was_force_check && !success &&
      mode == ContentVerifierDelegate::ENFORCE_STRICT) {
    // We weren't able to get verified_contents.json or weren't able to compute
    // hashes.
    delegate_->VerifyFailed("", base::FilePath(), ContentVerifyJob::MISSING_ALL_HASHES);
  } else {
    content::BrowserThread::PostTaskAndReplyWithResult(
        content::BrowserThread::IO,
        FROM_HERE,
        base::Bind(&ContentVerifier::ShouldVerifyAnyPaths,
                   this,
                   "",
                   package_path_,
                   hash_mismatch_paths),
        base::Bind(
            &ContentVerifier::OnFetchCompleteHelper, this, ""));
  }
}

bool ContentVerifier::ShouldVerifyAnyPaths(
    const std::string& extension_id,
    const base::FilePath& extension_root,
    const std::set<base::FilePath>& relative_paths) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::IO);
  const ContentVerifierIOData::ExtensionData* data =
      io_data_->GetData(extension_id);
  if (!data)
    return false;

  return relative_paths.size() > 0;
}

}  // namespace nw
