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

#include "content/nw/src/nw_shell.h"

#include "base/command_line.h"
#include "base/message_loop.h"
#include "base/string_util.h"
#include "base/utf_string_conversions.h"
#include "base/values.h"
#include "content/browser/child_process_security_policy_impl.h"
#include "content/public/browser/devtools_agent_host.h"
#include "content/public/browser/devtools_http_handler.h"
#include "content/public/browser/devtools_manager.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/notification_details.h"
#include "content/public/browser/notification_source.h"
#include "content/public/browser/notification_types.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/renderer_preferences.h"
#include "content/public/common/url_constants.h"
#include "content/nw/src/api/api_messages.h"
#include "content/nw/src/api/app/app.h"
#include "content/nw/src/browser/file_select_helper.h"
#include "content/nw/src/browser/native_window.h"
#include "content/nw/src/browser/shell_devtools_delegate.h"
#include "content/nw/src/browser/shell_javascript_dialog_creator.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/media/media_stream_devices_controller.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/shell_browser_context.h"
#include "content/nw/src/shell_browser_main_parts.h"
#include "content/nw/src/shell_content_browser_client.h"
#include "grit/nw_resources.h"
#include "net/base/escape.h"
#include "ui/base/resource/resource_bundle.h"

#include "content/nw/src/browser/printing/print_view_manager.h"

