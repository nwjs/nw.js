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
#include "chrome/browser/chrome_notification_types.h"
#include "content/nw/src/api/dispatcher_host.h"
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/browser/native_window.h"
#include "content/nw/src/nw_shell.h"
#include "content/nw/src/shell_browser_context.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/common/url_constants.h"
#include "net/cookies/canonical_cookie.h"
#include "net/cookies/cookie_constants.h"
#include "net/cookies/cookie_monster.h"
#include "net/cookies/cookie_util.h"
#include "net/url_request/url_request_context.h"
#include "url/gurl.h"

using content::BrowserThread;
using content::ShellBrowserContext;

namespace {

const char kCauseKey[] = "cause";
const char kCookieKey[] = "cookie";
const char kDomainKey[] = "domain";
const char kIdKey[] = "id";
const char kRemovedKey[] = "removed";
const char kTabIdsKey[] = "tabIds";

// Cause Constants
const char kEvictedChangeCause[] = "evicted";
const char kExpiredChangeCause[] = "expired";
const char kExpiredOverwriteChangeCause[] = "expired_overwrite";
const char kExplicitChangeCause[] = "explicit";
const char kOverwriteChangeCause[] = "overwrite";

GURL GetURLFromCanonicalCookie(const net::CanonicalCookie& cookie) {
  const std::string& domain_key = cookie.Domain();
  const std::string scheme =
    cookie.IsSecure() ? "https" : "http";
  const std::string host =
      domain_key.find('.') != 0 ? domain_key : domain_key.substr(1);
  return GURL(scheme + content::kStandardSchemeSeparator + host + "/");
}

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
  result->SetString("name", base::UTF16ToUTF8(base::UTF8ToUTF16(canonical_cookie.Name())));
  result->SetString("value", base::UTF16ToUTF8(base::UTF8ToUTF16(canonical_cookie.Value())));
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

namespace nwapi {

CookieAPIContext::~CookieAPIContext() {}

Window::Window(int id,
               const base::WeakPtr<DispatcherHost>& dispatcher_host,
               const base::DictionaryValue& option)
    : Base(id, dispatcher_host, option),
      shell_(content::Shell::FromRenderViewHost(dispatcher_host->
           render_view_host())) {
  DVLOG(1) << "Window::Window(" << id << ")";
  // Set ID for Shell
  shell_->set_id(id);
  CHECK(registrar_.IsEmpty());
  registrar_.Add(this,
                 chrome::NOTIFICATION_COOKIE_CHANGED,
                 content::NotificationService::AllBrowserContextsAndSources());
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
  } else if (method == "Focus") {
    shell_->window()->Focus(true);
  } else if (method == "Blur") {
    shell_->window()->Focus(false);
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
  } else if (method == "SetShowInTaskbar" ) {
    bool show;
    if (arguments.GetBoolean(0, &show))
      shell_->window()->SetShowInTaskbar(show);
  } else if (method == "MoveTo") {
    int x, y;
    if (arguments.GetInteger(0, &x) &&
        arguments.GetInteger(1, &y))
      shell_->window()->SetPosition(gfx::Point(x, y));
  } else if (method == "RequestAttention") {
    int count;
    if (arguments.GetInteger(0, &count))
      shell_->window()->FlashFrame(count);
  } else if (method == "SetBadgeLabel") {
    std::string label;
    if (arguments.GetString(0, &label))
      shell_->window()->SetBadgeLabel(label);
  } else if (method == "SetProgressBar") {
    double progress;
    if (arguments.GetDouble(0, &progress))
      shell_->window()->SetProgressBar(progress);
  } else if (method == "SetMenu") {
    int id;
    if (arguments.GetInteger(0, &id))
      shell_->window()->SetMenu(dispatcher_host()->GetApiObject<Menu>(id));
  } else if (method == "ClearMenu") {
    shell_->window()->ClearMenu();
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
  } else if (method == "CookieRemove") {
    CookieRemove(arguments);
  } else if (method == "CookieSet") {
    CookieSet(arguments);
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
  } else if (method == "ShowDevTools") {
    std::string jail_id;
    bool headless = false;
    arguments.GetString(0, &jail_id);
    arguments.GetBoolean(1, &headless);
    shell_->ShowDevTools(jail_id.c_str(), headless);
    int object_id = 0;
    if (!headless)
      object_id = shell_->WrapDevToolsWindow();
    result->AppendInteger(object_id);
  } else {
    NOTREACHED() << "Invalid call to Window method:" << method
                 << " arguments:" << arguments;
  }
}

void Window::CookieRemove(const base::ListValue& arguments) {
  if (!dispatcher_host())
    return;
  CookieAPIContext* api_context = new CookieAPIContext(dispatcher_host(), arguments);
  bool rv = BrowserThread::PostTask(
                                    BrowserThread::IO, FROM_HERE,
                                    base::Bind(&Window::RemoveCookieOnIOThread,
                                               base::Unretained(this),
                                               make_scoped_refptr(api_context)));
  DCHECK(rv);
}

void Window::CookieSet(const base::ListValue& arguments) {
  if (!dispatcher_host())
    return;
  CookieAPIContext* api_context = new CookieAPIContext(dispatcher_host(), arguments);
  bool rv = BrowserThread::PostTask(
                                    BrowserThread::IO, FROM_HERE,
                                    base::Bind(&Window::SetCookieOnIOThread,
                                               base::Unretained(this),
                                               make_scoped_refptr(api_context)));
  DCHECK(rv);
}

void Window::RemoveCookieOnIOThread(CookieAPIContext* api_context) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));

  // Remove the cookie
  net::CookieStore* cookie_store =
    api_context->store_context_->GetURLRequestContext()->cookie_store();
  std::string name;
  api_context->details_->GetString("name", &name);
  cookie_store->DeleteCookieAsync(
      api_context->url_, name,
      base::Bind(&Window::RemoveCookieCallback, base::Unretained(this),
                 make_scoped_refptr(api_context)));
}

