// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/printing/print_job_manager.h"

#include "content/nw/src/browser/printing/print_job.h"
#include "content/nw/src/browser/printing/printer_query.h"
#include "content/public/browser/notification_types.h"
#include "content/public/browser/notification_service.h"
#include "printing/printed_document.h"
#include "printing/printed_page.h"

namespace printing {

PrintJobManager::PrintJobManager() {
  registrar_.Add(this, content::NOTIFICATION_PRINT_JOB_EVENT,
                 content::NotificationService::AllSources());
}

PrintJobManager::~PrintJobManager() {
  base::AutoLock lock(lock_);
  queued_queries_.clear();
}

void PrintJobManager::OnQuit() {
  StopJobs(true);
  registrar_.RemoveAll();
}

void PrintJobManager::StopJobs(bool wait_for_finish) {
  // Copy the array since it can be modified in transit.
  PrintJobs to_stop;
  to_stop.swap(current_jobs_);

  for (PrintJobs::const_iterator job = to_stop.begin(); job != to_stop.end();
       ++job) {
    // Wait for two minutes for the print job to be spooled.
    if (wait_for_finish)
      (*job)->FlushJob(base::TimeDelta::FromMinutes(2));
    (*job)->Stop();
  }
}

void PrintJobManager::SetPrintDestination(
    PrintDestinationInterface* destination) {
  destination_ = destination;
}

void PrintJobManager::QueuePrinterQuery(PrinterQuery* job) {
  base::AutoLock lock(lock_);
  DCHECK(job);
  queued_queries_.push_back(make_scoped_refptr(job));
  DCHECK(job->is_valid());
}

void PrintJobManager::PopPrinterQuery(int document_cookie,
                                      scoped_refptr<PrinterQuery>* job) {
  base::AutoLock lock(lock_);
  for (PrinterQueries::iterator itr = queued_queries_.begin();
       itr != queued_queries_.end();
       ++itr) {
    PrinterQuery* current_query = *itr;
    if (current_query->cookie() == document_cookie &&
        !current_query->is_callback_pending()) {
      *job = current_query;
      queued_queries_.erase(itr);
      DCHECK(current_query->is_valid());
      return;
    }
  }
}

void PrintJobManager::Observe(int type,
                              const content::NotificationSource& source,
                              const content::NotificationDetails& details) {
  switch (type) {
    case content::NOTIFICATION_PRINT_JOB_EVENT: {
      OnPrintJobEvent(content::Source<PrintJob>(source).ptr(),
                      *content::Details<JobEventDetails>(details).ptr());
      break;
    }
    default: {
      NOTREACHED();
      break;
    }
  }
}

void PrintJobManager::OnPrintJobEvent(
    PrintJob* print_job,
    const JobEventDetails& event_details) {
  switch (event_details.type()) {
    case JobEventDetails::NEW_DOC: {
      DCHECK(current_jobs_.end() == current_jobs_.find(print_job));
      // Causes a AddRef().
      current_jobs_.insert(print_job);
      break;
    }
    case JobEventDetails::JOB_DONE: {
      DCHECK(current_jobs_.end() != current_jobs_.find(print_job));
      current_jobs_.erase(print_job);
      break;
    }
    case JobEventDetails::FAILED: {
      current_jobs_.erase(print_job);
      break;
    }
    case JobEventDetails::USER_INIT_DONE:
    case JobEventDetails::USER_INIT_CANCELED:
    case JobEventDetails::DEFAULT_INIT_DONE:
    case JobEventDetails::NEW_PAGE:
    case JobEventDetails::PAGE_DONE:
    case JobEventDetails::DOC_DONE:
    case JobEventDetails::ALL_PAGES_REQUESTED: {
      // Don't care.
      break;
    }
    default: {
      NOTREACHED();
      break;
    }
  }
}

}  // namespace printing
