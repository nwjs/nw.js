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
      this);
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

std::string ShellDevToolsDelegate::GetViewDescription(
    content::RenderViewHost*) {
  return std::string();
}

scoped_ptr<net::StreamListenSocket>
ShellDevToolsDelegate::CreateSocketForTethering(
    net::StreamListenSocket::Delegate* delegate,
    std::string* name) {
    return scoped_ptr<net::StreamListenSocket>();
}

const char kTargetTypePage[] = "page";

std::string GetViewDescription(WebContents* web_contents);

class Target : public content::DevToolsTarget {
 public:
  explicit Target(WebContents* web_contents);

  virtual std::string GetId() const OVERRIDE { return id_; }
  virtual std::string GetType() const OVERRIDE { return kTargetTypePage; }
  virtual std::string GetTitle() const OVERRIDE { return title_; }
  virtual std::string GetDescription() const OVERRIDE { return description_; }
  virtual GURL GetUrl() const OVERRIDE { return url_; }
  virtual GURL GetFaviconUrl() const OVERRIDE { return GURL(); }
  virtual base::TimeTicks GetLastActivityTime() const OVERRIDE {
    return last_activity_time_;
  }
  virtual bool IsAttached() const OVERRIDE {
    return agent_host_->IsAttached();
  }
  virtual scoped_refptr<DevToolsAgentHost> GetAgentHost() const OVERRIDE {
    return agent_host_;
  }
  virtual bool Activate() const OVERRIDE { return false; }
  virtual bool Close() const OVERRIDE { return false; }

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
      DevToolsAgentHost::GetOrCreateFor(web_contents->GetRenderViewHost());
  id_ = agent_host_->GetId();
  title_ = UTF16ToUTF8(web_contents->GetTitle());
  url_ = web_contents->GetURL();
  last_activity_time_ = web_contents->GetLastSelectedTime();
}

void ShellDevToolsDelegate::EnumerateTargets(TargetCallback callback) OVERRIDE {
  TargetList targets;
  std::vector<RenderViewHost*> rvh_list =
    content::DevToolsAgentHost::GetValidRenderViewHosts();
  for (std::vector<RenderViewHost*>::iterator it = rvh_list.begin();
       it != rvh_list.end(); ++it) {
    WebContents* web_contents = WebContents::FromRenderViewHost(*it);
    if (web_contents)
      targets.push_back(new Target(web_contents));
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
