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

#include "content/nw/src/shell.h"

#include "base/auto_reset.h"
#include "base/command_line.h"
#include "base/message_loop.h"
#include "base/path_service.h"
#include "base/string_number_conversions.h"
#include "base/string_util.h"
#include "base/utf_string_conversions.h"
#include "base/values.h"
#include "content/public/browser/devtools_http_handler.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/notification_details.h"
#include "content/public/browser/notification_source.h"
#include "content/public/browser/notification_types.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "content/nw/src/api/api_messages.h"
#include "content/nw/src/browser/file_select_helper.h"
#include "content/nw/src/browser/shell_devtools_delegate.h"
#include "content/nw/src/browser/shell_javascript_dialog_creator.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/media/media_stream_devices_controller.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/shell_browser_main_parts.h"
#include "content/nw/src/shell_content_browser_client.h"
#include "ui/gfx/size.h"

namespace content {

std::vector<Shell*> Shell::windows_;

bool Shell::quit_message_loop_ = true;

Shell::Shell(WebContents* web_contents, base::DictionaryValue* manifest)
    : window_(NULL),
      url_edit_view_(NULL),
      force_close_(false),
      id_(-1),
      has_frame_(true),
      is_show_devtools_(false),
      is_toolbar_open_(true)
#if defined(OS_WIN)
      , max_height_(-1),
      max_width_(-1),
      min_height_(-1),
      min_width_(-1),
      default_edit_wnd_proc_(0)
#endif
  {
  // Register shell.
  registrar_.Add(this, NOTIFICATION_WEB_CONTENTS_TITLE_UPDATED,
      Source<WebContents>(web_contents));
  windows_.push_back(this);

  // Debug settings
  is_show_devtools_ =
      CommandLine::ForCurrentProcess()->HasSwitch(switches::kDeveloper);
  manifest->GetBoolean(switches::kmToolbar, &is_toolbar_open_);

  // Initialize shell.
  int width = 700, height = 450;
  manifest->GetInteger(switches::kmWidth, &width);
  manifest->GetInteger(switches::kmHeight, &height);
  manifest->GetBoolean(switches::kmFrame, &has_frame_);
  PlatformCreateWindow(width, height);

  // Setup window from manifest.
#if defined(TOOLKIT_GTK)
  bool as_desktop;
  if (manifest->GetBoolean(switches::kmAsDesktop, &as_desktop) && as_desktop) {
    SetAsDesktop();
  }
#endif
  int x, y;
  std::string position;
  if (manifest->GetInteger(switches::kmX, &x) &&
      manifest->GetInteger(switches::kmY, &y)) {
    Move(gfx::Rect(x, y, width, height));
  } else if (manifest->GetString(switches::kmPosition, &position)) {
    SetPosition(position);
  }
  int min_height, min_width;
  if (manifest->GetInteger(switches::kmMinHeight, &min_height) &&
      manifest->GetInteger(switches::kmMinWidth, &min_width)) {
    SetMinimumSize(min_width, min_height);
  }
  int max_height, max_width;
  if (manifest->GetInteger(switches::kmMaxHeight, &max_height) &&
      manifest->GetInteger(switches::kmMaxWidth, &max_width)) {
    SetMaximumSize(max_width, max_height);
  }
  bool resizable;
  if (manifest->GetBoolean(switches::kmResizable, &resizable)) {
    SetResizable(resizable);
  }
  bool fullscreen;
  if (manifest->GetBoolean(switches::kmFullscreen, &fullscreen) && fullscreen) {
    EnterFullscreen();
  }
  std::string title("node-webkit");
  manifest->GetString(switches::kmTitle, &title);
  SetTitle(title);

  // Add web contents.
  web_contents_.reset(web_contents);
  content::WebContentsObserver::Observe(web_contents);
  web_contents_->SetDelegate(this);
  PlatformSetContents();
  PlatformResizeSubViews();
  Show();
}

Shell::~Shell() {
  SendEvent("closed");

  PlatformCleanUp();

  for (size_t i = 0; i < windows_.size(); ++i) {
    if (windows_[i] == this) {
      windows_.erase(windows_.begin() + i);
      break;
    }
  }

  if (windows_.empty() && quit_message_loop_)
    MessageLoop::current()->PostTask(FROM_HERE, MessageLoop::QuitClosure());
}

void Shell::SendEvent(const std::string& event) {
  if (id() < 0)
    return;

  base::ListValue args;
  web_contents()->GetRenderViewHost()->Send(new ShellViewMsg_Object_On_Event(
      web_contents()->GetRoutingID(), id(), event, args));
}

nw::Package* Shell::GetPackage() {
  ShellContentBrowserClient* browser_client = 
      static_cast<ShellContentBrowserClient*>(GetContentClient()->browser());
  return browser_client->shell_browser_main_parts()->package();
}

void Shell::CloseAllWindows() {
  AutoReset<bool> auto_reset(&quit_message_loop_, false);
  std::vector<Shell*> open_windows(windows_);
  for (size_t i = 0; i < open_windows.size(); ++i)
    open_windows[i]->Close();
  MessageLoop::current()->RunAllPending();
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

Shell* Shell::CreateNewWindow(BrowserContext* browser_context,
                              const GURL& url,
                              SiteInstance* site_instance,
                              int routing_id,
                              WebContents* base_web_contents) {
  WebContents* web_contents = WebContents::Create(
      browser_context,
      site_instance,
      routing_id,
      base_web_contents);

  Shell* shell = new Shell(web_contents, GetPackage()->window());
  shell->LoadURL(url);
  return shell;
}

void Shell::LoadURL(const GURL& url) {
  web_contents_->GetController().LoadURL(
      url,
      Referrer(),
      PAGE_TRANSITION_TYPED,
      std::string());
  web_contents_->Focus();
}

void Shell::GoBackOrForward(int offset) {
  web_contents_->GetController().GoToOffset(offset);
  web_contents_->Focus();
}

void Shell::Reload() {
  web_contents_->GetController().Reload(false);
  web_contents_->Focus();
}

void Shell::Stop() {
  web_contents_->Stop();
  web_contents_->Focus();
}

void Shell::UpdateNavigationControls() {
  int current_index = web_contents_->GetController().GetCurrentEntryIndex();
  int max_index = web_contents_->GetController().GetEntryCount() - 1;

  PlatformEnableUIControl(BACK_BUTTON, current_index > 0);
  PlatformEnableUIControl(FORWARD_BUTTON, current_index < max_index);
  PlatformEnableUIControl(STOP_BUTTON, web_contents_->IsLoading());
}

void Shell::ShowDevTools() {
  ShellContentBrowserClient* browser_client =
      static_cast<ShellContentBrowserClient*>(
          GetContentClient()->browser());
  ShellDevToolsDelegate* delegate =
      browser_client->shell_browser_main_parts()->devtools_delegate();
  GURL url = delegate->devtools_http_handler()->GetFrontendURL(
      web_contents()->GetRenderViewHost());

  // Use our minimum set manifest
  base::DictionaryValue manifest;
  manifest.SetBoolean(switches::kmToolbar, false);
  manifest.SetInteger(switches::kmWidth, 600);
  manifest.SetInteger(switches::kmHeight, 500);

  Shell* shell = new Shell(
      WebContents::Create(web_contents()->GetBrowserContext(),
                          NULL, MSG_ROUTING_NONE, NULL),
      &manifest);
  shell->force_close_ = true;
  shell->LoadURL(url);
}

gfx::NativeView Shell::GetContentView() {
  if (!web_contents_.get())
    return NULL;
  return web_contents_->GetNativeView();
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
  UpdateNavigationControls();
  PlatformSetIsLoading(source->IsLoading());
}

void Shell::ActivateContents(content::WebContents* contents) {
  Focus(true);
}

void Shell::DeactivateContents(content::WebContents* contents) {
  if (windows_.size() == 1) {
    Focus(false);
    return;
  }

  // Move focus to other window
  for (unsigned i = 0; i < windows_.size(); ++i) {
    if (windows_[i] != this) {
      windows_[i]->Focus(true);
      break;
    }
  }
}

void Shell::CloseContents(WebContents* source) {
  Close();
}

void Shell::MoveContents(WebContents* source, const gfx::Rect& pos) {
  Move(pos);
}

bool Shell::IsPopupOrPanel(const WebContents* source) const {
  // Treat very window as popup so we can use window operations
  return true;
}

bool Shell::TakeFocus(WebContents* soruce,
                      bool reverse) {
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

  new Shell(new_contents, manifest.get());
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
  PlatformSetAddressBarURL(web_contents->GetURL());
}

JavaScriptDialogCreator* Shell::GetJavaScriptDialogCreator() {
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

void Shell::RequestMediaAccessPermission(
      content::WebContents* web_contents,
      const content::MediaStreamRequest* request,
      const content::MediaResponseCallback& callback) {
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
      SetTitle(UTF16ToUTF8(text));
    }
  }
}

}  // namespace content
