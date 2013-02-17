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

#include "content/nw/src/shell_content_client.h"

#include "base/string_piece.h"
#include "content/nw/src/api/api_messages.h"
#include "content/nw/src/renderer/common/render_messages.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/resource/resource_bundle.h"
#include "webkit/user_agent/user_agent_util.h"

namespace content {

ShellContentClient::~ShellContentClient() {
}

std::string ShellContentClient::GetUserAgent() const {
  return webkit_glue::BuildUserAgentFromProduct("Chrome/26.0.1377.0");
}

string16 ShellContentClient::GetLocalizedString(int message_id) const {
  return l10n_util::GetStringUTF16(message_id);
}

base::StringPiece ShellContentClient::GetDataResource(
    int resource_id,
    ui::ScaleFactor scale_factor) const {
  return ResourceBundle::GetSharedInstance().GetRawDataResource(resource_id);
}

gfx::Image& ShellContentClient::GetNativeImageNamed(int resource_id) const {
  return ResourceBundle::GetSharedInstance().GetNativeImageNamed(resource_id);
}

// This is for message handling after the rvh is swapped out
// FIXME: move dispatcher_host to WebContentsObserver

bool ShellContentClient::CanHandleWhileSwappedOut(
    const IPC::Message& msg) {
  switch (msg.type()) {
    case ShellViewHostMsg_Allocate_Object::ID:
    case ShellViewHostMsg_Deallocate_Object::ID:
    case ShellViewHostMsg_Call_Object_Method::ID:
    case ShellViewHostMsg_Call_Object_Method_Sync::ID:
    case ShellViewHostMsg_Call_Static_Method::ID:
    case ShellViewHostMsg_Call_Static_Method_Sync::ID:
    case ShellViewHostMsg_UncaughtException::ID:
    case ShellViewHostMsg_GetShellId::ID:
    case ShellViewHostMsg_CreateShell::ID:
      return true;
    default:
      break;
  }
  return false;
}

}  // namespace content
