// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/printing/print_view_manager.h"

#include <map>

#include "base/bind.h"
#include "base/command_line.h"
#include "base/lazy_instance.h"
#include "base/memory/scoped_ptr.h"
#include "base/metrics/histogram.h"
#include "base/prefs/pref_service.h"
#include "base/timer/timer.h"
#include "base/strings/utf_string_conversions.h"
//#include "chrome/browser/browser_process.h"
//#include "chrome/browser/printing/print_error_dialog.h"
#include "content/nw/src/browser/printing/print_job.h"
#include "content/nw/src/browser/printing/print_job_manager.h"
//#include "content/nw/src/browser/printing/print_preview_dialog_controller.h"
#include "content/nw/src/browser/printing/print_view_manager_observer.h"
#include "content/nw/src/browser/printing/printer_query.h"
//#include "chrome/browser/profiles/profile.h"
//#include "chrome/browser/ui/webui/print_preview/print_preview_ui.h"
#include "content/nw/src/shell_content_browser_client.h"
#include "content/public/browser/notification_types.h"
//#include "chrome/common/chrome_switches.h"
//#include "chrome/common/pref_names.h"
#include "content/nw/src/common/print_messages.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/notification_details.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/notification_source.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "grit/nw_resources.h"
#include "printing/metafile.h"
#include "printing/metafile_impl.h"
#include "printing/print_destination_interface.h"
#include "printing/printed_document.h"
#include "ui/base/l10n/l10n_util.h"

using base::TimeDelta;
using base::MessageLoop;
using content::BrowserThread;
using content::WebContents;

DEFINE_WEB_CONTENTS_USER_DATA_KEY(printing::PrintViewManager);

namespace {

#if defined(OS_WIN) && !defined(WIN_PDF_METAFILE_FOR_PRINTING)
// Limits memory usage by raster to 64 MiB.
const int kMaxRasterSizeInPixels = 16*1024*1024;
#endif

}  // namespace

