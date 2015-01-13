// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/shell_content_utility_client.h"

#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/memory/ref_counted.h"
#include "base/time/time.h"
#include "chrome/common/chrome_utility_messages.h"
#include "chrome/utility/chrome_content_utility_ipc_whitelist.h"
#include "chrome/utility/utility_message_handler.h"
//#include "chrome/utility/web_resource_unpacker.h"
#include "content/public/child/image_decoder_utils.h"
#include "content/public/common/content_switches.h"
#include "content/public/utility/utility_thread.h"
#include "courgette/courgette.h"
#include "courgette/third_party/bsdiff.h"
#include "ipc/ipc_channel.h"
#include "skia/ext/image_operations.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/zlib/google/zip.h"
#include "ui/gfx/codec/jpeg_codec.h"
#include "ui/gfx/size.h"

#include "base/debug/debugger.h"

#if defined(ENABLE_PRINT_PREVIEW) || defined(OS_WIN)
#include "printing_handler.h"
#endif

namespace {

bool Send(IPC::Message* message) {
  return content::UtilityThread::Get()->Send(message);
}

#if 0
void ReleaseProcessIfNeeded() {
  content::UtilityThread::Get()->ReleaseProcessIfNeeded();
}
#endif

}  // namespace

int64_t ShellContentUtilityClient::max_ipc_message_size_ =
    IPC::Channel::kMaximumMessageSize;

ShellContentUtilityClient::ShellContentUtilityClient()
    : filter_messages_(false) {

#if defined(ENABLE_PRINT_PREVIEW) || defined(OS_WIN)
  handlers_.push_back(new PrintingHandler());
#endif

}

ShellContentUtilityClient::~ShellContentUtilityClient() {
}

void ShellContentUtilityClient::UtilityThreadStarted() {

}

bool ShellContentUtilityClient::OnMessageReceived(
    const IPC::Message& message) {
  if (filter_messages_ && !ContainsKey(message_id_whitelist_, message.type()))
    return false;

  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(ShellContentUtilityClient, message)
    IPC_MESSAGE_HANDLER(ChromeUtilityMsg_StartupPing, OnStartupPing)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()

  for (Handlers::iterator it = handlers_.begin();
       !handled && it != handlers_.end(); ++it) {
    handled = (*it)->OnMessageReceived(message);
  }

  return handled;
}

// static
void ShellContentUtilityClient::PreSandboxStartup() {
  //base::debug::WaitForDebugger(120, false);
#if defined(ENABLE_PRINT_PREVIEW) || defined(OS_WIN)
  PrintingHandler::PreSandboxStartup();
#endif

}

void ShellContentUtilityClient::OnStartupPing() {
  Send(new ChromeUtilityHostMsg_ProcessStarted);
  // Don't release the process, we assume further messages are on the way.
}


