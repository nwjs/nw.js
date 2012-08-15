// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_SHELL_RENDER_PROCESS_OBSERVER_H_
#define CONTENT_SHELL_SHELL_RENDER_PROCESS_OBSERVER_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "content/public/renderer/render_process_observer.h"

namespace content {

class ShellRenderProcessObserver : public RenderProcessObserver {
 public:
  ShellRenderProcessObserver();
  virtual ~ShellRenderProcessObserver();

  // RenderProcessObserver implementation.
  virtual void WebKitInitialized() OVERRIDE;

 private:
  DISALLOW_COPY_AND_ASSIGN(ShellRenderProcessObserver);
};

}  // namespace content

#endif  // CONTENT_SHELL_SHELL_RENDER_PROCESS_OBSERVER_H_
