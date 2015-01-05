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

#ifndef CONTENT_NW_SRC_API_DISPATCHER_H_
#define CONTENT_NW_SRC_API_DISPATCHER_H_

#include "base/basictypes.h"
#include "content/public/renderer/render_view_observer.h"
#include "third_party/WebKit/public/web/WebNavigationPolicy.h"
#include <v8.h>

namespace base {
class ListValue;
}

namespace content {
class RenderView;
}

namespace blink {
class WebFrame;
class WebURLRequest;
class WebView;
}

namespace nwapi {

class Dispatcher : public content::RenderViewObserver {
 public:
  explicit Dispatcher(content::RenderView* render_view);
  virtual ~Dispatcher();

  static v8::Handle<v8::Object> GetObjectRegistry();
  static v8::Handle<v8::Value> GetWindowId(blink::WebFrame* frame);
  static void ZoomLevelChanged(blink::WebView* web_view);
  static void willHandleNavigationPolicy(
    content::RenderView* rv,
    blink::WebFrame* frame,
    const blink::WebURLRequest& request,
    blink::WebNavigationPolicy* policy,
    blink::WebString* manifest);

 private:
  // RenderViewObserver implementation.
  virtual bool OnMessageReceived(const IPC::Message& message) OVERRIDE;
  virtual void DraggableRegionsChanged(blink::WebFrame* frame) OVERRIDE;
  virtual void DidFinishDocumentLoad(blink::WebLocalFrame* frame) OVERRIDE;
  virtual void DidCreateDocumentElement(blink::WebLocalFrame* frame) OVERRIDE;

  void documentCallback(const char* ev, blink::WebLocalFrame* frame);

  void OnEvent(int object_id,
               std::string event,
               const base::ListValue& arguments);

  DISALLOW_COPY_AND_ASSIGN(Dispatcher);
};

}  // namespace nwapi

#endif  // CONTENT_NW_SRC_API_DISPATCHER_H_