namespace printing {

PrintViewManager::PrintViewManager(content::WebContents* web_contents)
    : content::WebContentsObserver(web_contents),
      number_pages_(0),
      printing_succeeded_(false),
      inside_inner_message_loop_(false),
      observer_(NULL),
      cookie_(0),
      scripted_print_preview_rph_(NULL),
      tab_content_blocked_(false) {
#if defined(OS_POSIX) && !defined(OS_MAC)
  expecting_first_page_ = true;
#endif
  registrar_.Add(this, content::NOTIFICATION_CONTENT_BLOCKED_STATE_CHANGED,
                 content::Source<content::WebContents>(web_contents));
  printing_enabled_ = true;
}

PrintViewManager::~PrintViewManager() {
  ReleasePrinterQuery();
  DisconnectFromCurrentPrintJob();
}

bool PrintViewManager::PrintNow() {
  return PrintNowInternal(new PrintMsg_PrintPages(routing_id()));
}

bool PrintViewManager::PrintForSystemDialogNow() {
  return PrintNowInternal(new PrintMsg_PrintForSystemDialog(routing_id()));
}

// bool PrintViewManager::AdvancedPrintNow() {
//   return PrintNow();
// }

bool PrintViewManager::PrintToDestination() {
  // TODO(mad): Remove this once we can send user metrics from the metro driver.
  // crbug.com/142330
  UMA_HISTOGRAM_ENUMERATION("Metro.Print", 0, 2);
  // TODO(mad): Use a passed in destination interface instead.
  content::ShellContentBrowserClient* browser_client =
    static_cast<content::ShellContentBrowserClient*>(content::GetContentClient()->browser());
    browser_client->print_job_manager()->SetPrintDestination(
      printing::CreatePrintDestination());
  return PrintNowInternal(new PrintMsg_PrintPages(routing_id()));
}

void PrintViewManager::UpdateScriptedPrintingBlocked() {
  Send(new PrintMsg_SetScriptedPrintingBlocked(
       routing_id(),
       !printing_enabled_ || tab_content_blocked_));
}

void PrintViewManager::set_observer(PrintViewManagerObserver* observer) {
  DCHECK(!observer || !observer_);
  observer_ = observer;
}

void PrintViewManager::NavigationStopped() {
  // Cancel the current job, wait for the worker to finish.
  TerminatePrintJob(true);
}

void PrintViewManager::RenderProcessGone(base::TerminationStatus status) {
  ReleasePrinterQuery();

  if (!print_job_.get())
    return;

  scoped_refptr<PrintedDocument> document(print_job_->document());
  if (document) {
    // If IsComplete() returns false, the document isn't completely rendered.
    // Since our renderer is gone, there's nothing to do, cancel it. Otherwise,
    // the print job may finish without problem.
    TerminatePrintJob(!document->IsComplete());
  }
}

base::string16 PrintViewManager::RenderSourceName() {
  base::string16 name(web_contents()->GetTitle());
  if (name.empty())
    name = l10n_util::GetStringUTF16(IDS_DEFAULT_PRINT_DOCUMENT_TITLE);
  return name;
}

void PrintViewManager::OnDidGetPrintedPagesCount(int cookie, int number_pages) {
  DCHECK_GT(cookie, 0);
  DCHECK_GT(number_pages, 0);
  number_pages_ = number_pages;
  OpportunisticallyCreatePrintJob(cookie);
}

void PrintViewManager::OnDidGetDocumentCookie(int cookie) {
  cookie_ = cookie;
}

void PrintViewManager::OnDidShowPrintDialog() {
  if (observer_)
    observer_->OnPrintDialogShown();
}

void PrintViewManager::OnDidPrintPage(
    const PrintHostMsg_DidPrintPage_Params& params) {
  if (!OpportunisticallyCreatePrintJob(params.document_cookie))
    return;

  PrintedDocument* document = print_job_->document();
  if (!document || params.document_cookie != document->cookie()) {
    // Out of sync. It may happen since we are completely asynchronous. Old
    // spurious messages can be received if one of the processes is overloaded.
    return;
  }

#if defined(OS_WIN) || defined(OS_MAC)
  const bool metafile_must_be_valid = true;
#elif defined(OS_POSIX)
  const bool metafile_must_be_valid = expecting_first_page_;
  expecting_first_page_ = false;
#endif

  base::SharedMemory shared_buf(params.metafile_data_handle, true);
  if (metafile_must_be_valid) {
    if (!shared_buf.Map(params.data_size)) {
      NOTREACHED() << "couldn't map";
      web_contents()->Stop();
      return;
    }
  }

  scoped_ptr<NativeMetafile> metafile(new NativeMetafile);
  if (metafile_must_be_valid) {
    if (!metafile->InitFromData(shared_buf.memory(), params.data_size)) {
      NOTREACHED() << "Invalid metafile header";
      web_contents()->Stop();
      return;
    }
  }

#if defined(OS_WIN) && !defined(WIN_PDF_METAFILE_FOR_PRINTING)
  bool big_emf = (params.data_size && params.data_size >= kMetafileMaxSize);
  const CommandLine* cmdline = CommandLine::ForCurrentProcess();
  int raster_size = std::min(params.page_size.GetArea(),
                             kMaxRasterSizeInPixels);
  if (big_emf) {
    scoped_ptr<NativeMetafile> raster_metafile(
        metafile->RasterizeMetafile(raster_size));
    if (raster_metafile.get()) {
      metafile.swap(raster_metafile);
    } else if (big_emf) {
      // Don't fall back to emf here.
      NOTREACHED() << "size:" << params.data_size;
      TerminatePrintJob(true);
      web_contents()->Stop();
      return;
    }
  }
#endif

  // Update the rendered document. It will send notifications to the listener.
  document->SetPage(params.page_number,
                    metafile.release(),
#if defined(OS_WIN)
                    params.actual_shrink,
#endif
                    params.page_size,
                    params.content_area);

  ShouldQuitFromInnerMessageLoop();
}

void PrintViewManager::OnPrintingFailed(int cookie) {
  if (cookie != cookie_) {
    NOTREACHED();
    return;
  }

  ReleasePrinterQuery();

  content::NotificationService::current()->Notify(
      content::NOTIFICATION_PRINT_JOB_RELEASED,
      content::Source<content::WebContents>(web_contents()),
      content::NotificationService::NoDetails());
}

void PrintViewManager::OnScriptedPrintPreview(bool source_is_modifiable,
                                              IPC::Message* reply_msg) {
}

void PrintViewManager::OnScriptedPrintPreviewReply(IPC::Message* reply_msg) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  Send(reply_msg);
}

