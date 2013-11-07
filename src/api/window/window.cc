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

#include "content/nw/src/api/window/window.h"

#include "base/values.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "content/nw/src/api/dispatcher_host.h"
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/browser/native_window.h"
#include "content/nw/src/nw_shell.h"
#include "content/nw/src/shell_browser_context.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_view_host.h"
#include "net/cookies/canonical_cookie.h"
#include "net/cookies/cookie_constants.h"
#include "net/cookies/cookie_monster.h"
#include "net/cookies/cookie_util.h"
#include "net/url_request/url_request_context.h"
#include "url/gurl.h"

using content::BrowserThread;

namespace {

void GetCookieListFromStore(
    net::CookieStore* cookie_store, const GURL& url,
    const net::CookieMonster::GetCookieListCallback& callback) {
  DCHECK(cookie_store);
  net::CookieMonster* monster = cookie_store->GetCookieMonster();
  if (!url.is_empty()) {
    DCHECK(url.is_valid());
    monster->GetAllCookiesForURLAsync(url, callback);
  } else {
    monster->GetAllCookiesAsync(callback);
  }
}

bool MatchesDomain(base::DictionaryValue* details, const std::string& domain) {
  std::string val;
  if (!details->GetString("domain", &val))
    return true;

  // Add a leading '.' character to the filter domain if it doesn't exist.
  if (net::cookie_util::DomainIsHostOnly(val))
    val.insert(0, ".");

  std::string sub_domain(domain);
  // Strip any leading '.' character from the input cookie domain.
  if (!net::cookie_util::DomainIsHostOnly(sub_domain))
    sub_domain = sub_domain.substr(1);

  // Now check whether the domain argument is a subdomain of the filter domain.
  for (sub_domain.insert(0, ".");
       sub_domain.length() >= val.length();) {
    if (sub_domain == val)
      return true;
    const size_t next_dot = sub_domain.find('.', 1);  // Skip over leading dot.
    sub_domain.erase(0, next_dot);
  }
  return false;
}

bool MatchesCookie(base::DictionaryValue* details,
                     const net::CanonicalCookie& cookie) {
  std::string val;

  bool flag;
  if (details->GetString("name", &val))
    if (val != cookie.Name())
      return false;

  if (!MatchesDomain(details, cookie.Domain()))
      return false;

  if (details->GetString("path", &val))
    if (val != cookie.Path())
      return false;

  if (details->GetBoolean("secure", &flag))
    if (flag != cookie.IsSecure())
      return false;

  if (details->GetBoolean("session", &flag))
    if (flag != cookie.IsPersistent())
      return false;

  return true;
}

base::DictionaryValue*
PopulateCookieObject(const net::CanonicalCookie& canonical_cookie) {

  base::DictionaryValue* result = new base::DictionaryValue();
  // A cookie is a raw byte sequence. By explicitly parsing it as UTF-8, we
  // apply error correction, so the string can be safely passed to the renderer.
  result->SetString("name", UTF16ToUTF8(UTF8ToUTF16(canonical_cookie.Name())));
  result->SetString("value", UTF16ToUTF8(UTF8ToUTF16(canonical_cookie.Value())));
  result->SetString("domain", canonical_cookie.Domain());
  result->SetBoolean("host_only", net::cookie_util::DomainIsHostOnly(
                                                                      canonical_cookie.Domain()));
  // A non-UTF8 path is invalid, so we just replace it with an empty string.
  result->SetString("path", IsStringUTF8(canonical_cookie.Path()) ? canonical_cookie.Path()
                     : std::string());
  result->SetBoolean("secure", canonical_cookie.IsSecure());
  result->SetBoolean("http_only", canonical_cookie.IsHttpOnly());
  result->SetBoolean("session", !canonical_cookie.IsPersistent());
  if (canonical_cookie.IsPersistent()) {
    result->SetDouble("expiration_date",
                       canonical_cookie.ExpiryDate().ToDoubleT());
  }
  return result;
}

} // namespace

