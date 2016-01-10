// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/content_verify_job.h"

#include <algorithm>

#include "base/metrics/histogram.h"
#include "base/stl_util.h"
#include "base/task_runner_util.h"
#include "base/timer/elapsed_timer.h"
#include "content/public/browser/browser_thread.h"
#include "crypto/secure_hash.h"
#include "crypto/sha2.h"
#include "content/nw/src/browser/content_hash_reader.h"

namespace nw {

namespace {

ContentVerifyJob::TestDelegate* g_test_delegate = NULL;
ContentVerifyJob::TestObserver* g_test_observer = NULL;

class ScopedElapsedTimer {
 public:
  ScopedElapsedTimer(base::TimeDelta* total) : total_(total) { DCHECK(total_); }

  ~ScopedElapsedTimer() { *total_ += timer.Elapsed(); }

 private:
  // Some total amount of time we should add our elapsed time to at
  // destruction.
  base::TimeDelta* total_;

  // A timer for how long this object has been alive.
  base::ElapsedTimer timer;
};

}  // namespace

ContentVerifyJob::ContentVerifyJob(ContentHashReader* hash_reader,
                                   const FailureCallback& failure_callback)
    : done_reading_(false),
      hashes_ready_(false),
      total_bytes_read_(0),
      current_block_(0),
      current_hash_byte_count_(0),
      hash_reader_(hash_reader),
      failure_callback_(failure_callback),
      failed_(false) {
  // It's ok for this object to be constructed on a different thread from where
  // it's used.
  thread_checker_.DetachFromThread();
}

ContentVerifyJob::ContentVerifyJob(ContentHashReader* hash_reader,
                                   const FailureCallback& failure_callback,
                                   const ReadyCallback& ready_callback)
    : done_reading_(false),
      hashes_ready_(false),
      total_bytes_read_(0),
      current_block_(0),
      current_hash_byte_count_(0),
      hash_reader_(hash_reader),
      failure_callback_(failure_callback),
      ready_callback_(ready_callback),
      failed_(false) {
  // It's ok for this object to be constructed on a different thread from where
  // it's used.
  thread_checker_.DetachFromThread();
}


ContentVerifyJob::~ContentVerifyJob() {
  UMA_HISTOGRAM_COUNTS("ExtensionContentVerifyJob.TimeSpentUS",
                       time_spent_.InMicroseconds());
}

void ContentVerifyJob::Start() {
  DCHECK(thread_checker_.CalledOnValidThread());
  if (g_test_observer)
    g_test_observer->JobStarted(hash_reader_->extension_id(),
                                hash_reader_->relative_path());
  base::PostTaskAndReplyWithResult(
      content::BrowserThread::GetBlockingPool(),
      FROM_HERE,
      base::Bind(&ContentHashReader::Init, hash_reader_),
      base::Bind(&ContentVerifyJob::OnHashesReady, this));
}

void ContentVerifyJob::BytesRead(int count, const char* data) {
  ScopedElapsedTimer timer(&time_spent_);
  DCHECK(thread_checker_.CalledOnValidThread());
  if (failed_)
    return;
  if (g_test_delegate) {
    FailureReason reason =
        g_test_delegate->BytesRead(hash_reader_->extension_id(), count, data);
    if (reason != NONE)
      return DispatchFailureCallback(reason);
  }
  if (!hashes_ready_) {
    queue_.append(data, count);
    return;
  }
  DCHECK_GE(count, 0);
  int bytes_added = 0;

  while (bytes_added < count) {
    if (current_block_ >= hash_reader_->block_count())
      return DispatchFailureCallback(HASH_MISMATCH);

    if (!current_hash_.get()) {
      current_hash_byte_count_ = 0;
      current_hash_.reset(
          crypto::SecureHash::Create(crypto::SecureHash::SHA256));
    }
    // Compute how many bytes we should hash, and add them to the current hash.
    int bytes_to_hash =
        std::min(hash_reader_->block_size() - current_hash_byte_count_,
                 count - bytes_added);
    DCHECK(bytes_to_hash > 0);
    current_hash_->Update(data + bytes_added, bytes_to_hash);
    bytes_added += bytes_to_hash;
    current_hash_byte_count_ += bytes_to_hash;
    total_bytes_read_ += bytes_to_hash;

    // If we finished reading a block worth of data, finish computing the hash
    // for it and make sure the expected hash matches.
    if (current_hash_byte_count_ == hash_reader_->block_size() &&
        !FinishBlock()) {
      DispatchFailureCallback(HASH_MISMATCH);
      return;
    }
  }
}

void ContentVerifyJob::DoneReading() {
  ScopedElapsedTimer timer(&time_spent_);
  DCHECK(thread_checker_.CalledOnValidThread());
  if (failed_)
    return;
  if (g_test_delegate) {
    FailureReason reason =
        g_test_delegate->DoneReading(hash_reader_->extension_id());
    if (reason != NONE) {
      DispatchFailureCallback(reason);
      return;
    }
  }
  done_reading_ = true;
  if (hashes_ready_ && !FinishBlock())
    DispatchFailureCallback(HASH_MISMATCH);
  else if (hashes_ready_ && !success_callback_.is_null())
    success_callback_.Run();

  if (!failed_ && g_test_observer)
    g_test_observer->JobFinished(
        hash_reader_->extension_id(), hash_reader_->relative_path(), failed_);
}

bool ContentVerifyJob::FinishBlock() {
  if (current_hash_byte_count_ <= 0)
    return true;
  std::string final(crypto::kSHA256Length, 0);
  current_hash_->Finish(string_as_array(&final), final.size());
  current_hash_.reset();
  current_hash_byte_count_ = 0;

  int block = current_block_++;

  const std::string* expected_hash = NULL;
  if (!hash_reader_->GetHashForBlock(block, &expected_hash) ||
      *expected_hash != final)
    return false;

  return true;
}

void ContentVerifyJob::OnHashesReady(bool success) {
  if (!success && !g_test_delegate) {
    if (!hash_reader_->content_exists()) {
      // Ignore verification of non-existent resources.
      if (!success_callback_.is_null())
        success_callback_.Run();
      return;
    } else if (hash_reader_->have_verified_contents() &&
               hash_reader_->have_computed_hashes()) {
      DispatchFailureCallback(NO_HASHES_FOR_FILE);
    } else {
      DispatchFailureCallback(MISSING_ALL_HASHES);
    }
    return;
  }

  hashes_ready_ = true;
  if (!queue_.empty()) {
    std::string tmp;
    queue_.swap(tmp);
    BytesRead(tmp.size(), string_as_array(&tmp));
  }
  if (done_reading_) {
    ScopedElapsedTimer timer(&time_spent_);
    if (!FinishBlock())
      DispatchFailureCallback(HASH_MISMATCH);
  }
  if (!ready_callback_.is_null()) {
    ready_callback_.Run(this);
  }
}

// static
void ContentVerifyJob::SetDelegateForTests(TestDelegate* delegate) {
  g_test_delegate = delegate;
}

// static
void ContentVerifyJob::SetObserverForTests(TestObserver* observer) {
  g_test_observer = observer;
}

void ContentVerifyJob::DispatchFailureCallback(FailureReason reason) {
  DCHECK(!failed_);
  failed_ = true;
  if (!failure_callback_.is_null()) {
    VLOG(1) << "job failed for " << hash_reader_->extension_id() << " "
            << hash_reader_->relative_path().MaybeAsASCII()
            << " reason:" << reason;
    failure_callback_.Run(reason, this);
    failure_callback_.Reset();
  }
  if (g_test_observer)
    g_test_observer->JobFinished(
        hash_reader_->extension_id(), hash_reader_->relative_path(), failed_);
}

}  // namespace nw
