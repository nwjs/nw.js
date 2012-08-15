// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/shell_render_process_observer.h"

#include "base/command_line.h"
#include "content/public/renderer/render_thread.h"
#include "content/shell/layout_test_controller_bindings.h"
#include "content/shell/shell_switches.h"
#include "webkit/glue/webkit_glue.h"
#include "webkit/support/gc_extension.h"

namespace content {

ShellRenderProcessObserver::ShellRenderProcessObserver() {
  RenderThread::Get()->AddObserver(this);
}

ShellRenderProcessObserver::~ShellRenderProcessObserver() {
}

void ShellRenderProcessObserver::WebKitInitialized() {
  if (!CommandLine::ForCurrentProcess()->HasSwitch(switches::kDumpRenderTree))
    return;

  // To implement a catch-all for not yet implemented controller properties.
  webkit_glue::SetJavaScriptFlags(" --harmony_proxies");
  RenderThread::Get()->RegisterExtension(new LayoutTestControllerBindings());

  // We always expose GC to layout tests.
  webkit_glue::SetJavaScriptFlags(" --expose-gc");
  RenderThread::Get()->RegisterExtension(extensions_v8::GCExtension::Get());
}

}  // namespace content
