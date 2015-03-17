// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/printing/print_view_manager.h"

#include <map>

#include "base/bind.h"
#include "base/lazy_instance.h"
#include "base/metrics/histogram.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/printing/print_job_manager.h"
#include "chrome/browser/printing/print_preview_dialog_controller.h"
#include "chrome/browser/printing/print_view_manager_observer.h"
#include "chrome/browser/ui/webui/print_preview/print_preview_ui.h"
#include "chrome/common/print_messages.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/web_contents.h"
#include "printing/print_destination_interface.h"

#include "content/nw/src/shell_content_browser_client.h"

using content::BrowserThread;

DEFINE_WEB_CONTENTS_USER_DATA_KEY(printing::PrintViewManager);

namespace {

// Keeps track of pending scripted print preview closures.
// No locking, only access on the UI thread.
typedef std::map<content::RenderProcessHost*, base::Closure>
    ScriptedPrintPreviewClosureMap;
#if 0
static base::LazyInstance<ScriptedPrintPreviewClosureMap>
    g_scripted_print_preview_closure_map = LAZY_INSTANCE_INITIALIZER;
#endif

}  // namespace

namespace printing {

PrintViewManager::PrintViewManager(content::WebContents* web_contents)
    : PrintViewManagerBase(web_contents),
      observer_(NULL),
      print_preview_state_(NOT_PREVIEWING),
      scripted_print_preview_rph_(NULL) {
}

PrintViewManager::~PrintViewManager() {
  DCHECK_EQ(NOT_PREVIEWING, print_preview_state_);
}

bool PrintViewManager::PrintForSystemDialogNow() {
  return PrintNowInternal(new PrintMsg_PrintForSystemDialog(routing_id()));
}

bool PrintViewManager::AdvancedPrintNow() {
#if 0
  PrintPreviewDialogController* dialog_controller =
      PrintPreviewDialogController::GetInstance();
  if (!dialog_controller)
    return false;
  content::WebContents* print_preview_dialog =
      dialog_controller->GetPrintPreviewForContents(web_contents());
  if (print_preview_dialog) {
    if (!print_preview_dialog->GetWebUI())
      return false;
    PrintPreviewUI* print_preview_ui = static_cast<PrintPreviewUI*>(
        print_preview_dialog->GetWebUI()->GetController());
    print_preview_ui->OnShowSystemDialog();
    return true;
  } else {
#endif
    return PrintNow();
  //}
}

bool PrintViewManager::PrintToDestination() {
  // TODO(mad): Remove this once we can send user metrics from the metro driver.
  // crbug.com/142330
  UMA_HISTOGRAM_ENUMERATION("Metro.Print", 0, 2);
  // TODO(mad): Use a passed in destination interface instead.

  content::ShellContentBrowserClient* browser_client =
    static_cast<content::ShellContentBrowserClient*>(content::GetContentClient()->browser());
  browser_client->print_job_manager()->queue()->SetDestination(
      printing::CreatePrintDestination());
  return PrintNowInternal(new PrintMsg_PrintPages(routing_id()));
}

bool PrintViewManager::PrintPreviewNow(bool selection_only) {
#if 0
  // Users can send print commands all they want and it is beyond
  // PrintViewManager's control. Just ignore the extra commands.
  // See http://crbug.com/136842 for example.
  if (print_preview_state_ != NOT_PREVIEWING)
    return false;

  if (!PrintNowInternal(new PrintMsg_InitiatePrintPreview(routing_id(),
                                                          selection_only))) {
    return false;
  }

  print_preview_state_ = USER_INITIATED_PREVIEW;
#endif
  return true;
}

void PrintViewManager::PrintPreviewForWebNode() {
#if 0
  if (print_preview_state_ != NOT_PREVIEWING)
    return;
  print_preview_state_ = USER_INITIATED_PREVIEW;
#endif
}

void PrintViewManager::PrintPreviewDone() {
#if 0
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  DCHECK_NE(NOT_PREVIEWING, print_preview_state_);

  if (print_preview_state_ == SCRIPTED_PREVIEW) {
    ScriptedPrintPreviewClosureMap& map =
        g_scripted_print_preview_closure_map.Get();
    ScriptedPrintPreviewClosureMap::iterator it =
        map.find(scripted_print_preview_rph_);
    CHECK(it != map.end());
    it->second.Run();
    map.erase(scripted_print_preview_rph_);
    scripted_print_preview_rph_ = NULL;
  }
  print_preview_state_ = NOT_PREVIEWING;
#endif
}

void PrintViewManager::set_observer(PrintViewManagerObserver* observer) {
  DCHECK(!observer || !observer_);
  observer_ = observer;
}

void PrintViewManager::RenderProcessGone(base::TerminationStatus status) {
  print_preview_state_ = NOT_PREVIEWING;
  PrintViewManagerBase::RenderProcessGone(status);
}

void PrintViewManager::OnDidShowPrintDialog() {
  if (observer_)
    observer_->OnPrintDialogShown();
}

void PrintViewManager::OnSetupScriptedPrintPreview(IPC::Message* reply_msg) {
#if 0
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  ScriptedPrintPreviewClosureMap& map =
      g_scripted_print_preview_closure_map.Get();
  content::RenderProcessHost* rph = web_contents()->GetRenderProcessHost();

  // This should always be 0 once we get modal window.print().
  if (map.count(rph) != 0) {
    // Renderer already handling window.print() in another View.
    Send(reply_msg);
    return;
  }
  if (print_preview_state_ != NOT_PREVIEWING) {
    // If a user initiated print dialog is already open, ignore the scripted
    // print message.
    DCHECK_EQ(USER_INITIATED_PREVIEW, print_preview_state_);
    Send(reply_msg);
    return;
  }

  PrintPreviewDialogController* dialog_controller =
      PrintPreviewDialogController::GetInstance();
  if (!dialog_controller) {
    Send(reply_msg);
    return;
  }

  print_preview_state_ = SCRIPTED_PREVIEW;
  base::Closure callback =
      base::Bind(&PrintViewManager::OnScriptedPrintPreviewReply,
                 base::Unretained(this),
                 reply_msg);
  map[rph] = callback;
  scripted_print_preview_rph_ = rph;
#endif
}

void PrintViewManager::OnShowScriptedPrintPreview(bool source_is_modifiable) {
#if 0
  PrintPreviewDialogController* dialog_controller =
      PrintPreviewDialogController::GetInstance();
  if (!dialog_controller) {
    PrintPreviewDone();
    return;
  }
  dialog_controller->PrintPreview(web_contents());
  PrintHostMsg_RequestPrintPreview_Params params;
  params.is_modifiable = source_is_modifiable;
  PrintPreviewUI::SetInitialParams(
      dialog_controller->GetPrintPreviewForContents(web_contents()), params);
#endif
}

void PrintViewManager::OnScriptedPrintPreviewReply(IPC::Message* reply_msg) {
#if 0
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  Send(reply_msg);
#endif
}

bool PrintViewManager::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(PrintViewManager, message)
    IPC_MESSAGE_HANDLER(PrintHostMsg_DidShowPrintDialog, OnDidShowPrintDialog)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()

  return handled ? true : PrintViewManagerBase::OnMessageReceived(message);
}

}  // namespace printing
