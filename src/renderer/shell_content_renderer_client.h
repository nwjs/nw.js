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

#ifndef CONTENT_NW_SRC_RENDERER_SHELL_CONTENT_RENDERER_CLIENT_H_
#define CONTENT_NW_SRC_RENDERER_SHELL_CONTENT_RENDERER_CLIENT_H_

#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "content/public/renderer/content_renderer_client.h"
#include "v8/include/v8.h"

namespace nwapi {
class WindowBindings;
}

namespace autofill {
class PageClickTracker;
}
namespace content {

class ShellRenderProcessObserver;

class ShellContentRendererClient : public ContentRendererClient {
 public:
  ShellContentRendererClient();
  virtual ~ShellContentRendererClient();
  virtual void RenderThreadStarted() OVERRIDE;
  virtual void RenderViewCreated(RenderView* render_view) OVERRIDE;

  virtual void DidCreateScriptContext(WebKit::WebFrame* frame,
                                      v8::Handle<v8::Context> context,
                                      int extension_group,
                                      int world_id) OVERRIDE;
  virtual bool WillSetSecurityToken(WebKit::WebFrame* frame,
                                    v8::Handle<v8::Context>) OVERRIDE;

  virtual void willHandleNavigationPolicy(RenderView* rv,
                                          WebKit::WebFrame* frame,
                                          const WebKit::WebURLRequest& request,
                                          WebKit::WebNavigationPolicy* policy) OVERRIDE;

 private:
  scoped_ptr<ShellRenderProcessObserver> shell_observer_;
  scoped_ptr<nwapi::WindowBindings> window_bindings_;

  bool creating_first_context_;

  void InstallNodeSymbols(WebKit::WebFrame* frame,
                          v8::Handle<v8::Context> context, const GURL& url);
  void UninstallNodeSymbols(WebKit::WebFrame* frame,
                            v8::Handle<v8::Context> context);
  bool goodForNode(WebKit::WebFrame* frame);

  // Catch node uncaughtException.
  static void ReportException(const v8::FunctionCallbackInfo<v8::Value>&  args);

};

}  // namespace content

#endif  // CONTENT_NW_SRC_RENDERER_SHELL_CONTENT_RENDERER_CLIENT_H_
