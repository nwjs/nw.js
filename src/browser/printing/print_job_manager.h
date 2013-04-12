// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_PRINTING_PRINT_JOB_MANAGER_H_
#define CHROME_BROWSER_PRINTING_PRINT_JOB_MANAGER_H_

#include <set>
#include <vector>

#include "base/memory/ref_counted.h"
#include "base/synchronization/lock.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/notification_registrar.h"
#include "printing/print_destination_interface.h"

namespace printing {

class JobEventDetails;
class PrintJob;
class PrinterQuery;

class PrintJobManager : public content::NotificationObserver {
 public:
  PrintJobManager();
  virtual ~PrintJobManager();

  // On browser quit, we should wait to have the print job finished.
  void OnQuit();

  // Stops all printing jobs. If wait_for_finish is true, tries to give jobs
  // a chance to complete before stopping them.
  void StopJobs(bool wait_for_finish);

  // Sets the print destination to be set on the next print job.
  void SetPrintDestination(PrintDestinationInterface* destination);

  // Queues a semi-initialized worker thread. Can be called from any thread.
  // Current use case is queuing from the I/O thread.
  // TODO(maruel):  Have them vanish after a timeout (~5 minutes?)
  void QueuePrinterQuery(PrinterQuery* job);

  // Pops a queued PrintJobWorkerOwner object that was previously queued. Can be
  // called from any thread. Current use case is poping from the browser thread.
  void PopPrinterQuery(int document_cookie, scoped_refptr<PrinterQuery>* job);

  // content::NotificationObserver
  virtual void Observe(int type,
                       const content::NotificationSource& source,
                       const content::NotificationDetails& details) OVERRIDE;

  // May return NULL when no destination was set.
  PrintDestinationInterface* destination() const { return destination_.get(); }

 private:
  typedef std::set<scoped_refptr<PrintJob> > PrintJobs;
  typedef std::vector<scoped_refptr<PrinterQuery> > PrinterQueries;

  // Processes a NOTIFY_PRINT_JOB_EVENT notification.
  void OnPrintJobEvent(PrintJob* print_job,
                       const JobEventDetails& event_details);

  content::NotificationRegistrar registrar_;

  // Used to serialize access to queued_workers_.
  base::Lock lock_;

  PrinterQueries queued_queries_;

  scoped_refptr<PrintDestinationInterface> destination_;

  // Current print jobs that are active.
  PrintJobs current_jobs_;

  DISALLOW_COPY_AND_ASSIGN(PrintJobManager);
};

}  // namespace printing

#endif  // CHROME_BROWSER_PRINTING_PRINT_JOB_MANAGER_H_
