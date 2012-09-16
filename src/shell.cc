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
base::Callback<void(Shell*)> Shell::shell_created_callback_;

bool Shell::quit_message_loop_ = true;

Shell::Shell(WebContents* web_contents, base::DictionaryValue* manifest)
    : window_(NULL),
      url_edit_view_(NULL),
      window_manifest_(manifest),
      is_show_devtools_(false),
      is_toolbar_open_(true),
      max_height_(-1),
      max_width_(-1),
      min_height_(-1),
      min_width_(-1)
#if defined(OS_WIN) && !defined(USE_AURA)
      , default_edit_wnd_proc_(0)
#endif
  {
  if (window_manifest_) {
    window_manifest_->GetBoolean(switches::kmToolbar, &is_toolbar_open_);
    window_manifest_->GetBoolean(switches::kDeveloper, &is_show_devtools_);
    window_manifest_->GetInteger(switches::kmMaxHeight, &max_height_);
    window_manifest_->GetInteger(switches::kmMaxWidth, &max_width_);
    window_manifest_->GetInteger(switches::kmMinHeight, &min_height_);
    window_manifest_->GetInteger(switches::kmMinWidth, &min_width_);
  }

  registrar_.Add(this, NOTIFICATION_WEB_CONTENTS_TITLE_UPDATED,
      Source<WebContents>(web_contents));
  windows_.push_back(this);

  if (!shell_created_callback_.is_null()) {
    shell_created_callback_.Run(this);
    shell_created_callback_.Reset();
  }
}

Shell::~Shell() {
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

Shell* Shell::CreateShell(WebContents* web_contents,
                          base::DictionaryValue* manifest) {
  int width = 700;
  int height = 450;
  manifest->GetInteger(switches::kmWidth, &width);
  manifest->GetInteger(switches::kmHeight, &height);

  Shell* shell = new Shell(web_contents, manifest);
  shell->PlatformCreateWindow(width, height);

  shell->web_contents_.reset(web_contents);
  web_contents->SetDelegate(shell);

  shell->PlatformSetContents();

  shell->PlatformResizeSubViews();
  return shell;
}

void Shell::CloseAllWindows() {
  AutoReset<bool> auto_reset(&quit_message_loop_, false);
  std::vector<Shell*> open_windows(windows_);
  for (size_t i = 0; i < open_windows.size(); ++i)
    open_windows[i]->Close();
  MessageLoop::current()->RunAllPending();
}

void Shell::SetShellCreatedCallback(
    base::Callback<void(Shell*)> shell_created_callback) {
  DCHECK(shell_created_callback_.is_null());
  shell_created_callback_ = shell_created_callback;
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

  // Create with package's manifest
  base::DictionaryValue *manifest = NULL;
  nw::GetManifest()->GetDictionary(switches::kmWindow, &manifest);
  manifest->SetBoolean(switches::kDeveloper,
      CommandLine::ForCurrentProcess()->HasSwitch(switches::kDeveloper));

  // Center window by default
  if (!manifest->HasKey(switches::kmPosition))
    manifest->SetString(switches::kmPosition, "center");

  Shell* shell = CreateShell(web_contents, manifest);
  if (!url.is_empty())
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
  manifest.SetBoolean(switches::kDeveloper, true);
  manifest.SetInteger(switches::kmWidth, 600);
  manifest.SetInteger(switches::kmHeight, 500);

  Shell* shell = CreateShell(
      WebContents::Create(web_contents()->GetBrowserContext(),
                          NULL, MSG_ROUTING_NONE, NULL),
      &manifest);
  shell->LoadURL(url);
}

gfx::NativeView Shell::GetContentView() {
  if (!web_contents_.get())
    return NULL;
  return web_contents_->GetNativeView();
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
  LOG(ERROR) << "activate";
}

void Shell::DeactivateContents(content::WebContents* contents) {
  LOG(ERROR) << "deactivate";
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

void Shell::LostCapture() {
}

void Shell::WebContentsFocused(WebContents* contents) {
}

void Shell::WebContentsCreated(WebContents* source_contents,
                               int64 source_frame_id,
                               const GURL& target_url,
                               WebContents* new_contents) {
  // Create with package's manifest
  base::DictionaryValue *manifest = NULL;
  nw::GetManifest()->GetDictionary(switches::kmWindow, &manifest);
  manifest->SetBoolean(switches::kDeveloper,
      CommandLine::ForCurrentProcess()->HasSwitch(switches::kDeveloper));

  // Get window features
  WebKit::WebWindowFeatures features = new_contents->GetWindowFeatures();
  if (features.widthSet)
    manifest->SetInteger(switches::kmWidth, features.width);
  if (features.heightSet)
    manifest->SetInteger(switches::kmHeight, features.height);
  if (features.xSet)
    manifest->SetInteger(switches::kmX, features.x);
  if (features.ySet)
    manifest->SetInteger(switches::kmY, features.y);

  CreateShell(new_contents, manifest);
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
      PlatformSetTitle(text);
    }
  }
}

}  // namespace content
