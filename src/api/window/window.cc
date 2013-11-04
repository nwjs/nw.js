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


void Window::CookieGet(const base::ListValue& arguments) {
  content::RenderProcessHost* render_process_host =
    dispatcher_host()->render_view_host()->GetProcess();
  net::URLRequestContextGetter* context_getter =
    render_process_host->GetBrowserContext()->
    GetRequestContextForRenderProcess(render_process_host->GetID());

  const base::DictionaryValue* details = NULL;
  std::string url;

  store_context_ = context_getter;
  if (details) {
    details_.reset(details->DeepCopyWithoutEmptyChildren());
    details->GetString("url", &url);
  }
  url_ = GURL(url);

  bool rv = BrowserThread::PostTask(
      BrowserThread::IO, FROM_HERE,
      base::Bind(&Window::GetCookieOnIOThread, base::Unretained(this)));
  DCHECK(rv);
}

void Window::GetCookieOnIOThread() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  net::CookieStore* cookie_store =
      store_context_->GetURLRequestContext()->cookie_store();
  GetCookieListFromStore(
      cookie_store, url_,
      base::Bind(&Window::GetCookieCallback, base::Unretained(this)));
}

void Window::GetCookieCallback(const net::CookieList& cookie_list) {
  net::CookieList::const_iterator it;
  std::string name;
  details_->GetString("name", &name);

  if (!result_)
    result_.reset(new base::DictionaryValue);
  result_->Clear();

  for (it = cookie_list.begin(); it != cookie_list.end(); ++it) {
    // Return the first matching cookie. Relies on the fact that the
    // CookieMonster returns them in canonical order (longest path, then
    // earliest creation time).

    if (it->Name() == name) {
      const net::CanonicalCookie& canonical_cookie = *it;
      // A cookie is a raw byte sequence. By explicitly parsing it as UTF-8, we
      // apply error correction, so the string can be safely passed to the renderer.
      result_->SetString("name", UTF16ToUTF8(UTF8ToUTF16(canonical_cookie.Name())));
      result_->SetString("value", UTF16ToUTF8(UTF8ToUTF16(canonical_cookie.Value())));
      result_->SetString("domain", canonical_cookie.Domain());
      result_->SetBoolean("host_only", net::cookie_util::DomainIsHostOnly(
                                                                          canonical_cookie.Domain()));
      // A non-UTF8 path is invalid, so we just replace it with an empty string.
      result_->SetString("path", IsStringUTF8(canonical_cookie.Path()) ? canonical_cookie.Path()
                         : std::string());
      result_->SetBoolean("secure", canonical_cookie.IsSecure());
      result_->SetBoolean("http_only", canonical_cookie.IsHttpOnly());
      result_->SetBoolean("session", !canonical_cookie.IsPersistent());
      if (canonical_cookie.IsPersistent()) {
        result_->SetDouble("expiration_date",
                           canonical_cookie.ExpiryDate().ToDoubleT());
      }
      break;
    }
  }

  bool rv = BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&Window::RespondOnUIThread, base::Unretained(this)));
  DCHECK(rv);
}

void Window::RespondOnUIThread() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  base::ListValue args;
  args.Append(result_.release());
  dispatcher_host()->SendEvent(this, "__nw_gotcookie", args);
}

}  // namespace api
