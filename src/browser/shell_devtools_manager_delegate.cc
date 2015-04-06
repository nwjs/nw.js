// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/browser/shell_devtools_manager_delegate.h"

#include <vector>

#include "base/bind.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/devtools_agent_host.h"
#include "content/public/browser/devtools_http_handler.h"
#include "content/public/browser/devtools_target.h"
#include "content/public/browser/favicon_status.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/url_constants.h"
#include "content/public/common/user_agent.h"
#include "content/nw/src/nw_shell.h"
#include "grit/nw_resources.h"
#include "net/socket/tcp_server_socket.h"
#include "ui/base/resource/resource_bundle.h"

#if defined(OS_ANDROID)
#include "content/public/browser/android/devtools_auth.h"
#include "net/socket/unix_domain_server_socket_posix.h"
#endif

using base::CommandLine;

namespace content {

namespace {

#if defined(OS_ANDROID)
const char kFrontEndURL[] =
    "http://chrome-devtools-frontend.appspot.com/serve_rev/%s/inspector.html";
#endif
const char kTargetTypePage[] = "page";
const char kTargetTypeServiceWorker[] = "service_worker";
const char kTargetTypeOther[] = "other";

#if defined(OS_ANDROID)
class UnixDomainServerSocketFactory
    : public DevToolsHttpHandler::ServerSocketFactory {
 public:
  explicit UnixDomainServerSocketFactory(const std::string& socket_name)
      : DevToolsHttpHandler::ServerSocketFactory(socket_name, 0, 1) {}

 private:
  // DevToolsHttpHandler::ServerSocketFactory.
  virtual scoped_ptr<net::ServerSocket> Create() const override {
    return scoped_ptr<net::ServerSocket>(
        new net::UnixDomainServerSocket(
            base::Bind(&CanUserConnectToDevTools),
            true /* use_abstract_namespace */));
  }

  DISALLOW_COPY_AND_ASSIGN(UnixDomainServerSocketFactory);
};
#else
class TCPServerSocketFactory
    : public DevToolsHttpHandler::ServerSocketFactory {
 public:
  TCPServerSocketFactory(const std::string& address, uint16 port, int backlog)
      : DevToolsHttpHandler::ServerSocketFactory(
            address, port, backlog) {}

 private:
  // DevToolsHttpHandler::ServerSocketFactory.
  scoped_ptr<net::ServerSocket> Create() const override {
    return scoped_ptr<net::ServerSocket>(
        new net::TCPServerSocket(NULL, net::NetLog::Source()));
  }

  DISALLOW_COPY_AND_ASSIGN(TCPServerSocketFactory);
};
#endif

scoped_ptr<DevToolsHttpHandler::ServerSocketFactory>
CreateSocketFactory() {
  const CommandLine& command_line = *CommandLine::ForCurrentProcess();
#if defined(OS_ANDROID)
  std::string socket_name = "content_shell_devtools_remote";
  if (command_line.HasSwitch(switches::kRemoteDebuggingSocketName)) {
    socket_name = command_line.GetSwitchValueASCII(
        switches::kRemoteDebuggingSocketName);
  }
  return scoped_ptr<DevToolsHttpHandler::ServerSocketFactory>(
      new UnixDomainServerSocketFactory(socket_name));
#else
  // See if the user specified a port on the command line (useful for
  // automation). If not, use an ephemeral port by specifying 0.
  uint16 port = 0;
  if (command_line.HasSwitch(switches::kRemoteDebuggingPort)) {
    int temp_port;
    std::string port_str =
        command_line.GetSwitchValueASCII(switches::kRemoteDebuggingPort);
    if (base::StringToInt(port_str, &temp_port) &&
        temp_port > 0 && temp_port < 65535) {
      port = static_cast<uint16>(temp_port);
    } else {
      DLOG(WARNING) << "Invalid http debugger port number " << temp_port;
    }
  }
  return scoped_ptr<DevToolsHttpHandler::ServerSocketFactory>(
      new TCPServerSocketFactory("127.0.0.1", port, 1));
#endif
}

class Target : public DevToolsTarget {
 public:
  explicit Target(scoped_refptr<DevToolsAgentHost> agent_host);

  std::string GetId() const override { return agent_host_->GetId(); }
  std::string GetParentId() const override { return std::string(); }
  std::string GetType() const override {
    switch (agent_host_->GetType()) {
      case DevToolsAgentHost::TYPE_WEB_CONTENTS:
        return kTargetTypePage;
      case DevToolsAgentHost::TYPE_SERVICE_WORKER:
        return kTargetTypeServiceWorker;
      default:
        break;
    }
    return kTargetTypeOther;
  }
  std::string GetTitle() const override { return agent_host_->GetTitle(); }
  std::string GetDescription() const override { return std::string(); }
  GURL GetURL() const override { return agent_host_->GetURL(); }
  GURL GetFaviconURL() const override { return favicon_url_; }
  base::TimeTicks GetLastActivityTime() const override {
    return last_activity_time_;
  }
  bool IsAttached() const override { return agent_host_->IsAttached(); }
  scoped_refptr<DevToolsAgentHost> GetAgentHost() const override {
    return agent_host_;
  }
  bool Activate() const override;
  bool Close() const override;

