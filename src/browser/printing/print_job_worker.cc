// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/printing/print_job_worker.h"

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/message_loop/message_loop.h"
#include "base/values.h"
#include "content/nw/src/browser/printing/print_job.h"
#include "chrome/browser/printing/printing_ui_web_contents_observer.h"
#include "content/public/browser/notification_types.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/content_browser_client.h"
#include "content/public/common/content_client.h"
#include "grit/nw_resources.h"
#include "printing/backend/print_backend.h"
#include "printing/print_job_constants.h"
#include "printing/printed_document.h"
#include "printing/printed_page.h"
#include "printing/printing_utils.h"
#include "ui/base/l10n/l10n_util.h"

using content::BrowserThread;
using base::MessageLoop;

namespace {

// Helper function to ensure |owner| is valid until at least |callback| returns.
void HoldRefCallback(const scoped_refptr<printing::PrintJobWorkerOwner>& owner,
                     const base::Closure& callback) {
  callback.Run();
}

}  // namespace

namespace printing {

void NotificationCallback(PrintJobWorkerOwner* print_job,
                          JobEventDetails::Type detail_type,
                          PrintedDocument* document,
                          PrintedPage* page) {
  JobEventDetails* details = new JobEventDetails(detail_type, document, page);
  content::NotificationService::current()->Notify(
      content::NOTIFICATION_PRINT_JOB_EVENT,
      // We know that is is a PrintJob object in this circumstance.
      content::Source<PrintJob>(static_cast<PrintJob*>(print_job)),
      content::Details<JobEventDetails>(details));
}

PrintJobWorker::PrintJobWorker(PrintJobWorkerOwner* owner)
    : Thread("Printing_Worker"),
      owner_(owner),
      weak_factory_(this) {
  // The object is created in the IO thread.
  DCHECK_EQ(owner_->message_loop(), MessageLoop::current());

  printing_context_.reset(PrintingContext::Create(
                                                  content::GetContentClient()->browser()->GetApplicationLocale()));
}

PrintJobWorker::~PrintJobWorker() {
  // The object is normally deleted in the UI thread, but when the user
  // cancels printing or in the case of print preview, the worker is destroyed
  // on the I/O thread.
  DCHECK_EQ(owner_->message_loop(), MessageLoop::current());
  Stop();
}

void PrintJobWorker::SetNewOwner(PrintJobWorkerOwner* new_owner) {
  DCHECK(page_number_ == PageNumber::npos());
  owner_ = new_owner;
}

void PrintJobWorker::SetPrintDestination(
    PrintDestinationInterface* destination) {
  destination_ = destination;
}

void PrintJobWorker::GetSettings(
    bool ask_user_for_settings,
    scoped_ptr<PrintingUIWebContentsObserver> web_contents_observer,
    int document_page_count,
    bool has_selection,
    MarginType margin_type) {
  DCHECK_EQ(message_loop(), MessageLoop::current());
  DCHECK_EQ(page_number_, PageNumber::npos());

  // Recursive task processing is needed for the dialog in case it needs to be
  // destroyed by a task.
  // TODO(thestig): This code is wrong. SetNestableTasksAllowed(true) is needed
  // on the thread where the PrintDlgEx is called, and definitely both calls
  // should happen on the same thread. See http://crbug.com/73466
  // MessageLoop::current()->SetNestableTasksAllowed(true);
  printing_context_->set_margin_type(margin_type);

  // When we delegate to a destination, we don't ask the user for settings.
  // TODO(mad): Ask the destination for settings.
  if (ask_user_for_settings && destination_.get() == NULL) {
    BrowserThread::PostTask(
        BrowserThread::UI, FROM_HERE,
        base::Bind(&HoldRefCallback, make_scoped_refptr(owner_),
                   base::Bind(&PrintJobWorker::GetSettingsWithUI,
                              base::Unretained(this),
                              base::Passed(&web_contents_observer),
                              document_page_count,
                              has_selection)));
  } else {
    BrowserThread::PostTask(
        BrowserThread::UI, FROM_HERE,
        base::Bind(&HoldRefCallback, make_scoped_refptr(owner_),
                   base::Bind(&PrintJobWorker::UseDefaultSettings,
                              base::Unretained(this))));
  }
}

void PrintJobWorker::SetSettings(const base::DictionaryValue* const new_settings) {
  DCHECK_EQ(message_loop(), MessageLoop::current());

  BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&HoldRefCallback, make_scoped_refptr(owner_),
                 base::Bind(&PrintJobWorker::UpdatePrintSettings,
                            base::Unretained(this), new_settings)));
}

