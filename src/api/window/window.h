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

#ifndef CONTENT_NW_SRC_API_WINDOW_WINDOW_H_
#define CONTENT_NW_SRC_API_WINDOW_WINDOW_H_

#include "base/compiler_specific.h"
#include "base/values.h"
#include "chrome/browser/net/chrome_cookie_notification_details.h"
#include "content/nw/src/api/base/base.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/notification_registrar.h"
#include "content/public/browser/render_process_host.h"
#include "net/cookies/canonical_cookie.h"
#include "net/url_request/url_request_context_getter.h"
#include "url/gurl.h"


namespace content {
class Shell;
class ShellBrowserContext;
}

namespace nwapi {

class CookieAPIContext : public base::RefCountedThreadSafe<CookieAPIContext> {
public:
  CookieAPIContext(DispatcherHost* dispatcher_host,
                     const base::ListValue& arguments);

  net::URLRequestContextGetter* store_context_;
  scoped_ptr<base::DictionaryValue> details_;
  scoped_ptr<base::ListValue> result_;
  GURL url_;
  int req_id_;
  bool success_;
private:
  friend class base::RefCountedThreadSafe<CookieAPIContext>;
  ~CookieAPIContext();
};


class Window : public Base, public content::NotificationObserver {
 public:
  Window(int id,
         const base::WeakPtr<DispatcherHost>& dispatcher_host,
         const base::DictionaryValue& option);
  virtual ~Window();

  virtual void Call(const std::string& method,
                    const base::ListValue& arguments) OVERRIDE;
  virtual void CallSync(const std::string& method,
                        const base::ListValue& arguments,
                        base::ListValue* result) OVERRIDE;

  void CookieGet(const base::ListValue& arguments, bool get_all = false);
  void GetCookieOnIOThread(CookieAPIContext*);
  void GetAllCookieOnIOThread(CookieAPIContext*);
  void GetCookieCallback(CookieAPIContext*, const net::CookieList& cookie_list);
  void GetAllCookieCallback(CookieAPIContext*, const net::CookieList& cookie_list);
  void RespondOnUIThread(CookieAPIContext*);
  void RemoveCookieCallback(CookieAPIContext* api_context);
  void RemoveCookieOnIOThread(CookieAPIContext*);
  void CookieRemove(const base::ListValue& arguments);
  void CookieSet(const base::ListValue& arguments);
  void PullCookieCallback(CookieAPIContext* api_context,
                            const net::CookieList& cookie_list);
  void PullCookie(CookieAPIContext* api_context, bool set_cookie_result);
  void SetCookieOnIOThread(CookieAPIContext* api_context);

 private:
  // content::NotificationObserver implementation.
  virtual void Observe(int type,
                       const content::NotificationSource& source,
                       const content::NotificationDetails& details) OVERRIDE;

  // Handler for the COOKIE_CHANGED event. The method takes the details of such
  // an event and constructs a suitable JSON formatted extension event from it.
  void CookieChanged(content::ShellBrowserContext*, ChromeCookieDetails* details);

  content::Shell* shell_;
  content::NotificationRegistrar registrar_;

  DISALLOW_COPY_AND_ASSIGN(Window);
};

}  // namespace nwapi

#endif  // CONTENT_NW_SRC_API_WINDOW_WINDOW_H_
