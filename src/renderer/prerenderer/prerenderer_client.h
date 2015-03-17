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

#ifndef CONTENT_NW_SRC_RENDERER_PRERENDERER_PRERENDERER_CLIENT_H_
#define CONTENT_NW_SRC_RENDERER_PRERENDERER_PRERENDERER_CLIENT_H_

#include "base/compiler_specific.h"
#include "content/public/renderer/render_view_observer.h"
#include "third_party/WebKit/public/web/WebPrerendererClient.h"

namespace prerender {

class PrerendererClient : public content::RenderViewObserver,
                          public blink::WebPrerendererClient {
 public:
  explicit PrerendererClient(content::RenderView* render_view);

 private:
   ~PrerendererClient() final;

  // Implements blink::WebPrerendererClient
  virtual void willAddPrerender(blink::WebPrerender* prerender) override;
};

}  // namespace prerender

#endif  // CONTENT_NW_SRC_RENDERER_PRERENDERER_PRERENDERER_CLIENT_H_