 private:
  scoped_refptr<DevToolsAgentHost> agent_host_;
  GURL favicon_url_;
  base::TimeTicks last_activity_time_;
};

Target::Target(scoped_refptr<DevToolsAgentHost> agent_host)
    : agent_host_(agent_host) {
  if (WebContents* web_contents = agent_host_->GetWebContents()) {
    NavigationController& controller = web_contents->GetController();
    NavigationEntry* entry = controller.GetActiveEntry();
    if (entry != NULL && entry->GetURL().is_valid())
      favicon_url_ = entry->GetFavicon().url;
    last_activity_time_ = web_contents->GetLastActiveTime();
  }
}

bool Target::Activate() const {
  return agent_host_->Activate();
}

bool Target::Close() const {
  return agent_host_->Close();
}

// ShellDevToolsDelegate ----------------------------------------------------

class ShellDevToolsDelegate : public DevToolsHttpHandlerDelegate {
 public:
  explicit ShellDevToolsDelegate(BrowserContext* browser_context);
  ~ShellDevToolsDelegate() override;

  // DevToolsHttpHandlerDelegate implementation.
  std::string GetDiscoveryPageHTML() override;
  bool BundlesFrontendResources() override;
  base::FilePath GetDebugFrontendDir() override;
  scoped_ptr<net::ServerSocket> CreateSocketForTethering(
      std::string* name) override;

 private:
  BrowserContext* browser_context_;

  DISALLOW_COPY_AND_ASSIGN(ShellDevToolsDelegate);
};

ShellDevToolsDelegate::ShellDevToolsDelegate(BrowserContext* browser_context)
    : browser_context_(browser_context) {
}

ShellDevToolsDelegate::~ShellDevToolsDelegate() {
}

std::string ShellDevToolsDelegate::GetDiscoveryPageHTML() {
#if defined(OS_ANDROID)
  return std::string();
#else
  return ResourceBundle::GetSharedInstance().GetRawDataResource(
      IDR_NW_DEVTOOLS_DISCOVERY_PAGE).as_string();
#endif
}

bool ShellDevToolsDelegate::BundlesFrontendResources() {
#if defined(OS_ANDROID)
  return false;
#else
  return true;
#endif
}

base::FilePath ShellDevToolsDelegate::GetDebugFrontendDir() {
  return base::FilePath();
}

scoped_ptr<net::ServerSocket>
ShellDevToolsDelegate::CreateSocketForTethering(std::string* name) {
  return scoped_ptr<net::ServerSocket>();
}

}  // namespace

// ShellDevToolsManagerDelegate ----------------------------------------------

// static
DevToolsHttpHandler*
ShellDevToolsManagerDelegate::CreateHttpHandler(
    BrowserContext* browser_context) {
  std::string frontend_url;
#if defined(OS_ANDROID)
  frontend_url = base::StringPrintf(kFrontEndURL, GetWebKitRevision().c_str());
#endif
  return DevToolsHttpHandler::Start(CreateSocketFactory(),
                                    frontend_url,
                                    new ShellDevToolsDelegate(browser_context),
                                    base::FilePath());
}

ShellDevToolsManagerDelegate::ShellDevToolsManagerDelegate(
    BrowserContext* browser_context)
    : browser_context_(browser_context) {
}

ShellDevToolsManagerDelegate::~ShellDevToolsManagerDelegate() {
}

base::DictionaryValue* ShellDevToolsManagerDelegate::HandleCommand(
    DevToolsAgentHost* agent_host,
    base::DictionaryValue* command) {
  return NULL;
}

std::string ShellDevToolsManagerDelegate::GetPageThumbnailData(
    const GURL& url) {
  return std::string();
}

scoped_ptr<DevToolsTarget>
ShellDevToolsManagerDelegate::CreateNewTarget(const GURL& url) {
  Shell* shell = Shell::Create(browser_context_,
                               url,
                               NULL,
                               MSG_ROUTING_NONE,
                               NULL);
  return scoped_ptr<DevToolsTarget>(
      new Target(DevToolsAgentHost::GetOrCreateFor(shell->web_contents())));
}

void ShellDevToolsManagerDelegate::EnumerateTargets(TargetCallback callback) {
  TargetList targets;
  for (const auto& agent_host : DevToolsAgentHost::GetOrCreateAll()) {
    targets.push_back(new Target(agent_host));
  }
  callback.Run(targets);
}

}  // namespace content