void PrintViewManager::DidStartLoading(
    content::RenderViewHost* render_view_host) {
  UpdateScriptedPrintingBlocked();
}

bool PrintViewManager::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(PrintViewManager, message)
    IPC_MESSAGE_HANDLER(PrintHostMsg_DidGetPrintedPagesCount,
                        OnDidGetPrintedPagesCount)
    IPC_MESSAGE_HANDLER(PrintHostMsg_DidGetDocumentCookie,
                        OnDidGetDocumentCookie)
    IPC_MESSAGE_HANDLER(PrintHostMsg_DidShowPrintDialog, OnDidShowPrintDialog)
    IPC_MESSAGE_HANDLER(PrintHostMsg_DidPrintPage, OnDidPrintPage)
    IPC_MESSAGE_HANDLER(PrintHostMsg_PrintingFailed, OnPrintingFailed)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}

void PrintViewManager::Observe(int type,
                               const content::NotificationSource& source,
                               const content::NotificationDetails& details) {
  switch (type) {
    case content::NOTIFICATION_PRINT_JOB_EVENT: {
      OnNotifyPrintJobEvent(*content::Details<JobEventDetails>(details).ptr());
      break;
    }
    case content::NOTIFICATION_CONTENT_BLOCKED_STATE_CHANGED: {
      tab_content_blocked_ = *content::Details<const bool>(details).ptr();
      UpdateScriptedPrintingBlocked();
      break;
    }
    default: {
      NOTREACHED();
      break;
    }
  }
}

void PrintViewManager::OnNotifyPrintJobEvent(
    const JobEventDetails& event_details) {
  switch (event_details.type()) {
    case JobEventDetails::FAILED: {
      TerminatePrintJob(true);

      content::NotificationService::current()->Notify(
          content::NOTIFICATION_PRINT_JOB_RELEASED,
          content::Source<content::WebContents>(web_contents()),
          content::NotificationService::NoDetails());
      break;
    }
    case JobEventDetails::USER_INIT_DONE:
    case JobEventDetails::DEFAULT_INIT_DONE:
    case JobEventDetails::USER_INIT_CANCELED: {
      NOTREACHED();
      break;
    }
    case JobEventDetails::ALL_PAGES_REQUESTED: {
      ShouldQuitFromInnerMessageLoop();
      break;
    }
    case JobEventDetails::NEW_DOC:
    case JobEventDetails::NEW_PAGE:
    case JobEventDetails::PAGE_DONE:
    case JobEventDetails::DOC_DONE: {
      // Don't care about the actual printing process.
      break;
    }
    case JobEventDetails::JOB_DONE: {
      // Printing is done, we don't need it anymore.
      // print_job_->is_job_pending() may still be true, depending on the order
      // of object registration.
      printing_succeeded_ = true;
      ReleasePrintJob();

      content::NotificationService::current()->Notify(
          content::NOTIFICATION_PRINT_JOB_RELEASED,
          content::Source<content::WebContents>(web_contents()),
          content::NotificationService::NoDetails());
      break;
    }
    default: {
      NOTREACHED();
      break;
    }
  }
}