namespace api {

Window::Window(int id,
               DispatcherHost* dispatcher_host,
               const base::DictionaryValue& option)
    : Base(id, dispatcher_host, option),
      shell_(content::Shell::FromRenderViewHost(dispatcher_host->
           render_view_host())) {
  DVLOG(1) << "Window::Window(" << id << ")";
  // Set ID for Shell
  shell_->set_id(id);

}

Window::~Window() {
  // Window object got deleted when we launch new render view host and
  // delete the old one; at this time the Shell should be decoupled
  // with the renderer side
  DVLOG(1) << "Window::~Window(" << shell_->id() << ")";
  shell_->set_id(-1);
}

void Window::Call(const std::string& method,
                  const base::ListValue& arguments) {
  if (method == "Show") {
    shell_->window()->Show();
  } else if (method == "Close") {
    bool force = false;
    arguments.GetBoolean(0, &force);
    shell_->set_force_close(force);
    shell_->window()->Close();
  } else if (method == "Hide") {
    shell_->window()->Hide();
  } else if (method == "Maximize") {
    shell_->window()->Maximize();
  } else if (method == "Unmaximize") {
    shell_->window()->Unmaximize();
  } else if (method == "Minimize") {
    shell_->window()->Minimize();
  } else if (method == "Restore") {
    shell_->window()->Restore();
  } else if (method == "EnterFullscreen") {
    shell_->window()->SetFullscreen(true);
  } else if (method == "LeaveFullscreen") {
    shell_->window()->SetFullscreen(false);
  } else if (method == "ToggleFullscreen") {
    shell_->window()->SetFullscreen(!shell_->window()->IsFullscreen());
  } else if (method == "EnterKioskMode") {
    shell_->window()->SetKiosk(true);
  } else if (method == "LeaveKioskMode") {
    shell_->window()->SetKiosk(false);
  } else if (method == "ToggleKioskMode") {
    shell_->window()->SetKiosk(!shell_->window()->IsKiosk());
  } else if (method == "ShowDevTools") {
    std::string jail_id;
    bool headless = false;
    arguments.GetString(0, &jail_id);
    arguments.GetBoolean(1, &headless);
    shell_->ShowDevTools(jail_id.c_str(), headless);
  } else if (method == "CloseDevTools") {
    shell_->CloseDevTools();
  }else if (method == "ResizeTo") {
    int width, height;
    if (arguments.GetInteger(0, &width) &&
        arguments.GetInteger(1, &height))
      shell_->window()->SetSize(gfx::Size(width, height));
  } else if (method == "SetMaximumSize") {
    int width, height;
    if (arguments.GetInteger(0, &width) &&
        arguments.GetInteger(1, &height))
      shell_->window()->SetMaximumSize(width, height);
  } else if (method == "SetMinimumSize") {
    int width, height;
    if (arguments.GetInteger(0, &width) &&
        arguments.GetInteger(1, &height))
      shell_->window()->SetMinimumSize(width, height);
  } else if (method == "SetResizable") {
    bool resizable;
    if (arguments.GetBoolean(0, &resizable))
      shell_->window()->SetResizable(resizable);
  } else if (method == "SetAlwaysOnTop") {
    bool top;
    if (arguments.GetBoolean(0, &top))
      shell_->window()->SetAlwaysOnTop(top);
  } else if (method == "MoveTo") {
    int x, y;
    if (arguments.GetInteger(0, &x) &&
        arguments.GetInteger(1, &y))
      shell_->window()->SetPosition(gfx::Point(x, y));
  } else if (method == "RequestAttention") {
    bool flash;
    if (arguments.GetBoolean(0, &flash))
      shell_->window()->FlashFrame(flash);
  } else if (method == "SetMenu") {
    int id;
    if (arguments.GetInteger(0, &id))
      shell_->window()->SetMenu(dispatcher_host()->GetApiObject<Menu>(id));
  } else if (method == "Reload") {
    int type;
    if (arguments.GetInteger(0, &type))
      shell_->Reload(static_cast<content::Shell::ReloadType>(type));
  } else if (method == "CapturePage") {
    std::string image_format_str;
    if (arguments.GetString(0, &image_format_str))
      shell_->window()->CapturePage(image_format_str);
  } else if (method == "CookieGet") {
    CookieGet(arguments);
  } else if (method == "CookieGetAll") {
    CookieGet(arguments, true);
  } else {
    NOTREACHED() << "Invalid call to Window method:" << method
                 << " arguments:" << arguments;
  }
}

void Window::CallSync(const std::string& method,
                      const base::ListValue& arguments,
                      base::ListValue* result) {
  if (method == "IsFullscreen") {
    result->AppendBoolean(shell_->window()->IsFullscreen());
  } else if (method == "IsKioskMode") {
    result->AppendBoolean(shell_->window()->IsKiosk());
  } else if (method == "GetSize") {
    gfx::Size size = shell_->window()->GetSize();
    result->AppendInteger(size.width());
    result->AppendInteger(size.height());
  } else if (method == "GetPosition") {
    gfx::Point position = shell_->window()->GetPosition();
    result->AppendInteger(position.x());
    result->AppendInteger(position.y());
  } else if (method == "IsDevToolsOpen") {
    result->AppendBoolean(shell_->devToolsOpen());
  } else {
    NOTREACHED() << "Invalid call to Window method:" << method
                 << " arguments:" << arguments;
  }
}


void Window::CookieGet(const base::ListValue& arguments, bool get_all) {
  content::RenderProcessHost* render_process_host =
    dispatcher_host()->render_view_host()->GetProcess();
  net::URLRequestContextGetter* context_getter =
    render_process_host->GetBrowserContext()->
    GetRequestContextForRenderProcess(render_process_host->GetID());

  const base::DictionaryValue* details = NULL;
  std::string url;

  CookieAPIContext* api_context = new CookieAPIContext;
  api_context->store_context_ = context_getter;
  arguments.GetInteger(0, &api_context->req_id_);
  arguments.GetDictionary(1, &details);
  if (details) {
    api_context->details_.reset(details->DeepCopyWithoutEmptyChildren());
    details->GetString("url", &url);
  }

  api_context->url_ = GURL(url);
  api_context->result_.reset(new base::ListValue);

  if (get_all) {
    bool rv = BrowserThread::PostTask(
                                      BrowserThread::IO, FROM_HERE,
                                      base::Bind(&Window::GetAllCookieOnIOThread,
                                                 base::Unretained(this),
                                                 make_scoped_refptr(api_context)));
    DCHECK(rv);
  }else{
    bool rv = BrowserThread::PostTask(
                                      BrowserThread::IO, FROM_HERE,
                                      base::Bind(&Window::GetCookieOnIOThread,
                                                 base::Unretained(this),
                                                 make_scoped_refptr(api_context)));
    DCHECK(rv);
  }
}

void Window::GetAllCookieOnIOThread(CookieAPIContext* api_context) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  net::CookieStore* cookie_store =
    api_context->store_context_->GetURLRequestContext()->cookie_store();
  GetCookieListFromStore(
      cookie_store, api_context->url_,
      base::Bind(&Window::GetAllCookieCallback, base::Unretained(this),
                 make_scoped_refptr(api_context)));
}

