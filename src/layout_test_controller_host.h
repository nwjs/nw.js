// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_LAYOUT_TEST_CONTROLLER_HOST_H_
#define CONTENT_SHELL_LAYOUT_TEST_CONTROLLER_HOST_H_

#include <map>
#include <string>

#include "base/cancelable_callback.h"
#include "content/public/browser/render_view_host_observer.h"

class SkBitmap;

namespace content {

class LayoutTestControllerHost : public RenderViewHostObserver {
 public:
  static LayoutTestControllerHost* FromRenderViewHost(
      RenderViewHost* render_view_host);

  // Initialize the LayoutTestControllerHost for a given test.
  static void Init(const std::string& expected_pixel_hash);

  explicit LayoutTestControllerHost(RenderViewHost* render_view_host);
  virtual ~LayoutTestControllerHost();

  bool should_stay_on_page_after_handling_before_unload() const {
    return should_stay_on_page_after_handling_before_unload_;
  }

  // RenderViewHostObserver implementation.
  virtual bool OnMessageReceived(const IPC::Message& message) OVERRIDE;

 private:
  void CaptureDump();
  void TimeoutHandler();

  // Message handlers.
  void OnDidFinishLoad();
  void OnTextDump(const std::string& dump);
  void OnImageDump(const std::string& actual_pixel_hash, const SkBitmap& image);

  // testRunner handlers.
  void OnNotifyDone();
  void OnDumpAsText();
  void OnDumpChildFramesAsText();
  void OnSetPrinting();
  void OnSetShouldStayOnPageAfterHandlingBeforeUnload(bool should_stay_on_page);
  void OnWaitUntilDone();

  void OnNotImplemented(const std::string& object_name,
                        const std::string& method_name);

  static std::map<RenderViewHost*, LayoutTestControllerHost*> controllers_;
  static std::string expected_pixel_hash_;

  bool captured_dump_;

  bool dump_as_text_;
  bool dump_child_frames_;
  bool is_printing_;
  bool should_stay_on_page_after_handling_before_unload_;
  bool wait_until_done_;

  base::CancelableClosure watchdog_;

  DISALLOW_COPY_AND_ASSIGN(LayoutTestControllerHost);
};

}  // namespace content

#endif  // CONTENT_SHELL_LAYOUT_TEST_CONTROLLER_HOST_H_
