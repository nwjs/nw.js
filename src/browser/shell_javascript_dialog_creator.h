// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_SHELL_JAVASCRIPT_DIALOG_CREATOR_H_
#define CONTENT_SHELL_SHELL_JAVASCRIPT_DIALOG_CREATOR_H_

#include "base/compiler_specific.h"
#include "base/callback_forward.h"
#include "base/memory/scoped_ptr.h"
#include "content/public/browser/javascript_dialog_manager.h"

namespace content {

class ShellJavaScriptDialog;

class ShellJavaScriptDialogCreator : public JavaScriptDialogManager {
 public:
  ShellJavaScriptDialogCreator();
  ~ShellJavaScriptDialogCreator() final;

  // JavaScriptDialogCreator:
  void RunJavaScriptDialog(
      WebContents* web_contents,
      const GURL& origin_url,
      const std::string& accept_lang,
      JavaScriptMessageType javascript_message_type,
      const base::string16& message_text,
      const base::string16& default_prompt_text,
      const DialogClosedCallback& callback,
      bool* did_suppress_message) override;

  void RunBeforeUnloadDialog(
      WebContents* web_contents,
      const base::string16& message_text,
      bool is_reload,
      const DialogClosedCallback& callback) override;

  void CancelActiveAndPendingDialogs(
      WebContents* web_contents) override;

  // Called by the ShellJavaScriptDialog when it closes.
  void DialogClosed(ShellJavaScriptDialog* dialog);
  void WebContentsDestroyed(WebContents* web_contents) override;

  // Used for content_browsertests.
  void set_dialog_request_callback(
      base::Callback<void()> dialog_request_callback) {
    dialog_request_callback_ = dialog_request_callback;
  }

 private:
#if defined(OS_MACOSX) || defined(OS_WIN) || defined(TOOLKIT_GTK)
  // The dialog being shown. No queueing.
  scoped_ptr<ShellJavaScriptDialog> dialog_;
#else
  // TODO: implement ShellJavaScriptDialog for other platforms, drop this #if
#endif

  base::Callback<void()> dialog_request_callback_;

  DISALLOW_COPY_AND_ASSIGN(ShellJavaScriptDialogCreator);
};

}  // namespace content

#endif  // CONTENT_SHELL_SHELL_JAVASCRIPT_DIALOG_CREATOR_H_
