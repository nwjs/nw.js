// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/shell_devtools_frontend.h"

#include "base/command_line.h"
#include "base/path_service.h"
#include "content/public/browser/devtools_http_handler.h"
#include "content/public/browser/devtools_manager.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_view.h"
#include "content/public/common/content_client.h"
#include "content/nw/src/nw_shell.h"
#include "content/nw/src/shell_browser_context.h"
#include "content/nw/src/shell_browser_main_parts.h"
#include "content/nw/src/shell_content_browser_client.h"
#include "content/nw/src/browser/shell_devtools_delegate.h"

#include "net/base/net_util.h"

namespace content {

#if 0
// static
ShellDevToolsFrontend* ShellDevToolsFrontend::Show(
    WebContents* inspected_contents) {
  Shell* shell = Shell::CreateNewWindow(inspected_contents->GetBrowserContext(),
                                        GURL(),
                                        NULL,
                                        MSG_ROUTING_NONE,
                                        gfx::Size());
  ShellDevToolsFrontend* devtools_frontend = new ShellDevToolsFrontend(
      shell,
      DevToolsAgentHost::GetOrCreateFor(
          inspected_contents->GetRenderViewHost()));

  ShellDevToolsDelegate* delegate = ShellContentBrowserClient::Get()->
      shell_browser_main_parts()->devtools_delegate();
  if (CommandLine::ForCurrentProcess()->HasSwitch(switches::kDumpRenderTree))
    shell->LoadURL(GetDevToolsPathAsURL());
  else
    shell->LoadURL(delegate->devtools_http_handler()->GetFrontendURL(NULL));

  return devtools_frontend;
}
#endif

void ShellDevToolsFrontend::Focus() {
  web_contents()->GetView()->Focus();
}

void ShellDevToolsFrontend::Close() {
}

ShellDevToolsFrontend::ShellDevToolsFrontend(Shell* frontend_shell,
                                             DevToolsAgentHost* agent_host)
    : WebContentsObserver(frontend_shell->web_contents()),
      frontend_shell_(frontend_shell),
      agent_host_(agent_host) {
  frontend_host_.reset(
      DevToolsClientHost::CreateDevToolsFrontendHost(web_contents(), this));
}

ShellDevToolsFrontend::~ShellDevToolsFrontend() {
}

void ShellDevToolsFrontend::RenderViewCreated(
    RenderViewHost* render_view_host) {
  DevToolsClientHost::SetupDevToolsFrontendClient(render_view_host);
  //      web_contents()->GetRenderViewHost());
  DevToolsManager* manager = DevToolsManager::GetInstance();
  manager->RegisterDevToolsClientHostFor(agent_host_.get(),
                                         frontend_host_.get());
}

void ShellDevToolsFrontend::WebContentsDestroyed(WebContents* web_contents) {
  DevToolsManager::GetInstance()->ClientHostClosing(frontend_host_.get());
  delete this;
}

void ShellDevToolsFrontend::InspectedContentsClosing() {
}

}  // namespace content
