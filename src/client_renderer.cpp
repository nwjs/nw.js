// Copyright (c) 2012 Intel Corp
// Copyright (c) 2012 The Chromium Embedded Framework Authors
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


#include "client_renderer.h"

#include <sstream>
#include <string>

#include "include/cef_dom.h"
#include "util.h"

namespace client_renderer {

const char kFocusedNodeChangedMessage[] = "ClientRenderer.FocusedNodeChanged";

namespace {

class ClientRenderDelegate : public ClientApp::RenderDelegate {
 public:
  ClientRenderDelegate()
    : last_node_is_editable_(false) {
  }

  virtual void OnFocusedNodeChanged(CefRefPtr<ClientApp> app,
                                    CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefDOMNode> node) OVERRIDE {
    bool is_editable = (node.get() && node->IsEditable());
    if (is_editable != last_node_is_editable_) {
      // Notify the browser of the change in focused element type.
      last_node_is_editable_ = is_editable;
      CefRefPtr<CefProcessMessage> message =
          CefProcessMessage::Create(kFocusedNodeChangedMessage);
      message->GetArgumentList()->SetBool(0, is_editable);
      browser->SendProcessMessage(PID_BROWSER, message);
    }
  }

 private:
  bool last_node_is_editable_;

  IMPLEMENT_REFCOUNTING(ClientRenderDelegate);
};

}  // namespace

void CreateRenderDelegates(ClientApp::RenderDelegateSet& delegates) {
  delegates.insert(new ClientRenderDelegate);
}

}  // namespace client_renderer