bool PrintViewManager::RenderAllMissingPagesNow() {
  if (!print_job_.get() || !print_job_->is_job_pending())
    return false;

  // We can't print if there is no renderer.
  if (!web_contents() ||
      !web_contents()->GetRenderViewHost() ||
      !web_contents()->GetRenderViewHost()->IsRenderViewLive()) {
    return false;
  }

  // Is the document already complete?
  if (print_job_->document() && print_job_->document()->IsComplete()) {
    printing_succeeded_ = true;
    return true;
  }

  // WebContents is either dying or a second consecutive request to print
  // happened before the first had time to finish. We need to render all the
  // pages in an hurry if a print_job_ is still pending. No need to wait for it
  // to actually spool the pages, only to have the renderer generate them. Run
  // a message loop until we get our signal that the print job is satisfied.
  // PrintJob will send a ALL_PAGES_REQUESTED after having received all the
  // pages it needs. MessageLoop::current()->Quit() will be called as soon as
  // print_job_->document()->IsComplete() is true on either ALL_PAGES_REQUESTED
  // or in DidPrintPage(). The check is done in
  // ShouldQuitFromInnerMessageLoop().
  // BLOCKS until all the pages are received. (Need to enable recursive task)
  if (!RunInnerMessageLoop()) {
    // This function is always called from DisconnectFromCurrentPrintJob() so we
    // know that the job will be stopped/canceled in any case.
    return false;
  }
  return true;
}

void PrintViewManager::ShouldQuitFromInnerMessageLoop() {
  // Look at the reason.
  DCHECK(print_job_->document());
  if (print_job_->document() &&
      print_job_->document()->IsComplete() &&
      inside_inner_message_loop_) {
    // We are in a message loop created by RenderAllMissingPagesNow. Quit from
    // it.
    MessageLoop::current()->Quit();
    inside_inner_message_loop_ = false;
  }
}

bool PrintViewManager::CreateNewPrintJob(PrintJobWorkerOwner* job) {
  DCHECK(!inside_inner_message_loop_);

  // Disconnect the current print_job_.
  DisconnectFromCurrentPrintJob();

  // We can't print if there is no renderer.
  if (!web_contents()->GetRenderViewHost() ||
      !web_contents()->GetRenderViewHost()->IsRenderViewLive()) {
    return false;
  }

  // Ask the renderer to generate the print preview, create the print preview
  // view and switch to it, initialize the printer and show the print dialog.
  DCHECK(!print_job_.get());
  DCHECK(job);
  if (!job)
    return false;

  print_job_ = new PrintJob();
  print_job_->Initialize(job, this, number_pages_);
  registrar_.Add(this, content::NOTIFICATION_PRINT_JOB_EVENT,
                 content::Source<PrintJob>(print_job_.get()));
  printing_succeeded_ = false;
  return true;
}

void PrintViewManager::DisconnectFromCurrentPrintJob() {
  // Make sure all the necessary rendered page are done. Don't bother with the
  // return value.
  bool result = RenderAllMissingPagesNow();

  // Verify that assertion.
  if (print_job_.get() &&
      print_job_->document() &&
      !print_job_->document()->IsComplete()) {
    DCHECK(!result);
    // That failed.
    TerminatePrintJob(true);
  } else {
    // DO NOT wait for the job to finish.
    ReleasePrintJob();
  }
#if defined(OS_POSIX) && !defined(OS_MAC)
  expecting_first_page_ = true;
#endif
}

void PrintViewManager::PrintingDone(bool success) {
  if (!print_job_.get())
    return;
  Send(new PrintMsg_PrintingDone(routing_id(), success));
}

void PrintViewManager::TerminatePrintJob(bool cancel) {
  if (!print_job_.get())
    return;

  if (cancel) {
    // We don't need the metafile data anymore because the printing is canceled.
    print_job_->Cancel();
    inside_inner_message_loop_ = false;
  } else {
    DCHECK(!inside_inner_message_loop_);
    DCHECK(!print_job_->document() || print_job_->document()->IsComplete());

    // WebContents is either dying or navigating elsewhere. We need to render
    // all the pages in an hurry if a print job is still pending. This does the
    // trick since it runs a blocking message loop:
    print_job_->Stop();
  }
  ReleasePrintJob();
}

