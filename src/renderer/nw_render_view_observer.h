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

#ifndef CONTENT_NW_SRC_RENDERER_NW_RENDER_VIEW_OBSERVER_H_
#define CONTENT_NW_SRC_RENDERER_NW_RENDER_VIEW_OBSERVER_H_

#include "content/public/renderer/render_view_observer.h"

#include <string>

class SkBitmap;

namespace content {
class RenderViewImpl;
}

namespace blink {
class WebView;
}

namespace nw {

class NwRenderViewObserver : public content::RenderViewObserver {
 public:
  NwRenderViewObserver(content::RenderView* render_view);
  ~NwRenderViewObserver() final;

  // RenderViewObserver implementation.
   bool OnMessageReceived(const IPC::Message& message) override;
   void DidCreateDocumentElement(blink::WebLocalFrame* frame) override;
   void DidFinishDocumentLoad(blink::WebLocalFrame* frame) override;

 private:

  void OnCaptureSnapshot();

  // Capture a snapshot of a view.  This is used to allow an extension
  // to get a snapshot of a tab using chrome.tabs.captureVisibleTab().
  bool CaptureSnapshot(blink::WebView* view, SkBitmap* snapshot);

  void OnDocumentCallback(content::RenderViewImpl* rv,
                          const std::string& js_fn,
                          blink::WebFrame* frame);

  DISALLOW_COPY_AND_ASSIGN(NwRenderViewObserver);
};

}  // namespace content

#endif  // CONTENT_NW_SRC_RENDERER_NW_RENDER_VIEW_OBSERVER_H_
