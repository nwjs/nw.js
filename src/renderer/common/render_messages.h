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

#include "content/public/common/common_param_traits.h"
#include "ipc/ipc_message.h"
#include "ipc/ipc_message_macros.h"
#include "ipc/ipc_param_traits.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "ui/gfx/ipc/gfx_param_traits.h"

// Singly-included section
#ifndef CONTENT_NW_SRC_RENDERER_COMMON_RENDER_MESSAGES_H_
#define CONTENT_NW_SRC_RENDERER_COMMON_RENDER_MESSAGES_H_

class SkBitmap;

#endif // CONTENT_NW_SRC_RENDERER_COMMON_RENDER_MESSAGES_H_

#define IPC_MESSAGE_START ShellMsgStart

// Tells the render view to capture a thumbnail image of the page. The
// render view responds with a ShellViewHostMsg_Snapshot.
IPC_MESSAGE_ROUTED0(NwViewMsg_CaptureSnapshot)

// Send a snapshot of the tab contents to the render host.
IPC_MESSAGE_ROUTED1(NwViewHostMsg_Snapshot,
                    SkBitmap /* bitmap */)

// Brings up SaveAs... dialog to save specified URL.
IPC_MESSAGE_ROUTED2(ChromeViewHostMsg_PDFSaveURLAs,
                    GURL /* url */,
                    content::Referrer /* referrer */)

// Updates the content restrictions, i.e. to disable print/copy.
IPC_MESSAGE_ROUTED1(ChromeViewHostMsg_PDFUpdateContentRestrictions,
                    int /* restrictions */)

// Brings up a Password... dialog for protected documents.
IPC_SYNC_MESSAGE_ROUTED1_1(ChromeViewHostMsg_PDFModalPromptForPassword,
                           std::string /* prompt */,
                           std::string /* actual_value */)

#if defined(ENABLE_PLUGINS)
// Sent by the renderer to check if crash reporting is enabled.
IPC_SYNC_MESSAGE_CONTROL0_1(ChromeViewHostMsg_IsCrashReportingEnabled,
                            bool /* enabled */)
#endif