void PrintJobWorker::UpdatePrintSettings(
    const base::DictionaryValue* const new_settings) {
  PrintingContext::Result result =
    printing_context_->UpdatePrintSettings(*new_settings);
  GetSettingsDone(result);
}

void PrintJobWorker::GetSettingsDone(PrintingContext::Result result) {
  // Most PrintingContext functions may start a message loop and process
  // message recursively, so disable recursive task processing.
  // TODO(thestig): See above comment. SetNestableTasksAllowed(false) needs to
  // be called on the same thread as the previous call.  See
  // http://crbug.com/73466
  // MessageLoop::current()->SetNestableTasksAllowed(false);

  // We can't use OnFailure() here since owner_ may not support notifications.

  // PrintJob will create the new PrintedDocument.
  owner_->message_loop()->PostTask(
      FROM_HERE,
      base::Bind(&PrintJobWorkerOwner::GetSettingsDone,
                 make_scoped_refptr(owner_), printing_context_->settings(),
                 result));
}

void PrintJobWorker::GetSettingsWithUI(
    scoped_ptr<PrintingUIWebContentsObserver> web_contents_observer,
    int document_page_count,
    bool has_selection) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  gfx::NativeView parent_view = web_contents_observer->GetParentView();
  if (!parent_view) {
    GetSettingsWithUIDone(printing::PrintingContext::FAILED);
    return;
  }
  printing_context_->AskUserForSettings(
      parent_view, document_page_count, has_selection,
      base::Bind(&PrintJobWorker::GetSettingsWithUIDone,
                 base::Unretained(this)));
}

void PrintJobWorker::GetSettingsWithUIDone(PrintingContext::Result result) {
  message_loop()->PostTask(
      FROM_HERE,
      base::Bind(&HoldRefCallback, make_scoped_refptr(owner_),
                 base::Bind(&PrintJobWorker::GetSettingsDone,
                            base::Unretained(this), result)));
}

void PrintJobWorker::UseDefaultSettings() {
  PrintingContext::Result result = printing_context_->UseDefaultSettings();
  GetSettingsDone(result);
}

void PrintJobWorker::StartPrinting(PrintedDocument* new_document) {
  DCHECK_EQ(message_loop(), MessageLoop::current());
  DCHECK_EQ(page_number_, PageNumber::npos());
  DCHECK_EQ(document_, new_document);
  DCHECK(document_.get());

  if (!document_.get() || page_number_ != PageNumber::npos() ||
      document_ != new_document) {
    return;
  }

  base::string16 document_name =
      printing::SimplifyDocumentTitle(document_->name());
  if (document_name.empty()) {
    document_name = printing::SimplifyDocumentTitle(
        l10n_util::GetStringUTF16(IDS_DEFAULT_PRINT_DOCUMENT_TITLE));
  }
  PrintingContext::Result result =
      printing_context_->NewDocument(document_name);
  if (result != PrintingContext::OK) {
    OnFailure();
    return;
  }

  // Try to print already cached data. It may already have been generated for
  // the print preview.
  OnNewPage();
  // Don't touch this anymore since the instance could be destroyed. It happens
  // if all the pages are printed a one sweep and the client doesn't have a
  // handle to us anymore. There's a timing issue involved between the worker
  // thread and the UI thread. Take no chance.
}

void PrintJobWorker::OnDocumentChanged(PrintedDocument* new_document) {
  DCHECK_EQ(message_loop(), MessageLoop::current());
  DCHECK_EQ(page_number_, PageNumber::npos());

  if (page_number_ != PageNumber::npos())
    return;

  document_ = new_document;
}

