// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_SHELL_DEVTOOLS_FRONTEND_H_
#define CONTENT_SHELL_SHELL_DEVTOOLS_FRONTEND_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "content/public/browser/devtools_agent_host.h"
#include "content/public/browser/devtools_client_host.h"
#include "content/public/browser/devtools_frontend_host_delegate.h"
#include "content/public/browser/web_contents_observer.h"

namespace content {

class RenderViewHost;
class Shell;
class WebContents;

class ShellDevToolsFrontend : public WebContentsObserver,
                              public DevToolsFrontendHostDelegate {
 public:

                                // static ShellDevToolsFrontend* Show(WebContents* inspected_contents);
  void Focus();
  void Close();

  Shell* frontend_shell() const { return frontend_shell_; }

  ShellDevToolsFrontend(Shell* frontend_shell, DevToolsAgentHost* agent_host);
  virtual ~ShellDevToolsFrontend();
 private:

  // WebContentsObserver overrides
  virtual void RenderViewCreated(RenderViewHost* render_view_host) OVERRIDE;
  virtual void WebContentsDestroyed(WebContents* web_contents) OVERRIDE;

  // DevToolsFrontendHostDelegate implementation
  virtual void DispatchOnEmbedder(const std::string& message) OVERRIDE {}
  virtual void InspectedContentsClosing() OVERRIDE;

  Shell* frontend_shell_;
  scoped_refptr<DevToolsAgentHost> agent_host_;
  scoped_ptr<DevToolsClientHost> frontend_host_;

  DISALLOW_COPY_AND_ASSIGN(ShellDevToolsFrontend);
};

}  // namespace content

#endif  // CONTENT_SHELL_SHELL_DEVTOOLS_FRONTEND_H_
