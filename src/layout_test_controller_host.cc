// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/layout_test_controller_host.h"

#include "base/message_loop.h"
#include "content/public/browser/render_view_host.h"
#include "content/shell/shell_messages.h"
#include "webkit/support/webkit_support_gfx.h"

namespace content {

namespace {
const int kTestTimeoutMilliseconds = 30 * 1000;
}  // namespace

std::map<RenderViewHost*, LayoutTestControllerHost*>
    LayoutTestControllerHost::controllers_;
std::string LayoutTestControllerHost::expected_pixel_hash_;

// static
LayoutTestControllerHost* LayoutTestControllerHost::FromRenderViewHost(
    RenderViewHost* render_view_host) {
  const std::map<RenderViewHost*, LayoutTestControllerHost*>::iterator it =
      controllers_.find(render_view_host);
  if (it == controllers_.end())
    return NULL;
  return it->second;
}

// static
void LayoutTestControllerHost::Init(const std::string& expected_pixel_hash) {
  // TODO(jochen): We should only dump the results for the "main window".
  expected_pixel_hash_ = expected_pixel_hash;
}

LayoutTestControllerHost::LayoutTestControllerHost(
    RenderViewHost* render_view_host)
    : RenderViewHostObserver(render_view_host),
      captured_dump_(false),
      dump_as_text_(false),
      dump_child_frames_(false),
      is_printing_(false),
      should_stay_on_page_after_handling_before_unload_(false),
      wait_until_done_(false) {
  controllers_[render_view_host] = this;
}

LayoutTestControllerHost::~LayoutTestControllerHost() {
  controllers_.erase(render_view_host());
  watchdog_.Cancel();
}

void LayoutTestControllerHost::CaptureDump() {
  if (captured_dump_)
    return;
  captured_dump_ = true;

  render_view_host()->Send(
      new ShellViewMsg_CaptureTextDump(render_view_host()->GetRoutingID(),
                                       dump_as_text_,
                                       is_printing_,
                                       dump_child_frames_));
  if (!dump_as_text_) {
    render_view_host()->Send(
        new ShellViewMsg_CaptureImageDump(render_view_host()->GetRoutingID(),
                                          expected_pixel_hash_));
  }
}

void LayoutTestControllerHost::TimeoutHandler() {
  printf("FAIL: Timed out waiting for notifyDone to be called\n");
  fprintf(stderr, "FAIL: Timed out waiting for notifyDone to be called\n");
  CaptureDump();
}

bool LayoutTestControllerHost::OnMessageReceived(
    const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(LayoutTestControllerHost, message)
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_DidFinishLoad, OnDidFinishLoad)
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_TextDump, OnTextDump)
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_ImageDump, OnImageDump)
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_NotifyDone, OnNotifyDone)
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_DumpAsText, OnDumpAsText)
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_DumpChildFramesAsText,
                        OnDumpChildFramesAsText)
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_SetPrinting, OnSetPrinting)
    IPC_MESSAGE_HANDLER(
        ShellViewHostMsg_SetShouldStayOnPageAfterHandlingBeforeUnload,
        OnSetShouldStayOnPageAfterHandlingBeforeUnload)
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_WaitUntilDone, OnWaitUntilDone)
    IPC_MESSAGE_HANDLER(ShellViewHostMsg_NotImplemented, OnNotImplemented)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()

  return handled;
}

void LayoutTestControllerHost::OnDidFinishLoad() {
  if (wait_until_done_)
    return;

  CaptureDump();
}

void LayoutTestControllerHost::OnTextDump(const std::string& dump) {
  printf("%s#EOF\n", dump.c_str());
  fprintf(stderr, "#EOF\n");

  if (dump_as_text_)
    MessageLoop::current()->PostTask(FROM_HERE, MessageLoop::QuitClosure());
}

void LayoutTestControllerHost::OnImageDump(
    const std::string& actual_pixel_hash,
    const SkBitmap& image) {
#if !defined(OS_ANDROID)
  // DumpRenderTree is not currently supported for Android. Also, on Android
  // the required webkit_support methods are not defined, so this method just
  // doesn't compile.

  SkAutoLockPixels image_lock(image);

  printf("\nActualHash: %s\n", actual_pixel_hash.c_str());
  if (!expected_pixel_hash_.empty())
    printf("\nExpectedHash: %s\n", expected_pixel_hash_.c_str());

  // Only encode and dump the png if the hashes don't match. Encoding the
  // image is really expensive.
  if (actual_pixel_hash != expected_pixel_hash_) {
    std::vector<unsigned char> png;

    // Only the expected PNGs for Mac have a valid alpha channel.
#if defined(OS_MACOSX)
    bool discard_transparency = false;
#else
    bool discard_transparency = true;
#endif

    webkit_support::EncodeBGRAPNGWithChecksum(
        reinterpret_cast<const unsigned char*>(image.getPixels()),
        image.width(),
        image.height(),
        static_cast<int>(image.rowBytes()),
        discard_transparency,
        actual_pixel_hash,
        &png);

    printf("Content-Type: image/png\n");
    printf("Content-Length: %u\n", static_cast<unsigned>(png.size()));
    fwrite(&png[0], 1, png.size(), stdout);
  }

  MessageLoop::current()->PostTask(FROM_HERE, MessageLoop::QuitClosure());
#endif
}

void LayoutTestControllerHost::OnNotifyDone() {
  if (!wait_until_done_)
    return;
  watchdog_.Cancel();
  CaptureDump();
}

void LayoutTestControllerHost::OnDumpAsText() {
  dump_as_text_ = true;
}

void LayoutTestControllerHost::OnSetPrinting() {
  is_printing_ = true;
}

void LayoutTestControllerHost::OnSetShouldStayOnPageAfterHandlingBeforeUnload(
    bool should_stay_on_page) {
  should_stay_on_page_after_handling_before_unload_ = should_stay_on_page;
}

void LayoutTestControllerHost::OnDumpChildFramesAsText() {
  dump_child_frames_ = true;
}

void LayoutTestControllerHost::OnWaitUntilDone() {
  if (wait_until_done_)
    return;
  watchdog_.Reset(base::Bind(&LayoutTestControllerHost::TimeoutHandler,
                             base::Unretained(this)));
  MessageLoop::current()->PostDelayedTask(
      FROM_HERE,
      watchdog_.callback(),
      base::TimeDelta::FromMilliseconds(kTestTimeoutMilliseconds));
  wait_until_done_ = true;
}

void LayoutTestControllerHost::OnNotImplemented(
    const std::string& object_name,
    const std::string& property_name) {
  if (captured_dump_)
    return;
  printf("FAIL: NOT IMPLEMENTED: %s.%s\n",
         object_name.c_str(), property_name.c_str());
  fprintf(stderr, "FAIL: NOT IMPLEMENTED: %s.%s\n",
          object_name.c_str(), property_name.c_str());
  watchdog_.Cancel();
  CaptureDump();
}

}  // namespace content
