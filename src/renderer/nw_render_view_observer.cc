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

#include "content/nw/src/renderer/nw_render_view_observer.h"

#include <v8.h>

#include "base/files/file_util.h"
#include "base/strings/utf_string_conversions.h"
#include "content/nw/src/renderer/common/render_messages.h"
#include "content/public/renderer/render_view.h"
#include "content/renderer/render_view_impl.h"
#include "skia/ext/platform_canvas.h"
#include "third_party/WebKit/public/platform/WebRect.h"
#include "third_party/WebKit/public/platform/WebSize.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebScriptSource.h"
#include "third_party/WebKit/public/web/WebView.h"

using content::RenderView;
using content::RenderViewImpl;

using blink::WebFrame;
using blink::WebRect;
using blink::WebScriptSource;
using blink::WebSize;

namespace nw {

NwRenderViewObserver::NwRenderViewObserver(content::RenderView* render_view) 
    : content::RenderViewObserver(render_view) {
}

NwRenderViewObserver::~NwRenderViewObserver() {
}

bool NwRenderViewObserver::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(NwRenderViewObserver, message)
    IPC_MESSAGE_HANDLER(NwViewMsg_CaptureSnapshot, OnCaptureSnapshot)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()

  return handled;
}

void NwRenderViewObserver::OnCaptureSnapshot() {
  SkBitmap snapshot;
  bool error = false;

  WebFrame* main_frame = render_view()->GetWebView()->mainFrame();
  if (!main_frame)
    error = true;

  if (!error && !CaptureSnapshot(render_view()->GetWebView(), &snapshot))
    error = true;

  DCHECK(error == snapshot.empty()) <<
      "Snapshot should be empty on error, non-empty otherwise.";

  // Send the snapshot to the browser process.
  Send(new NwViewHostMsg_Snapshot(routing_id(), snapshot));
}

void NwRenderViewObserver::DidFinishDocumentLoad(blink::WebLocalFrame* frame) {
  RenderViewImpl* rv = RenderViewImpl::FromWebView(frame->view());
  if (!rv)
    return;
  std::string js_fn = rv->renderer_preferences_.nw_inject_js_doc_end;
  OnDocumentCallback(rv, js_fn, frame);
}

void NwRenderViewObserver::DidCreateDocumentElement(blink::WebLocalFrame* frame) {
  RenderViewImpl* rv = RenderViewImpl::FromWebView(frame->view());
  if (!rv)
    return;
  std::string js_fn = rv->renderer_preferences_.nw_inject_js_doc_start;
  OnDocumentCallback(rv, js_fn, frame);
}

void NwRenderViewObserver::OnDocumentCallback(RenderViewImpl* rv,
                                              const std::string& js_fn,
                                              blink::WebFrame* frame) {
  if (js_fn.empty())
    return;
  std::string content;
  base::FilePath path = rv->renderer_preferences_.nw_app_root_path.AppendASCII(js_fn);
  if (!base::ReadFileToString(path, &content)) {
    LOG(WARNING) << "Failed to load js script file: " << path.value();
    return;
  }
  base::string16 jscript = base::UTF8ToUTF16(content);
  v8::HandleScope handle_scope(v8::Isolate::GetCurrent());
  // v8::Handle<v8::Value> result;
  frame->executeScriptAndReturnValue(WebScriptSource(jscript));
}

bool NwRenderViewObserver::CaptureSnapshot(blink::WebView* view,
                                           SkBitmap* snapshot) {
  view->layout();
  const WebSize& size = view->size();

  skia::RefPtr<SkCanvas> canvas = skia::AdoptRef(
      skia::CreatePlatformCanvas(
          size.width, size.height, true, NULL, skia::RETURN_NULL_ON_FAILURE));
  if (!canvas)
    return false;

  view->paint(canvas.get(),
              WebRect(0, 0, size.width, size.height));
  // TODO: Add a way to snapshot the whole page, not just the currently
  // visible part.

  SkBaseDevice* device = skia::GetTopDevice(*canvas);

  const SkBitmap& bitmap = device->accessBitmap(false);
  if (!bitmap.copyTo(snapshot, kN32_SkColorType))
    return false;

  return true;
}

}  // namespace content
