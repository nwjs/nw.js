// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/shell_content_renderer_client.h"

#include "base/command_line.h"
#include "content/shell/layout_test_controller.h"
#include "content/shell/shell_render_process_observer.h"
#include "content/shell/shell_switches.h"
#include "v8/include/v8.h"

namespace content {

ShellContentRendererClient::ShellContentRendererClient() {
}

ShellContentRendererClient::~ShellContentRendererClient() {
}

void ShellContentRendererClient::RenderThreadStarted() {
  shell_observer_.reset(new ShellRenderProcessObserver());
}

void ShellContentRendererClient::RenderViewCreated(RenderView* render_view) {
  if (!CommandLine::ForCurrentProcess()->HasSwitch(switches::kDumpRenderTree))
    return;

  new LayoutTestController(render_view);
}

}  // namespace content
