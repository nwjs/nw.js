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

#include <deque>
#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "base/strings/string16.h"
#include "base/synchronization/lock.h"
#include "content/public/browser/notification_observer.h"
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

namespace content {
class RenderViewHostDelegate;
class NotificationRegistrar;
}  // namespace content

namespace net {
class AuthChallengeInfo;
class URLRequest;
}

namespace content {

// This class provides a dialog box to ask the user for credentials. Useful in
// ResourceDispatcherHostDelegate::CreateLoginDelegate.
class ShellLoginDialog : public ResourceDispatcherHostLoginDelegate,
                         public content::NotificationObserver
{
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

  // Implements the content::NotificationObserver interface.
  // Listens for AUTH_SUPPLIED and AUTH_CANCELLED notifications from other
  // LoginHandlers so that this LoginHandler has the chance to dismiss itself
  // if it was waiting for the same authentication.
  virtual void Observe(int type,
                       const content::NotificationSource& source,
                       const content::NotificationDetails& details) OVERRIDE;

  // Who/where/what asked for the authentication.
  const net::AuthChallengeInfo* auth_info() const { return auth_info_.get(); }

  // Returns whether authentication had been handled (SetAuth or CancelAuth).
  bool WasAuthHandled() const;

  // Performs necessary cleanup before deletion.
  void ReleaseSoon();

 protected:
  // Threading: any
  virtual ~ShellLoginDialog();

 private:

  // popup next dialog in queue on closing of this dialog
  void HandleQueueOnClose();

  // Calls SetAuth from the IO loop.
  void SetAuthDeferred(const string16& username,
                       const string16& password);

  // Calls CancelAuth from the IO loop.
  void CancelAuthDeferred();

  // Closes the view_contents from the UI loop.
  void CloseContentsDeferred();

  // Marks authentication as handled and returns the previous handled
  // state.
  bool TestAndSetAuthHandled();

  // Notify observers that authentication is supplied.
  void NotifyAuthSupplied(const string16& username,
                          const string16& password);

  void NotifyAuthCancelled();
  // All the methods that begin with Platform need to be implemented by the
  // platform specific LoginDialog implementation.
  // Creates the dialog.
  // Threading: UI thread.
  void PlatformCreateDialog(const string16& message);

  void PlatformShowDialog();
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

  // Starts observing notifications from other LoginHandlers.
  void AddObservers();

  // Stops observing notifications from other LoginHandlers.
  void RemoveObservers();

  // Who/where/what asked for the authentication.
  // Threading: IO thread.
  scoped_refptr<net::AuthChallengeInfo> auth_info_;

  // The request that wants login data.
  // Threading: IO thread.
  net::URLRequest* request_;

  // True if we've handled auth (SetAuth or CancelAuth has been called).
  bool handled_auth_;
  mutable base::Lock handled_auth_lock_;

  // Observes other login handlers so this login handler can respond.
  // This is only accessed on the UI thread.
  scoped_ptr<content::NotificationRegistrar> registrar_;

  typedef std::deque<ShellLoginDialog*> ShellLoginDialogList;

  static ShellLoginDialogList dialog_queue_;

#if defined(OS_MACOSX)
  // Threading: UI thread.
  ShellLoginDialogHelper* helper_;  // owned
  base::string16 message_;
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
  CHROMEGTK_CALLBACK_0(ShellLoginDialog, void, OnDestroy);
#endif
};

// Details to provide the content::NotificationObserver.  Used by the automation
// proxy for testing.
class LoginNotificationDetails {
 public:
  explicit LoginNotificationDetails(ShellLoginDialog* handler)
      : handler_(handler) {}
  ShellLoginDialog* handler() const { return handler_; }

 private:
  LoginNotificationDetails() {}

  ShellLoginDialog* handler_;  // Where to send the response.

  DISALLOW_COPY_AND_ASSIGN(LoginNotificationDetails);
};


// Details to provide the NotificationObserver.  Used by the automation proxy
// for testing and by other LoginHandlers to dismiss themselves when an
// identical auth is supplied.
class AuthSuppliedLoginNotificationDetails : public LoginNotificationDetails {
 public:
  AuthSuppliedLoginNotificationDetails(ShellLoginDialog* handler,
                                       const string16& username,
                                       const string16& password)
      : LoginNotificationDetails(handler),
        username_(username),
        password_(password) {}
  const string16& username() const { return username_; }
  const string16& password() const { return password_; }

 private:
  // The username that was used for the authentication.
  const string16 username_;

  // The password that was used for the authentication.
  const string16 password_;

  DISALLOW_COPY_AND_ASSIGN(AuthSuppliedLoginNotificationDetails);
};

}  // namespace content

#endif  // CONTENT_NW_SRC_BROWSER_SHELL_LOGIN_DIALOG_H_