namespace content {

std::vector<Shell*> Shell::windows_;

bool Shell::quit_message_loop_ = true;

int Shell::exit_code_ = 0;

// static
Shell* Shell::Create(BrowserContext* browser_context,
                     const GURL& url,
                     SiteInstance* site_instance,
                     int routing_id,
                     WebContents* base_web_contents) {
  WebContents::CreateParams create_params(browser_context, site_instance);
  create_params.routing_id = routing_id;
  WebContents* web_contents = WebContents::Create(create_params);

  Shell* shell = new Shell(web_contents, GetPackage()->window());
  NavigationController::LoadURLParams params(url);
  params.transition_type = PAGE_TRANSITION_TYPED;
  params.override_user_agent = NavigationController::UA_OVERRIDE_TRUE;

  web_contents->GetController().LoadURLWithParams(params);

  return shell;
}

// static
Shell* Shell::Create(WebContents* source_contents,
                     const GURL& target_url,
                     base::DictionaryValue* manifest,
                     WebContents* new_contents) {
  Shell* shell = new Shell(new_contents, manifest);

  NavigationController::LoadURLParams params(target_url);
  params.transition_type = PAGE_TRANSITION_TYPED;
  params.override_user_agent = NavigationController::UA_OVERRIDE_TRUE;
  new_contents->GetController().LoadURLWithParams(params);

  // Use the user agent value from the source WebContents.
  std::string source_user_agent =
      source_contents->GetMutableRendererPrefs()->user_agent_override;
  RendererPreferences* prefs = source_contents->GetMutableRendererPrefs();
  prefs->user_agent_override = source_user_agent;

  return shell;
}


Shell* Shell::FromRenderViewHost(RenderViewHost* rvh) {
  for (size_t i = 0; i < windows_.size(); ++i) {
    if (windows_[i]->web_contents() &&
        windows_[i]->web_contents()->GetRenderViewHost() == rvh) {
      return windows_[i];
    }
  }
  return NULL;
}

Shell::Shell(WebContents* web_contents, base::DictionaryValue* manifest)
    : weak_ptr_factory_(this),
      is_devtools_(false),
      force_close_(false),
      id_(-1),
      enable_nodejs_(true)
{
  // Register shell.
  registrar_.Add(this, NOTIFICATION_WEB_CONTENTS_TITLE_UPDATED,
      Source<WebContents>(web_contents));
  registrar_.Add(this, NOTIFICATION_RENDERER_PROCESS_CLOSED,
                 NotificationService::AllBrowserContextsAndSources());
  windows_.push_back(this);

  bool enable_nodejs = true;
  if (manifest->GetBoolean(switches::kNodejs, &enable_nodejs))
    enable_nodejs_ = enable_nodejs;

  // Add web contents.
  web_contents_.reset(web_contents);
  content::WebContentsObserver::Observe(web_contents);
  web_contents_->SetDelegate(this);

  // Create window.
  window_.reset(nw::NativeWindow::Create(this, manifest));

#if defined(ENABLE_PRINTING)
  printing::PrintViewManager::CreateForWebContents(web_contents);
#endif

  // Initialize window after we set window_, because some operations of
  // NativeWindow requires the window_ to be non-NULL.
  window_->InitFromManifest(manifest);
}

Shell::~Shell() {
  SendEvent("closed");

  for (size_t i = 0; i < windows_.size(); ++i) {
    if (windows_[i] == this) {
      windows_.erase(windows_.begin() + i);

      // Close the devtools window if it has one.
      if (devtools_window_)
        devtools_window_->CloseContents(web_contents());
      break;
    }
  }

  if (windows_.empty() && quit_message_loop_)
    api::App::Quit(web_contents()->GetRenderProcessHost());
}

void Shell::SendEvent(const std::string& event, const std::string& arg1) {

  if (id() < 0)
    return;

  DVLOG(1) << "Shell::SendEvent " << event << " id():"
           << id() << " RoutingID: " << web_contents()->GetRoutingID();

  base::ListValue args;
  if (!arg1.empty())
    args.AppendString(arg1);

  web_contents()->GetRenderViewHost()->Send(new ShellViewMsg_Object_On_Event(
      web_contents()->GetRoutingID(), id(), event, args));
}

bool Shell::ShouldCloseWindow() {
  if (id() < 0 || force_close_)
    return true;

  SendEvent("close");
  return false;
}

void Shell::PrintCriticalError(const std::string& title,
                               const std::string& content) {
  const base::StringPiece template_html(
      ResourceBundle::GetSharedInstance().GetRawDataResource(
          IDR_NW_FATAL_ERROR));

  std::string error_page_url;

  if (template_html.empty()) {
    // Print hand written error info if nw.pak doesn't exist.
    NOTREACHED() << "Unable to load error template.";
    error_page_url = "data:text/html;base64,VW5hYmxlIHRvIGZpbmQgbncucGFrLgo=";
  } else {
    std::string content_with_no_newline, content_with_no_space;
    ReplaceChars(net::EscapeForHTML(content),
                 "\n", "<br/>", &content_with_no_newline);
    ReplaceChars(content_with_no_newline,
                 " ", "&nbsp;", &content_with_no_space);

    std::vector<std::string> subst;
    subst.push_back(title);
    subst.push_back(content_with_no_space);
    error_page_url = "data:text/html;charset=utf-8," +
        net::EscapeQueryParamValue(
            ReplaceStringPlaceholders(template_html, subst, NULL), false);
  }

  LoadURL(GURL(error_page_url));
}

nw::Package* Shell::GetPackage() {
  ShellContentBrowserClient* browser_client =
      static_cast<ShellContentBrowserClient*>(GetContentClient()->browser());
  return browser_client->shell_browser_main_parts()->package();
}

void Shell::LoadURL(const GURL& url) {
  web_contents_->GetController().LoadURL(
      url,
      Referrer(),
      PAGE_TRANSITION_TYPED,
      std::string());
  web_contents_->GetView()->Focus();
  window()->SetToolbarButtonEnabled(nw::NativeWindow::BUTTON_FORWARD, false);
}

void Shell::GoBackOrForward(int offset) {
  web_contents_->GetController().GoToOffset(offset);
  web_contents_->GetView()->Focus();
}

void Shell::Reload(ReloadType type) {
  ShellBrowserContext* browser_context =
      static_cast<ShellBrowserContext*>(web_contents_->GetBrowserContext());

  switch (type) {
    case RELOAD:
      web_contents_->GetController().Reload(false);
      break;
    case RELOAD_IGNORING_CACHE:
      web_contents_->GetController().ReloadIgnoringCache(false);
      break;
    case RELOAD_ORIGINAL_REQUEST_URL:
      web_contents_->GetController().ReloadOriginalRequestURL(false);
      break;
    case RELOAD_DEV:
      // ShellContentBrowserClient always tries to use the existing
      // process so we need to overwrite it here
      browser_context->set_pinning_renderer(false);
      web_contents_->GetController().ReloadDev(false);
      browser_context->set_pinning_renderer(true);
      break;
  }

  web_contents_->GetView()->Focus();
}

void Shell::Stop() {
  web_contents_->Stop();
  web_contents_->GetView()->Focus();
}

void Shell::ReloadOrStop() {
  if (web_contents_->IsLoading())
    Stop();
  else
    Reload();
}

#if 0
void Shell::CloseDevTools() {
  if (!devtools_frontend_)
    return;
  registrar_.Remove(this,
                    NOTIFICATION_WEB_CONTENTS_DESTROYED,
                    Source<WebContents>(
                        devtools_frontend_->frontend_shell()->web_contents()));
  devtools_frontend_->Close();
  devtools_frontend_ = NULL;
}
#endif

void Shell::ShowDevTools() {
#if 0
  if (devtools_frontend_) {
    devtools_frontend_->Focus();
    return;
  }
  devtools_frontend_ = ShellDevToolsFrontend::Show(web_contents());
  registrar_.Add(this,
                 NOTIFICATION_WEB_CONTENTS_DESTROYED,
                 Source<WebContents>(
                     devtools_frontend_->frontend_shell()->web_contents()));
#else
  ShellContentBrowserClient* browser_client =
      static_cast<ShellContentBrowserClient*>(
          GetContentClient()->browser());
  // Reuse reopened one if we have.
  if (devtools_window_) {
    devtools_window_->window_->Focus(true);
    return;
  }

  RenderViewHost* inspected_rvh = web_contents()->GetRenderViewHost();
  scoped_refptr<DevToolsAgentHost> agent(DevToolsAgentHost::GetOrCreateFor(inspected_rvh));
  DevToolsManager* manager = DevToolsManager::GetInstance();

  if (agent->IsAttached()) {
    // Break remote debugging debugging session.
    manager->CloseAllClientHosts();
  }

  ShellDevToolsDelegate* delegate =
      browser_client->shell_browser_main_parts()->devtools_delegate();
  GURL url = delegate->devtools_http_handler()->GetFrontendURL(agent.get());

  // Use our minimum set manifest
  base::DictionaryValue manifest;
  manifest.SetBoolean(switches::kmToolbar, false);
  manifest.SetBoolean(switches::kmFullscreen, false);
  manifest.SetInteger(switches::kmWidth, 700);
  manifest.SetInteger(switches::kmHeight, 500);

  ShellBrowserContext* browser_context =
      static_cast<ShellBrowserContext*>(web_contents_->GetBrowserContext());

  browser_context->set_pinning_renderer(false); // opens devtool in
                                                // new renderer
                                                // process or break
                                                // points will stall both
  WebContents::CreateParams create_params(web_contents()->GetBrowserContext(), NULL);
  WebContents* web_contents = WebContents::Create(create_params);
  Shell* shell = new Shell(web_contents, &manifest);
  browser_context->set_pinning_renderer(true);

  int rh_id = shell->web_contents_->GetRenderProcessHost()->GetID();
  ChildProcessSecurityPolicyImpl::GetInstance()->GrantScheme(rh_id, chrome::kFileScheme);
  shell->is_devtools_ = true;
  shell->force_close_ = true;
  shell->LoadURL(url);

  // Save devtools window in current shell.
  devtools_window_ = shell->weak_ptr_factory_.GetWeakPtr();
#endif
}

void Shell::UpdateDraggableRegions(
      const std::vector<extensions::DraggableRegion>& regions) {
  window()->UpdateDraggableRegions(regions);
}

bool Shell::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(Shell, message)
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_UpdateDraggableRegions,
                        UpdateDraggableRegions)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}

