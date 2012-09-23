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

#include "content/nw/src/api/utils.h"

#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/renderer/render_view.h"
#include "content/nw/src/shell.h"
#include "content/nw/src/shell_content_browser_client.h"
#include "content/renderer/render_view_impl.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebDocument.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebFrame.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebView.h"

using WebKit::WebDocument;
using WebKit::WebFrame;
using WebKit::WebView;
using namespace v8;

namespace api {

Handle<v8::Value> CallMethod(Handle<v8::Value> recv,
                         const char* method,
                         int argc,
                         Handle<v8::Value> argv[]) {
  Handle<Object> object = recv->ToObject();
  Handle<Function> func = Handle<Function>::Cast(
      object->Get(String::New(method)));
  return func->Call(object, argc, argv);
}

int GetCurrentRenderViewInRenderer() {
  WebFrame* webframe = WebFrame::frameForCurrentContext();
  DCHECK(webframe) << "RetrieveCurrentFrame called when not in a V8 context.";
  if (!webframe)
    return -1;

  WebView* webview = webframe->view();
  if (!webview)
    return -1;  // can happen during closing

  RenderViewImpl* render_view = RenderViewImpl::FromWebView(webview);
  DCHECK(render_view) << "Encountered a WebView without a WebViewDelegate";

  return render_view->routing_id();
}

content::Shell* GetShellFromRenderViewInUI(int render_view_id) {
  // Get RenderViewHost from render thread.
  int render_process_id = static_cast<content::ShellContentBrowserClient*>(
      content::GetContentClient()->browser())->render_process_id();
  content::RenderViewHost* rvh = content::RenderViewHost::FromID(
      render_process_id, render_view_id);

  DCHECK(rvh) << "Cannot get a valid RenderViewHost.";
  return content::Shell::FromRenderViewHost(rvh);
}

}  // api
