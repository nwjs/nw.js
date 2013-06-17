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
#include "content/nw/src/nw_shell.h"
#include "content/public/browser/devtools_http_handler.h"
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

RenderViewHost* ShellDevToolsDelegate::CreateNewTarget() {
  Shell* shell = Shell::Create(browser_context_,
                               GURL("nw:blank"),
                               NULL,
                               MSG_ROUTING_NONE,
                               NULL);
  return shell->web_contents()->GetRenderViewHost();
}

DevToolsHttpHandlerDelegate::TargetType
ShellDevToolsDelegate::GetTargetType(RenderViewHost*) {
  return kTargetTypeTab;
}

std::string ShellDevToolsDelegate::GetViewDescription(
    content::RenderViewHost*) {
  return std::string();
}

scoped_refptr<net::StreamListenSocket>
ShellDevToolsDelegate::CreateSocketForTethering(
    net::StreamListenSocket::Delegate* delegate,
    std::string* name) {
  return NULL;
}

}  // namespace content
