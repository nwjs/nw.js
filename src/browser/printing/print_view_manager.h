// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_PRINTING_PRINT_VIEW_MANAGER_H_
#define CHROME_BROWSER_PRINTING_PRINT_VIEW_MANAGER_H_

#include "base/memory/ref_counted.h"
#include "base/prefs/pref_member.h"
#include "base/strings/string16.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/notification_registrar.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/public/browser/web_contents_user_data.h"
#include "printing/printed_pages_source.h"

struct PrintHostMsg_DidPrintPage_Params;

namespace content {
class RenderProcessHost;
class RenderViewHost;
}

namespace printing {

class JobEventDetails;
class PrintJob;
class PrintJobWorkerOwner;
class PrintViewManagerObserver;

// Manages the print commands for a WebContents.
class PrintViewManager : public content::NotificationObserver,
                         public PrintedPagesSource,
                         public content::WebContentsObserver,
                         public content::WebContentsUserData<PrintViewManager> {
 public:
  virtual ~PrintViewManager();

  // Prints the current document immediately. Since the rendering is
  // asynchronous, the actual printing will not be completed on the return of
  // this function. Returns false if printing is impossible at the moment.
  bool PrintNow();

  // Same as PrintNow(), but for the case where a user prints with the system
  // dialog from print preview.
  bool PrintForSystemDialogNow();

  // Same as PrintNow(), but for the case where we want to send the result to
  // another destination.
  // TODO(mad) Add an argument so we can pass the destination interface.
  bool PrintToDestination();

  // Whether to block scripted printing for our tab or not.
  void UpdateScriptedPrintingBlocked();

  // Sets |observer| as the current PrintViewManagerObserver. Pass in NULL to
  // remove the current observer. |observer| may always be NULL, but |observer_|
  // must be NULL if |observer| is non-NULL.
  void set_observer(PrintViewManagerObserver* observer);

  // PrintedPagesSource implementation.
  virtual base::string16 RenderSourceName() override;

  // content::NotificationObserver implementation.
  virtual void Observe(int type,
                       const content::NotificationSource& source,
                       const content::NotificationDetails& details) override;

  // content::WebContentsObserver implementation.
  virtual void DidStartLoading(
      content::RenderViewHost* render_view_host) override;

  // content::WebContentsObserver implementation.
  virtual bool OnMessageReceived(const IPC::Message& message) override;

  // Terminates or cancels the print job if one was pending.
  virtual void RenderProcessGone(base::TerminationStatus status) override;

  // Cancels the print job.
  virtual void NavigationStopped() override;

 private:
  explicit PrintViewManager(content::WebContents* web_contents);
  friend class content::WebContentsUserData<PrintViewManager>;

  enum PrintPreviewState {
    NOT_PREVIEWING,
    USER_INITIATED_PREVIEW,
    SCRIPTED_PREVIEW,
  };

  // IPC Message handlers.
  void OnDidGetPrintedPagesCount(int cookie, int number_pages);
  void OnDidGetDocumentCookie(int cookie);
  void OnDidShowPrintDialog();
  void OnDidPrintPage(const PrintHostMsg_DidPrintPage_Params& params);
  void OnPrintingFailed(int cookie);

  void OnScriptedPrintPreview(bool source_is_modifiable,
                              IPC::Message* reply_msg);
  void OnScriptedPrintPreviewReply(IPC::Message* reply_msg);

  // Processes a NOTIFY_PRINT_JOB_EVENT notification.
  void OnNotifyPrintJobEvent(const JobEventDetails& event_details);

  // Requests the RenderView to render all the missing pages for the print job.
  // No-op if no print job is pending. Returns true if at least one page has
  // been requested to the renderer.
  bool RenderAllMissingPagesNow();

  // Quits the current message loop if these conditions hold true: a document is
  // loaded and is complete and waiting_for_pages_to_be_rendered_ is true. This
  // function is called in DidPrintPage() or on ALL_PAGES_REQUESTED
  // notification. The inner message loop is created was created by
  // RenderAllMissingPagesNow().
  void ShouldQuitFromInnerMessageLoop();

  // Creates a new empty print job. It has no settings loaded. If there is
  // currently a print job, safely disconnect from it. Returns false if it is
  // impossible to safely disconnect from the current print job or it is
  // impossible to create a new print job.
  bool CreateNewPrintJob(PrintJobWorkerOwner* job);

  // Makes sure the current print_job_ has all its data before continuing, and
  // disconnect from it.
  void DisconnectFromCurrentPrintJob();

  // Notify that the printing is done.
  void PrintingDone(bool success);

  // Terminates the print job. No-op if no print job has been created. If
  // |cancel| is true, cancel it instead of waiting for the job to finish. Will
  // call ReleasePrintJob().
  void TerminatePrintJob(bool cancel);

  // Releases print_job_. Correctly deregisters from notifications. No-op if
  // no print job has been created.
  void ReleasePrintJob();

  // Runs an inner message loop. It will set inside_inner_message_loop_ to true
  // while the blocking inner message loop is running. This is useful in cases
  // where the RenderView is about to be destroyed while a printing job isn't
  // finished.
  bool RunInnerMessageLoop();

  // In the case of Scripted Printing, where the renderer is controlling the
  // control flow, print_job_ is initialized whenever possible. No-op is
  // print_job_ is initialized.
  bool OpportunisticallyCreatePrintJob(int cookie);

  // Helper method for Print*Now().
  bool PrintNowInternal(IPC::Message* message);

  // Release the PrinterQuery associated with our |cookie_|.
  void ReleasePrinterQuery();

  content::NotificationRegistrar registrar_;

  // Manages the low-level talk to the printer.
  scoped_refptr<PrintJob> print_job_;

  // Number of pages to print in the print job.
  int number_pages_;

  // Indication of success of the print job.
  bool printing_succeeded_;

  // Running an inner message loop inside RenderAllMissingPagesNow(). This means
  // we are _blocking_ until all the necessary pages have been rendered or the
  // print settings are being loaded.
  bool inside_inner_message_loop_;

#if defined(OS_POSIX) && !defined(OS_MAC)
  // Set to true when OnDidPrintPage() should be expecting the first page.
  bool expecting_first_page_;
#endif

  // Weak pointer to an observer that is notified when the print dialog is
  // shown.
  PrintViewManagerObserver* observer_;

  // The document cookie of the current PrinterQuery.
  int cookie_;


  // Keeps track of the pending callback during scripted print preview.
  content::RenderProcessHost* scripted_print_preview_rph_;

  // Whether printing is enabled.
  bool printing_enabled_;

  // Whether our content is in blocked state.
  bool tab_content_blocked_;

  DISALLOW_COPY_AND_ASSIGN(PrintViewManager);
};

}  // namespace printing

#endif  // CHROME_BROWSER_PRINTING_PRINT_VIEW_MANAGER_H_
