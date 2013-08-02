// Copyright (c) 2012 Intel Corp
// Copyright (c) 2012 The Chromium Authors
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell co
// pies of the Software, and to permit persons to whom the Software is furnished
//  to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in al
// l copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IM
// PLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNES
// S FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
//  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WH
// ETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef CONTENT_NW_SRC_BROWSER_SHELL_LOGIN_DIALOG_H_
#define CONTENT_NW_SRC_BROWSER_SHELL_LOGIN_DIALOG_H_

#include "base/compiler_specific.h"
#include "base/strings/string16.h"
#include "content/public/browser/resource_dispatcher_host_login_delegate.h"

#if defined(TOOLKIT_GTK)
#include "ui/base/gtk/gtk_signal.h"
#endif

#if defined(OS_MACOSX)
#if __OBJC__
@class ShellLoginDialogHelper;
#else
class ShellLoginDialogHelper;
#endif  // __OBJC__
#endif  // defined(OS_MACOSX)

namespace net {
class AuthChallengeInfo;
class URLRequest;
}

namespace content {

// This class provides a dialog box to ask the user for credentials. Useful in
// ResourceDispatcherHostDelegate::CreateLoginDelegate.
class ShellLoginDialog : public ResourceDispatcherHostLoginDelegate {
 public:
  // Threading: IO thread.
  ShellLoginDialog(net::AuthChallengeInfo* auth_info, net::URLRequest* request);

  // ResourceDispatcherHostLoginDelegate implementation:
  // Threading: IO thread.
  virtual void OnRequestCancelled() OVERRIDE;

  // Called by the platform specific code when the user responds. Public because
  // the aforementioned platform specific code may not have access to private
  // members. Not to be called from client code.
  // Threading: UI thread.
  void UserAcceptedAuth(const string16& username, const string16& password);
  void UserCancelledAuth();

 protected:
  // Threading: any
  virtual ~ShellLoginDialog();

 private:
  // All the methods that begin with Platform need to be implemented by the
  // platform specific LoginDialog implementation.
  // Creates the dialog.
  // Threading: UI thread.
  void PlatformCreateDialog(const string16& message);
  // Called from the destructor to let each platform do any necessary cleanup.
  // Threading: UI thread.
  void PlatformCleanUp();
  // Called from OnRequestCancelled if the request was cancelled.
  // Threading: UI thread.
  void PlatformRequestCancelled();

  // Sets up dialog creation.
  // Threading: UI thread.
  void PrepDialog(const string16& host, const string16& realm);

  // Sends the authentication to the requester.
  // Threading: IO thread.
  void SendAuthToRequester(bool success,
                           const string16& username,
                           const string16& password);

  // Who/where/what asked for the authentication.
  // Threading: IO thread.
  scoped_refptr<net::AuthChallengeInfo> auth_info_;

  // The request that wants login data.
  // Threading: IO thread.
  net::URLRequest* request_;

#if defined(OS_MACOSX)
  // Threading: UI thread.
  ShellLoginDialogHelper* helper_;  // owned
#elif defined(OS_WIN)
  HWND dialog_win_;
  string16 message_text_;
  static INT_PTR CALLBACK DialogProc(HWND dialog, UINT message, WPARAM wparam,
                                     LPARAM lparam);
#elif defined(TOOLKIT_GTK)
  GtkWidget* username_entry_;
  GtkWidget* password_entry_;
  GtkWidget* root_;
  CHROMEGTK_CALLBACK_1(ShellLoginDialog, void, OnResponse, int);
#endif
};

}  // namespace content

#endif  // CONTENT_NW_SRC_BROWSER_SHELL_LOGIN_DIALOG_H_
