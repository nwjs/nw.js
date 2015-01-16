// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/shell_devtools_frontend.h"

#include "base/command_line.h"
#include "base/json/json_reader.h"
#include "base/path_service.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "base/strings/stringprintf.h"
#include "content/public/browser/devtools_http_handler.h"
#include "content/public/browser/devtools_manager.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/content_client.h"
#include "content/nw/src/nw_shell.h"
#include "content/nw/src/shell_browser_context.h"
#include "content/nw/src/shell_browser_main_parts.h"
#include "content/nw/src/shell_content_browser_client.h"
#include "content/nw/src/browser/shell_devtools_delegate.h"

#include "net/base/net_util.h"
#include "net/base/filename_util.h"

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
  web_contents()->Focus();
}

void ShellDevToolsFrontend::Close() {
}

ShellDevToolsFrontend::ShellDevToolsFrontend(Shell* frontend_shell,
                                             DevToolsAgentHost* agent_host)
    : WebContentsObserver(frontend_shell->web_contents()),
      frontend_shell_(frontend_shell),
      agent_host_(agent_host) {
}

ShellDevToolsFrontend::~ShellDevToolsFrontend() {
}

void ShellDevToolsFrontend::RenderViewCreated(
    RenderViewHost* render_view_host) {
  if (!frontend_host_) {
    frontend_host_.reset(DevToolsFrontendHost::Create(render_view_host, this));
    DevToolsManager::GetInstance()->RegisterDevToolsClientHostFor(
        agent_host_.get(), this);
  }
}

void ShellDevToolsFrontend::DocumentOnLoadCompletedInMainFrame() {
  web_contents()->GetMainFrame()->ExecuteJavaScript(
      base::ASCIIToUTF16("InspectorFrontendAPI.setUseSoftMenu(true);"));
}

void ShellDevToolsFrontend::WebContentsDestroyed() {
  DevToolsManager::GetInstance()->ClientHostClosing(this);
  delete this;
}

void ShellDevToolsFrontend::InspectedContentsClosing() {
}

void ShellDevToolsFrontend::DispatchOnInspectorFrontend(
    const std::string& message) {
  std::string code = "InspectorFrontendAPI.dispatchMessage(" + message + ");";
  base::string16 javascript = base::UTF8ToUTF16(code);
  web_contents()->GetMainFrame()->ExecuteJavaScript(javascript);
}

void ShellDevToolsFrontend::HandleMessageFromDevToolsFrontend(
    const std::string& message) {
  std::string method;
  std::string browser_message;
  std::string url;
  int id = 0;

  base::ListValue* params = NULL;
  base::DictionaryValue* dict = NULL;
  scoped_ptr<base::Value> parsed_message(base::JSONReader::Read(message));
  if (!parsed_message ||
      !parsed_message->GetAsDictionary(&dict) ||
      !dict->GetString("method", &method) ||
      !dict->GetList("params", &params)) {
    return;
  }

  // handle sendMessageToBrowser
  if (method == "sendMessageToBrowser" &&
      params->GetSize() == 1 &&
      params->GetString(0, &browser_message)) {
    DevToolsManager::GetInstance()->DispatchOnInspectorBackend(
      this, browser_message);
  // handle inspectedURLChanged
  } else if (method == "inspectedURLChanged" &&
    params->GetSize() == 1 &&
    params->GetString(0, &url)) {
    InspectedURLChanged(url);
  // ignore others
  } else {
    return;
  }

  dict->GetInteger("id", &id);
  
  if (id) {
    std::string code = "InspectorFrontendAPI.embedderMessageAck(" +
        base::IntToString(id) + ",\"\");";
    base::string16 javascript = base::UTF8ToUTF16(code);
    web_contents()->GetMainFrame()->ExecuteJavaScript(javascript);
  }
}

void ShellDevToolsFrontend::InspectedURLChanged(const std::string& url) {
  std::string title = base::StringPrintf("Developer Tools - %s", url.c_str());
  base::ReplaceChars(title, "\\", "\\\\", &title);
  base::ReplaceChars(title, "\"", "\\\"", &title);
  base::string16 javascript = base::UTF8ToUTF16(base::StringPrintf("document.title = \"%s\"", title.c_str()));
  web_contents()->GetMainFrame()->ExecuteJavaScript(javascript);
}

void ShellDevToolsFrontend::HandleMessageFromDevToolsFrontendToBackend(
    const std::string& message) {
  DevToolsManager::GetInstance()->DispatchOnInspectorBackend(
      this, message);
}

}  // namespace content
