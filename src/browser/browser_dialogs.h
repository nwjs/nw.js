// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NW_BROWSER_UI_BROWSER_DIALOGS_H_
#define NW_BROWSER_UI_BROWSER_DIALOGS_H_

#include "base/callback.h"
#include "ipc/ipc_message.h"  // For IPC_MESSAGE_LOG_ENABLED.
#include "third_party/skia/include/core/SkColor.h"
#include "ui/gfx/native_widget_types.h"

namespace content {
class BrowserContext;
class ColorChooser;
class WebContents;
}

namespace nw {

// Shows a color chooser that reports to the given WebContents.
content::ColorChooser* ShowColorChooser(content::WebContents* web_contents,
                                        SkColor initial_color);
}  // namespace nw

#endif  // NW_BROWSER_UI_BROWSER_DIALOGS_H_