WebContents* Shell::OpenURLFromTab(WebContents* source,
                                   const OpenURLParams& params) {
  // The only one we implement for now.
  DCHECK(params.disposition == CURRENT_TAB);
  source->GetController().LoadURL(
      params.url, params.referrer, params.transition, std::string());
  return source;
}

void Shell::LoadingStateChanged(WebContents* source) {
  int current_index = web_contents_->GetController().GetCurrentEntryIndex();
  int max_index = web_contents_->GetController().GetEntryCount() - 1;

  window()->SetToolbarButtonEnabled(nw::NativeWindow::BUTTON_BACK,
                                    current_index > 0);
  window()->SetToolbarButtonEnabled(nw::NativeWindow::BUTTON_FORWARD,
                                    current_index < max_index);
  window()->SetToolbarIsLoading(source->IsLoading());

  DVLOG(1) << "Shell(" << id() << ")::LoadingStateChanged "
           << (source->IsLoading() ? "loading" : "loaded");

  if (source->IsLoading())
    SendEvent("loading");
  else
    SendEvent("loaded");
}

void Shell::ActivateContents(content::WebContents* contents) {
  window()->Focus(true);
}

void Shell::DeactivateContents(content::WebContents* contents) {
  if (windows_.size() == 1) {
    window()->Focus(false);
    return;
  }

  // Move focus to other window
  for (unsigned i = 0; i < windows_.size(); ++i)
    if (windows_[i] != this) {
      windows_[i]->window()->Focus(true);
      break;
    }
}

