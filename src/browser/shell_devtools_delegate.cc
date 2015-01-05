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

#include "content/nw/src/browser/shell_devtools_delegate.h"

#include "base/files/file_path.h"
#include "base/strings/utf_string_conversions.h"
#include "content/nw/src/nw_shell.h"
#include "content/public/browser/devtools_http_handler.h"
#include "content/public/browser/devtools_target.h"
#include "content/public/browser/web_contents.h"
#include "grit/nw_resources.h"
#include "net/socket/tcp_listen_socket.h"
#include "net/url_request/url_request_context_getter.h"
#include "ui/base/layout.h"
#include "ui/base/resource/resource_bundle.h"

namespace content {

ShellDevToolsDelegate::ShellDevToolsDelegate(BrowserContext* browser_context,
                                             int port)
    : browser_context_(browser_context) {
  devtools_http_handler_ = DevToolsHttpHandler::Start(
      new net::TCPListenSocketFactory("127.0.0.1", port),
      "",
      this, base::FilePath());
}

ShellDevToolsDelegate::~ShellDevToolsDelegate() {
}

void ShellDevToolsDelegate::Stop() {
  // The call below destroys this.
  devtools_http_handler_->Stop();
}

std::string ShellDevToolsDelegate::GetDiscoveryPageHTML() {
  return ResourceBundle::GetSharedInstance().GetRawDataResource(
      IDR_NW_DEVTOOLS_DISCOVERY_PAGE).as_string();
}

bool ShellDevToolsDelegate::BundlesFrontendResources() {
  return true;
}

base::FilePath ShellDevToolsDelegate::GetDebugFrontendDir() {
  return base::FilePath();
}

std::string ShellDevToolsDelegate::GetPageThumbnailData(const GURL& url) {
  return "";
}

scoped_ptr<net::StreamListenSocket>
ShellDevToolsDelegate::CreateSocketForTethering(
    net::StreamListenSocket::Delegate* delegate,
    std::string* name) {
    return scoped_ptr<net::StreamListenSocket>();
}

const char kTargetTypePage[] = "page";

class Target : public content::DevToolsTarget {
 public:
  explicit Target(WebContents* web_contents);

  virtual std::string GetId() const OVERRIDE { return id_; }
  virtual std::string GetParentId() const OVERRIDE { return std::string(); }
  virtual std::string GetType() const OVERRIDE { return kTargetTypePage; }
  virtual std::string GetTitle() const OVERRIDE { return title_; }
  virtual std::string GetDescription() const OVERRIDE { return description_; }
  virtual GURL GetURL() const OVERRIDE { return url_; }
  virtual GURL GetFaviconURL() const OVERRIDE { return GURL(); }
  virtual base::TimeTicks GetLastActivityTime() const OVERRIDE {
    return last_activity_time_;
  }
  virtual bool IsAttached() const OVERRIDE {
    return agent_host_->IsAttached();
  }
  virtual scoped_refptr<DevToolsAgentHost> GetAgentHost() const OVERRIDE {
    return agent_host_;
  }
  virtual bool Activate() const OVERRIDE;
  virtual bool Close() const OVERRIDE;

 private:
  scoped_refptr<DevToolsAgentHost> agent_host_;
  std::string id_;
  std::string title_;
  std::string description_;
  GURL url_;
  base::TimeTicks last_activity_time_;
};

Target::Target(WebContents* web_contents) {
  agent_host_ =
      DevToolsAgentHost::GetOrCreateFor(web_contents);
  id_ = agent_host_->GetId();
  title_ = base::UTF16ToUTF8(web_contents->GetTitle());
  url_ = web_contents->GetURL();
  last_activity_time_ = web_contents->GetLastActiveTime();
}

bool Target::Activate() const {
  WebContents* web_contents = agent_host_->GetWebContents();
  if (!web_contents)
    return false;
  web_contents->GetDelegate()->ActivateContents(web_contents);
  return true;
}

bool Target::Close() const {
  WebContents* web_contents = agent_host_->GetWebContents();
  if (!web_contents)
    return false;
  web_contents->GetRenderViewHost()->ClosePage();
  return true;
}

void ShellDevToolsDelegate::EnumerateTargets(TargetCallback callback) {
  TargetList targets;
  std::vector<WebContents*> wc_list =
      content::DevToolsAgentHost::GetInspectableWebContents();
  for (std::vector<WebContents*>::iterator it = wc_list.begin();
       it != wc_list.end();
       ++it) {
    targets.push_back(new Target(*it));
  }
  callback.Run(targets);
}

scoped_ptr<DevToolsTarget> ShellDevToolsDelegate::CreateNewTarget(const GURL& url) {
  Shell* shell = Shell::Create(browser_context_,
                               GURL("nw:blank"),
                               NULL,
                               MSG_ROUTING_NONE,
                               NULL);
  return scoped_ptr<DevToolsTarget>(new Target(shell->web_contents()));
}

}  // namespace content
