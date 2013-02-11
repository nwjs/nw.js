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

#include "content/nw/src/browser/capture_page_helper.h"

#include <vector>

#include "base/base64.h"
#include "base/bind.h"
#include "base/stl_util.h"
#include "base/stringprintf.h"
#include "content/nw/src/api/api_messages.h"
#include "content/nw/src/nw_shell.h"
#include "content/nw/src/renderer/common/render_messages.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/web_contents.h"
#include "skia/ext/platform_canvas.h"
#include "ui/gfx/codec/jpeg_codec.h"
#include "ui/gfx/codec/png_codec.h"
#include "ui/gfx/rect.h"

namespace nw {

namespace capture_page_helper_constants {

const char kFormatValueJpeg[] = "jpeg";
const char kFormatValuePng[] = "png";
const char kMimeTypeJpeg[] = "image/jpeg";
const char kMimeTypePng[] = "image/png";

const int kDefaultQuality = 90;

}; // namespace capture_page_helper_constants

namespace keys = nw::capture_page_helper_constants;

// static
scoped_refptr<CapturePageHelper> CapturePageHelper::Create(
    content::Shell* shell) {
  return make_scoped_refptr(new CapturePageHelper(shell));
}

CapturePageHelper::CapturePageHelper(content::Shell *shell)
    : content::WebContentsObserver(shell->web_contents()),
      shell_(shell) {
}

CapturePageHelper::~CapturePageHelper() {
}

void CapturePageHelper::StartCapturePage(const std::string& image_format_str) {
  image_format_ = FORMAT_JPEG; // default image format.
  if (image_format_str == keys::kFormatValueJpeg) {
    image_format_ = FORMAT_JPEG;
  } else if (image_format_str == keys::kFormatValuePng) {
    image_format_ = FORMAT_PNG;
  } else {
    NOTREACHED() << "Invalid image format";
  }

  content::WebContents* web_contents = shell_->web_contents();
  content::RenderViewHost* render_view_host =
      web_contents->GetRenderViewHost();
  content::RenderWidgetHostView* view = render_view_host->GetView();

  if (!view) {
    VLOG(1) << "Get RenderViewWidgetHostView Failed.";
    return;
  }

  skia::PlatformBitmap* temp_bitmap = new skia::PlatformBitmap;
  render_view_host->CopyFromBackingStore(
      gfx::Rect(),
      view->GetViewBounds().size(),
      base::Bind(&CapturePageHelper::CopyFromBackingStoreComplete,
                 this,
                 base::Owned(temp_bitmap)),
      temp_bitmap);
}

void CapturePageHelper::CopyFromBackingStoreComplete(
    skia::PlatformBitmap* bitmap,
    bool succeeded) {
  if (succeeded) {
    // Get image from backing store.
    SendResultFromBitmap(bitmap->GetBitmap());
    return;
  }

  // Ask the renderer for a snapshot.
  Send(new NwViewMsg_CaptureSnapshot(routing_id()));
}

void CapturePageHelper::SendResultFromBitmap(const SkBitmap& screen_capture) {
  std::vector<unsigned char> data;
  SkAutoLockPixels screen_capture_lock(screen_capture);
  bool encoded = false;
  std::string mime_type;
  switch (image_format_) {
    case FORMAT_JPEG:
      encoded = gfx::JPEGCodec::Encode(
          reinterpret_cast<unsigned char*>(screen_capture.getAddr32(0, 0)),
          gfx::JPEGCodec::FORMAT_SkBitmap,
          screen_capture.width(),
          screen_capture.height(),
          static_cast<int>(screen_capture.rowBytes()),
          keys::kDefaultQuality,
          &data);
      mime_type = keys::kMimeTypeJpeg;
      break;
    case FORMAT_PNG:
      encoded = gfx::PNGCodec::EncodeBGRASkBitmap(
          screen_capture,
          true,  // Discard transparency.
          &data);
      mime_type = keys::kMimeTypePng;
      break;
    default:
      NOTREACHED() << "Invalid image format.";
  }

  if (!encoded) {
    VLOG(1) << "Encoding failed.";
    return;
  }

  std::string base64_result;
  base::StringPiece stream_as_string(
      reinterpret_cast<const char*>(vector_as_array(&data)), data.size());

  base::Base64Encode(stream_as_string, &base64_result);
  base64_result.insert(0, base::StringPrintf("data:%s;base64,",
                                             mime_type.c_str()));

  shell_->SendEvent("capturepagedone", base64_result);
}

void CapturePageHelper::OnSnapshot(const SkBitmap& bitmap) {
  SendResultFromBitmap(bitmap);
}

////////////////////////////////////////////////////////////////////////////////
// WebContentsObserver overrides
bool CapturePageHelper::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(CapturePageHelper, message)
    IPC_MESSAGE_HANDLER(NwViewHostMsg_Snapshot, OnSnapshot)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}

} // namespace nw