void Window::GetCookieOnIOThread(CookieAPIContext* api_context) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  net::CookieStore* cookie_store =
    api_context->store_context_->GetURLRequestContext()->cookie_store();
  GetCookieListFromStore(
      cookie_store, api_context->url_,
      base::Bind(&Window::GetCookieCallback, base::Unretained(this),
                 make_scoped_refptr(api_context)));
}

void Window::GetAllCookieCallback(CookieAPIContext* api_context,
                                  const net::CookieList& cookie_list) {
  net::CookieList::const_iterator it;
  api_context->result_->Clear();
  for (it = cookie_list.begin(); it != cookie_list.end(); ++it) {
    if (MatchesCookie(api_context->details_.get(), *it)) {
      api_context->result_->Append(PopulateCookieObject(*it));
    }
  }

  bool rv = BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&Window::RespondOnUIThread, base::Unretained(this),
                 make_scoped_refptr(api_context)));
  DCHECK(rv);
}

void Window::GetCookieCallback(CookieAPIContext* api_context,
                               const net::CookieList& cookie_list) {
  net::CookieList::const_iterator it;
  std::string name;
  api_context->details_->GetString("name", &name);

  api_context->result_->Clear();

  for (it = cookie_list.begin(); it != cookie_list.end(); ++it) {
    // Return the first matching cookie. Relies on the fact that the
    // CookieMonster returns them in canonical order (longest path, then
    // earliest creation time).

    if (it->Name() == name) {
      api_context->result_->Append(PopulateCookieObject(*it));
      break;
    }
  }

  bool rv = BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&Window::RespondOnUIThread, base::Unretained(this),
                 make_scoped_refptr(api_context)));
  DCHECK(rv);
}

void Window::RespondOnUIThread(CookieAPIContext* api_context) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  base::ListValue ret;
  ret.Append(api_context->result_.release());
  dispatcher_host()->SendEvent(this, base::StringPrintf("__nw_gotcookie%d", api_context->req_id_), ret);
}

}  // namespace api
