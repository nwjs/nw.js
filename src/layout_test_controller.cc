// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/layout_test_controller.h"

#include "base/md5.h"
#include "base/stringprintf.h"
#include "content/public/renderer/render_view.h"
#include "content/shell/shell_messages.h"
#include "skia/ext/platform_canvas.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/platform/WebCString.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/platform/WebRect.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/platform/WebSize.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/platform/WebString.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebDocument.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebElement.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebFrame.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebTestingSupport.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebView.h"
#include "webkit/glue/webkit_glue.h"

using WebKit::WebFrame;
using WebKit::WebElement;
using WebKit::WebRect;
using WebKit::WebSize;
using WebKit::WebTestingSupport;
using WebKit::WebView;

namespace content {

namespace {

std::string DumpDocumentText(WebFrame* frame) {
  // We use the document element's text instead of the body text here because
  // not all documents have a body, such as XML documents.
  WebElement documentElement = frame->document().documentElement();
  if (documentElement.isNull())
    return std::string();
  return documentElement.innerText().utf8();
}

std::string DumpDocumentPrintedText(WebFrame* frame) {
  return frame->renderTreeAsText(WebFrame::RenderAsTextPrinting).utf8();
}

std::string DumpFramesAsText(WebFrame* frame, bool printing, bool recursive) {
  std::string result;

  // Cannot do printed format for anything other than HTML.
  if (printing && !frame->document().isHTMLDocument())
    return std::string();

  // Add header for all but the main frame. Skip emtpy frames.
  if (frame->parent() && !frame->document().documentElement().isNull()) {
    result.append("\n--------\nFrame: '");
    result.append(frame->name().utf8().data());
    result.append("'\n--------\n");
  }

  result.append(
      printing ? DumpDocumentPrintedText(frame) : DumpDocumentText(frame));
  result.append("\n");

  if (recursive) {
    for (WebFrame* child = frame->firstChild(); child;
         child = child->nextSibling()) {
      result.append(DumpFramesAsText(child, printing, recursive));
    }
  }
  return result;
}

std::string DumpFrameScrollPosition(WebFrame* frame, bool recursive) {
  std::string result;

  WebSize offset = frame->scrollOffset();
  if (offset.width > 0 || offset.height > 0) {
    if (frame->parent()) {
      result.append(
          base::StringPrintf("frame '%s' ", frame->name().utf8().data()));
    }
    result.append(
        base::StringPrintf("scrolled to %d,%d\n", offset.width, offset.height));
  }

  if (recursive) {
    for (WebFrame* child = frame->firstChild(); child;
         child = child->nextSibling()) {
      result.append(DumpFrameScrollPosition(child, recursive));
    }
  }
  return result;
}

bool PaintViewIntoCanvas(WebView* view, skia::PlatformCanvas& canvas) {
  view->layout();
  const WebSize& size = view->size();

  if (!canvas.initialize(size.width, size.height, true))
    return false;

  view->paint(webkit_glue::ToWebCanvas(&canvas),
              WebRect(0, 0, size.width, size.height));
  return true;
}

#if !defined(OS_MACOSX)
void MakeBitmapOpaque(SkBitmap* bitmap) {
  SkAutoLockPixels lock(*bitmap);
  DCHECK(bitmap->config() == SkBitmap::kARGB_8888_Config);
  for (int y = 0; y < bitmap->height(); ++y) {
    uint32_t* row = bitmap->getAddr32(0, y);
    for (int x = 0; x < bitmap->width(); ++x)
      row[x] |= 0xFF000000;  // Set alpha bits to 1.
  }
}
#endif

void CaptureSnapshot(WebView* view, SkBitmap* snapshot) {
  skia::PlatformCanvas canvas;
  if (!PaintViewIntoCanvas(view, canvas))
    return;

  SkDevice* device = skia::GetTopDevice(canvas);

  const SkBitmap& bitmap = device->accessBitmap(false);
  bitmap.copyTo(snapshot, SkBitmap::kARGB_8888_Config);

#if !defined(OS_MACOSX)
  // Only the expected PNGs for Mac have a valid alpha channel.
  MakeBitmapOpaque(snapshot);
#endif

}

}  // namespace

LayoutTestController::LayoutTestController(RenderView* render_view)
    : RenderViewObserver(render_view) {
}

LayoutTestController::~LayoutTestController() {
}

void LayoutTestController::DidClearWindowObject(WebFrame* frame) {
  WebTestingSupport::injectInternalsObject(frame);
}

void LayoutTestController::DidFinishLoad(WebFrame* frame) {
  if (!frame->parent())
    Send(new ShellViewHostMsg_DidFinishLoad(routing_id()));
}

bool LayoutTestController::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(LayoutTestController, message)
    IPC_MESSAGE_HANDLER(ShellViewMsg_CaptureTextDump, OnCaptureTextDump)
    IPC_MESSAGE_HANDLER(ShellViewMsg_CaptureImageDump, OnCaptureImageDump)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()

  return handled;
}

void LayoutTestController::OnCaptureTextDump(bool as_text,
                                             bool printing,
                                             bool recursive) {
  WebFrame* frame = render_view()->GetWebView()->mainFrame();
  std::string dump;
  if (as_text) {
    dump = DumpFramesAsText(frame, printing, recursive);
  } else {
    WebFrame::RenderAsTextControls render_text_behavior =
        WebFrame::RenderAsTextNormal;
    if (printing)
      render_text_behavior |= WebFrame::RenderAsTextPrinting;
    dump = frame->renderTreeAsText(render_text_behavior).utf8();
    dump.append(DumpFrameScrollPosition(frame, recursive));
  }
  Send(new ShellViewHostMsg_TextDump(routing_id(), dump));
}

void LayoutTestController::OnCaptureImageDump(
    const std::string& expected_pixel_hash) {
  SkBitmap snapshot;
  CaptureSnapshot(render_view()->GetWebView(), &snapshot);

  SkAutoLockPixels snapshot_lock(snapshot);
  base::MD5Digest digest;
  base::MD5Sum(snapshot.getPixels(), snapshot.getSize(), &digest);
  std::string actual_pixel_hash = base::MD5DigestToBase16(digest);

  if (actual_pixel_hash == expected_pixel_hash) {
    SkBitmap empty_image;
    Send(new ShellViewHostMsg_ImageDump(
        routing_id(), actual_pixel_hash, empty_image));
  }
  Send(new ShellViewHostMsg_ImageDump(
      routing_id(), actual_pixel_hash, snapshot));
}

}  // namespace content