void Window::RemoveCookieCallback(CookieAPIContext* api_context) {
  std::string name;
  api_context->details_->GetString("name", &name);

  base::DictionaryValue* result = new base::DictionaryValue();
  result->SetString("name", name);
  result->SetString("url", api_context->url_.spec());
  api_context->result_->Append(result);

  // Return to UI thread
  bool rv = BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&Window::RespondOnUIThread, base::Unretained(this),
                 make_scoped_refptr(api_context)));
  DCHECK(rv);
}

CookieAPIContext::CookieAPIContext(DispatcherHost* dispatcher_host,
                                   const base::ListValue& arguments) {
  content::RenderProcessHost* render_process_host =
    dispatcher_host->render_view_host()->GetProcess();
  net::URLRequestContextGetter* context_getter =
    render_process_host->GetBrowserContext()->
    GetRequestContextForRenderProcess(render_process_host->GetID());

  const base::DictionaryValue* details = NULL;
  std::string url;

  store_context_ = context_getter;
  arguments.GetInteger(0, &req_id_);
  arguments.GetDictionary(1, &details);
  if (details) {
    details_.reset(details->DeepCopyWithoutEmptyChildren());
    details->GetString("url", &url);
  }

  url_ = GURL(url);
  result_.reset(new base::ListValue);
}

void Window::CookieGet(const base::ListValue& arguments, bool get_all) {

  if (!dispatcher_host())
    return;
  CookieAPIContext* api_context = new CookieAPIContext(dispatcher_host(), arguments);

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
  if (!dispatcher_host())
    return;
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  base::ListValue ret;
  ret.Append(api_context->result_.release());
  dispatcher_host()->SendEvent(this, base::StringPrintf("__nw_gotcookie%d", api_context->req_id_), ret);
}

