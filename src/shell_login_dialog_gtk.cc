// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/shell_login_dialog.h"

#include <gtk/gtk.h>

#include "base/logging.h"
#include "base/string16.h"
#include "base/utf_string_conversions.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/resource_dispatcher_host.h"
#include "content/public/browser/resource_request_info.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_view.h"
#include "ui/base/gtk/gtk_hig_constants.h"

namespace content {

void ShellLoginDialog::PlatformCreateDialog(const string16& message) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  int render_process_id;
  int render_view_id;
  if (!ResourceRequestInfo::ForRequest(request_)->GetAssociatedRenderView(
          &render_process_id,  &render_view_id)) {
    NOTREACHED();
  }

  WebContents* web_contents = NULL;
  RenderViewHost* render_view_host =
      RenderViewHost::FromID(render_process_id, render_view_id);
  if (render_view_host)
    web_contents = WebContents::FromRenderViewHost(render_view_host);
  DCHECK(web_contents);

  gfx::NativeWindow parent_window =
      web_contents->GetView()->GetTopLevelNativeWindow();

  root_ = gtk_message_dialog_new(parent_window,
                                 GTK_DIALOG_MODAL,
                                 GTK_MESSAGE_INFO,
                                 GTK_BUTTONS_OK_CANCEL,
                                 "Please log in.");

  GtkWidget* content_area = gtk_dialog_get_content_area(GTK_DIALOG(root_));
  GtkWidget* label = gtk_label_new(UTF16ToUTF8(message).c_str());
  gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
  gtk_box_pack_start(GTK_BOX(content_area), label, FALSE, FALSE, 0);

  username_entry_ = gtk_entry_new();
  gtk_entry_set_activates_default(GTK_ENTRY(username_entry_), TRUE);

  password_entry_ = gtk_entry_new();
  gtk_entry_set_activates_default(GTK_ENTRY(password_entry_), TRUE);
  gtk_entry_set_visibility(GTK_ENTRY(password_entry_), FALSE);

  GtkWidget* table = gtk_table_new(2, 2, FALSE);
  gtk_table_set_col_spacing(GTK_TABLE(table), 0, ui::kLabelSpacing);
  gtk_table_set_row_spacings(GTK_TABLE(table), ui::kControlSpacing);

  GtkWidget* username_label = gtk_label_new("Username:");
  gtk_misc_set_alignment(GTK_MISC(username_label), 0, 0.5);

  gtk_table_attach(GTK_TABLE(table), username_label, 0, 1, 0, 1, GTK_FILL,
                   GTK_FILL, 0, 0);
  gtk_table_attach_defaults(GTK_TABLE(table), username_entry_, 1, 2, 0, 1);

  GtkWidget* password_label = gtk_label_new("Password:");
  gtk_misc_set_alignment(GTK_MISC(password_label), 0, 0.5);

  gtk_table_attach(GTK_TABLE(table), password_label, 0, 1, 1, 2, GTK_FILL,
                   GTK_FILL, 0, 0);
  gtk_table_attach_defaults(GTK_TABLE(table), password_entry_, 1, 2, 1, 2);

  gtk_box_pack_start(GTK_BOX(content_area), table, FALSE, FALSE, 0);

  g_signal_connect(root_, "response", G_CALLBACK(OnResponseThunk), this);
  gtk_widget_grab_focus(username_entry_);
  gtk_widget_show_all(GTK_WIDGET(root_));
}

void ShellLoginDialog::PlatformCleanUp() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
}

void ShellLoginDialog::PlatformRequestCancelled() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
}

void ShellLoginDialog::OnResponse(GtkWidget* sender, int response_id) {
  switch (response_id) {
    case GTK_RESPONSE_OK:
      UserAcceptedAuth(
          UTF8ToUTF16(gtk_entry_get_text(GTK_ENTRY(username_entry_))),
          UTF8ToUTF16(gtk_entry_get_text(GTK_ENTRY(password_entry_))));
      break;
    case GTK_RESPONSE_CANCEL:
    case GTK_RESPONSE_DELETE_EVENT:
      UserCancelledAuth();
      break;
    default:
      NOTREACHED();
  }

  gtk_widget_destroy(root_);
}

}  // namespace content