void PrintJobWorker::OnNewPage() {
  if (!document_.get())  // Spurious message.
    return;

  // message_loop() could return NULL when the print job is cancelled.
  DCHECK_EQ(message_loop(), MessageLoop::current());

  if (page_number_ == PageNumber::npos()) {
    // Find first page to print.
    int page_count = document_->page_count();
    if (!page_count) {
      // We still don't know how many pages the document contains. We can't
      // start to print the document yet since the header/footer may refer to
      // the document's page count.
      return;
    }
    // We have enough information to initialize page_number_.
    page_number_.Init(document_->settings(), page_count);
    if (destination_.get() != NULL)
      destination_->SetPageCount(page_count);
  }
  DCHECK_NE(page_number_, PageNumber::npos());

  while (true) {
    // Is the page available?
    scoped_refptr<PrintedPage> page = document_->GetPage(page_number_.ToInt());
    if (!page) {
      // We need to wait for the page to be available.
      MessageLoop::current()->PostDelayedTask(
          FROM_HERE,
          base::Bind(&PrintJobWorker::OnNewPage, weak_factory_.GetWeakPtr()),
          base::TimeDelta::FromMilliseconds(500));
      break;
    }
    // The page is there, print it.
    SpoolPage(page);
    ++page_number_;
    if (page_number_ == PageNumber::npos()) {
      OnDocumentDone();
      // Don't touch this anymore since the instance could be destroyed.
      break;
    }
  }
}

void PrintJobWorker::Cancel() {
  // This is the only function that can be called from any thread.
  printing_context_->Cancel();
  // Cannot touch any member variable since we don't know in which thread
  // context we run.
}

void PrintJobWorker::OnDocumentDone() {
  DCHECK_EQ(message_loop(), MessageLoop::current());
  DCHECK_EQ(page_number_, PageNumber::npos());
  DCHECK(document_.get());

  if (printing_context_->DocumentDone() != PrintingContext::OK) {
    OnFailure();
    return;
  }

  owner_->message_loop()->PostTask(
      FROM_HERE, base::Bind(NotificationCallback, make_scoped_refptr(owner_),
                            JobEventDetails::DOC_DONE, document_,
                            scoped_refptr<PrintedPage>()));

  // Makes sure the variables are reinitialized.
  document_ = NULL;
}

void PrintJobWorker::SpoolPage(PrintedPage* page) {
  DCHECK_EQ(message_loop(), MessageLoop::current());
  DCHECK_NE(page_number_, PageNumber::npos());

  // Signal everyone that the page is about to be printed.
  owner_->message_loop()->PostTask(
      FROM_HERE, base::Bind(NotificationCallback, make_scoped_refptr(owner_),
                            JobEventDetails::NEW_PAGE, document_,
                            make_scoped_refptr(page)));

  // Preprocess.
  if (printing_context_->NewPage() != PrintingContext::OK) {
    OnFailure();
    return;
  }

  if (destination_.get() != NULL) {
    std::vector<uint8> metabytes(page->metafile()->GetDataSize());
    bool success = page->metafile()->GetData(
        reinterpret_cast<void*>(&metabytes[0]), metabytes.size());
    DCHECK(success) << "Failed to get metafile data.";
    destination_->SetPageContent(
        page->page_number(),
        reinterpret_cast<void*>(&metabytes[0]),
        metabytes.size());
    return;
  }

  // Actual printing.
#if defined(OS_WIN) || defined(OS_MACOSX)
  document_->RenderPrintedPage(*page, printing_context_->context());
#elif defined(OS_POSIX)
  document_->RenderPrintedPage(*page, printing_context_.get());
#endif

  // Postprocess.
  if (printing_context_->PageDone() != PrintingContext::OK) {
    OnFailure();
    return;
  }

  // Signal everyone that the page is printed.
  owner_->message_loop()->PostTask(
      FROM_HERE,
      base::Bind(NotificationCallback, make_scoped_refptr(owner_),
                 JobEventDetails::PAGE_DONE, document_,
                 make_scoped_refptr(page)));
}

void PrintJobWorker::OnFailure() {
  DCHECK_EQ(message_loop(), MessageLoop::current());

  // We may loose our last reference by broadcasting the FAILED event.
  scoped_refptr<PrintJobWorkerOwner> handle(owner_);

  owner_->message_loop()->PostTask(
      FROM_HERE, base::Bind(NotificationCallback, make_scoped_refptr(owner_),
                            JobEventDetails::FAILED, document_,
                            scoped_refptr<PrintedPage>()));
  Cancel();

  // Makes sure the variables are reinitialized.
  document_ = NULL;
  page_number_ = PageNumber::npos();
}

}  // namespace printing