void Shell::CloseContents(WebContents* source) {
  window()->Close();
}

void Shell::MoveContents(WebContents* source, const gfx::Rect& pos) {
  window()->Move(pos);
}

bool Shell::IsPopupOrPanel(const WebContents* source) const {
  // Treat very window as popup so we can use window operations
  return true;
}

// Window opened by window.open
void Shell::WebContentsCreated(WebContents* source_contents,
                               int64 source_frame_id,
                               const GURL& target_url,
                               WebContents* new_contents) {
  // Create with package's manifest
  scoped_ptr<base::DictionaryValue> manifest(
      GetPackage()->window()->DeepCopy());

  // Get window features
  WebKit::WebWindowFeatures features = new_contents->GetWindowFeatures();
  manifest->SetBoolean(switches::kmResizable, features.resizable);
  manifest->SetBoolean(switches::kmFullscreen, features.fullscreen);
  if (features.widthSet)
    manifest->SetInteger(switches::kmWidth, features.width);
  if (features.heightSet)
    manifest->SetInteger(switches::kmHeight, features.height);
  if (features.xSet)
    manifest->SetInteger(switches::kmX, features.x);
  if (features.ySet)
    manifest->SetInteger(switches::kmY, features.y);
  // window.open should show the window by default.
  manifest->SetBoolean(switches::kmShow, true);

  Shell::Create(source_contents, target_url, manifest.get(), new_contents);
}

void Shell::RunFileChooser(WebContents* web_contents,
                           const content::FileChooserParams& params) {
  FileSelectHelper::RunFileChooser(web_contents, params);
}

void Shell::EnumerateDirectory(WebContents* web_contents,
                               int request_id,
                               const FilePath& path) {
  FileSelectHelper::EnumerateDirectory(web_contents, request_id, path);
}

void Shell::DidNavigateMainFramePostCommit(WebContents* web_contents) {
  window()->SetToolbarUrlEntry(web_contents->GetURL().spec());
}

JavaScriptDialogManager* Shell::GetJavaScriptDialogManager() {
  if (!dialog_creator_.get())
    dialog_creator_.reset(new ShellJavaScriptDialogCreator());
  return dialog_creator_.get();
}

bool Shell::AddMessageToConsole(WebContents* source,
                                int32 level,
                                const string16& message,
                                int32 line_no,
                                const string16& source_id) {
  return false;
}

void Shell::RequestToLockMouse(WebContents* web_contents,
                               bool user_gesture,
                               bool last_unlocked_by_target) {
  web_contents->GotResponseToLockMouseRequest(true);
}

void Shell::HandleKeyboardEvent(WebContents* source,
                                const NativeWebKeyboardEvent& event) {
  window()->HandleKeyboardEvent(event);
}

void Shell::RequestMediaAccessPermission(
      WebContents* web_contents,
      const MediaStreamRequest& request,
      const MediaResponseCallback& callback) {
  scoped_ptr<MediaStreamDevicesController>
      controller(new MediaStreamDevicesController(request,
                                                  callback));
  controller->DismissInfoBarAndTakeActionOnSettings();
}

void Shell::Observe(int type,
                    const NotificationSource& source,
                    const NotificationDetails& details) {
  if (type == NOTIFICATION_WEB_CONTENTS_TITLE_UPDATED) {
    std::pair<NavigationEntry*, bool>* title =
        Details<std::pair<NavigationEntry*, bool> >(details).ptr();

    if (title->first) {
      string16 text = title->first->GetTitle();
      window()->SetTitle(UTF16ToUTF8(text));
    }
  } else if (type == NOTIFICATION_RENDERER_PROCESS_CLOSED) {
    exit_code_ =
        content::Details<content::RenderProcessHost::RendererClosedDetails>(
            details)->exit_code;
#if defined(OS_POSIX)
    if (WIFEXITED(exit_code_))
      exit_code_ = WEXITSTATUS(exit_code_);
#endif
    MessageLoop::current()->PostTask(FROM_HERE, MessageLoop::QuitClosure());
  }
}

}  // namespace content