void Window::SetCookieOnIOThread(CookieAPIContext* api_context) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  net::CookieMonster* cookie_monster =
    api_context->store_context_->GetURLRequestContext()->cookie_store()->
      GetCookieMonster();

  base::Time expiration_time;
  double expiration_date;

  if (api_context->details_->GetDouble("expirationDate", &expiration_date)) {
    // Time::FromDoubleT converts double time 0 to empty Time object. So we need
    // to do special handling here.
    expiration_time = (expiration_date == 0) ?
      base::Time::UnixEpoch() :
      base::Time::FromDoubleT(expiration_date);
  }

  const base::DictionaryValue* details = api_context->details_.get();
  std::string name, value, domain, path;
  details->GetString("name",   &name);
  details->GetString("value",  &value);
  details->GetString("domain", &domain);
  details->GetString("path",   &path);

  bool secure = false, http_only = false;
  details->GetBoolean("httpOnly", &http_only);
  details->GetBoolean("secure",   &secure);

  cookie_monster->SetCookieWithDetailsAsync(
      api_context->url_,
      name, value, domain, path,
      expiration_time,
      secure, http_only,
      net::COOKIE_PRIORITY_DEFAULT,
      base::Bind(&Window::PullCookie, base::Unretained(this),
                 make_scoped_refptr(api_context)));
}

void Window::PullCookie(CookieAPIContext* api_context, bool set_cookie_result) {
  // Pull the newly set cookie.
  net::CookieMonster* cookie_monster =
    api_context->store_context_->GetURLRequestContext()->cookie_store()->
      GetCookieMonster();
  api_context->success_ = set_cookie_result;
  GetCookieListFromStore(
      cookie_monster, api_context->url_,
      base::Bind(&Window::PullCookieCallback,
                 base::Unretained(this),
                 make_scoped_refptr(api_context)));
}

void Window::PullCookieCallback(CookieAPIContext* api_context,
    const net::CookieList& cookie_list) {
  net::CookieList::const_iterator it;
  for (it = cookie_list.begin(); it != cookie_list.end(); ++it) {
    // Return the first matching cookie. Relies on the fact that the
    // CookieMonster returns them in canonical order (longest path, then
    // earliest creation time).
    std::string name;
    api_context->details_->GetString("name", &name);
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

void Window::Observe(
    int type,
    const content::NotificationSource& source,
    const content::NotificationDetails& details) {

  ShellBrowserContext* browser_context =
    content::Source<ShellBrowserContext>(source).ptr();

  switch (type) {
    case chrome::NOTIFICATION_COOKIE_CHANGED:
      CookieChanged(
          browser_context,
          content::Details<ChromeCookieDetails>(details).ptr());
      break;

    default:
      NOTREACHED();
  }
}

void Window::CookieChanged(
    ShellBrowserContext* browser_context,
    ChromeCookieDetails* details) {
  if (!dispatcher_host())
    return;
  scoped_ptr<base::ListValue> args(new base::ListValue());
  base::DictionaryValue* dict = new base::DictionaryValue();
  dict->SetBoolean(kRemovedKey, details->removed);
  dict->Set(kCookieKey, PopulateCookieObject(*details->cookie));

  // Map the internal cause to an external string.
  std::string cause;
  switch (details->cause) {
    case net::CookieMonster::Delegate::CHANGE_COOKIE_EXPLICIT:
      cause = kExplicitChangeCause;
      break;

    case net::CookieMonster::Delegate::CHANGE_COOKIE_OVERWRITE:
      cause = kOverwriteChangeCause;
      break;

    case net::CookieMonster::Delegate::CHANGE_COOKIE_EXPIRED:
      cause = kExpiredChangeCause;
      break;

    case net::CookieMonster::Delegate::CHANGE_COOKIE_EVICTED:
      cause = kEvictedChangeCause;
      break;

    case net::CookieMonster::Delegate::CHANGE_COOKIE_EXPIRED_OVERWRITE:
      cause = kExpiredOverwriteChangeCause;
      break;

    default:
      NOTREACHED();
  }
  dict->SetString(kCauseKey, cause);

  args->Append(dict);

  GURL cookie_domain =
      GetURLFromCanonicalCookie(*details->cookie);

  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  dispatcher_host()->SendEvent(this, "__nw_cookie_changed", *args);

}

}  // namespace nwapi