void PrintViewManager::ReleasePrintJob() {
  if (!print_job_.get())
    return;

  PrintingDone(printing_succeeded_);

  registrar_.Remove(this, content::NOTIFICATION_PRINT_JOB_EVENT,
                    content::Source<PrintJob>(print_job_.get()));
  print_job_->DisconnectSource();
  // Don't close the worker thread.
  print_job_ = NULL;
}

bool PrintViewManager::RunInnerMessageLoop() {
  // This value may actually be too low:
  //
  // - If we're looping because of printer settings initialization, the premise
  // here is that some poor users have their print server away on a VPN over a
  // slow connection. In this situation, the simple fact of opening the printer
  // can be dead slow. On the other side, we don't want to die infinitely for a
  // real network error. Give the printer 60 seconds to comply.
  //
  // - If we're looping because of renderer page generation, the renderer could
  // be CPU bound, the page overly complex/large or the system just
  // memory-bound.
  static const int kPrinterSettingsTimeout = 60000;
  base::OneShotTimer<MessageLoop> quit_timer;
  quit_timer.Start(FROM_HERE,
                   TimeDelta::FromMilliseconds(kPrinterSettingsTimeout),
                   MessageLoop::current(), &MessageLoop::Quit);

  inside_inner_message_loop_ = true;

  // Need to enable recursive task.
  {
    MessageLoop::ScopedNestableTaskAllower allow(MessageLoop::current());
    MessageLoop::current()->Run();
  }

  bool success = true;
  if (inside_inner_message_loop_) {
    // Ok we timed out. That's sad.
    inside_inner_message_loop_ = false;
    success = false;
  }

  return success;
}

bool PrintViewManager::OpportunisticallyCreatePrintJob(int cookie) {
  if (print_job_.get())
    return true;

  if (!cookie) {
    // Out of sync. It may happens since we are completely asynchronous. Old
    // spurious message can happen if one of the processes is overloaded.
    return false;
  }

  // The job was initiated by a script. Time to get the corresponding worker
  // thread.
  scoped_refptr<PrinterQuery> queued_query;
  content::ShellContentBrowserClient* browser_client =
    static_cast<content::ShellContentBrowserClient*>(content::GetContentClient()->browser());
  browser_client->print_job_manager()->PopPrinterQuery(cookie, &queued_query);
  DCHECK(queued_query.get());
  if (!queued_query.get())
    return false;

  if (!CreateNewPrintJob(queued_query.get())) {
    // Don't kill anything.
    return false;
  }

  // Settings are already loaded. Go ahead. This will set
  // print_job_->is_job_pending() to true.
  print_job_->StartPrinting();
  return true;
}

bool PrintViewManager::PrintNowInternal(IPC::Message* message) {
  // Don't print / print preview interstitials.
  if (web_contents()->ShowingInterstitialPage()) {
    delete message;
    return false;
  }
  return Send(message);
}

void PrintViewManager::ReleasePrinterQuery() {
  if (!cookie_)
    return;

  int cookie = cookie_;
  cookie_ = 0;
  content::ShellContentBrowserClient* browser_client =
    static_cast<content::ShellContentBrowserClient*>(content::GetContentClient()->browser());
  browser_client->print_job_manager()->SetPrintDestination(NULL);


  printing::PrintJobManager* print_job_manager =
      browser_client->print_job_manager();
  // May be NULL in tests.
  if (!print_job_manager)
    return;

  scoped_refptr<printing::PrinterQuery> printer_query;
  print_job_manager->PopPrinterQuery(cookie, &printer_query);
  if (!printer_query.get())
    return;
  BrowserThread::PostTask(
      BrowserThread::IO, FROM_HERE,
      base::Bind(&PrinterQuery::StopWorker, printer_query.get()));
}

}  // namespace printing
