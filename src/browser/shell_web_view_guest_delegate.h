// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NW_BROWSER_GUEST_VIEW_WEB_VIEW_CHROME_WEB_VIEW_GUEST_DELEGATE_H_
#define NW_BROWSER_GUEST_VIEW_WEB_VIEW_CHROME_WEB_VIEW_GUEST_DELEGATE_H_

#include "extensions/browser/guest_view/web_view/web_view_guest.h"
#include "extensions/browser/guest_view/web_view/web_view_guest_delegate.h"


namespace extensions {

class ShellWebViewGuestDelegate : public WebViewGuestDelegate {
 public :
  explicit ShellWebViewGuestDelegate(WebViewGuest* web_view_guest);
  ~ShellWebViewGuestDelegate() override;

  // WebViewGuestDelegate implementation.
  double GetZoom() override;
  bool HandleContextMenu(const content::ContextMenuParams& params) override;
  void OnAttachWebViewHelpers(content::WebContents* contents) override;
  void OnDidAttachToEmbedder() override;
  void OnDidCommitProvisionalLoadForFrame(bool is_main_frame) override;
  void OnDidInitialize() override;
  void OnDocumentLoadedInFrame(
      content::RenderFrameHost* render_frame_host) override;
  void OnEmbedderWillBeDestroyed() override;
  void OnGuestDestroyed() override;
  void OnSetZoom(double zoom_factor) override;
  void OnShowContextMenu(int request_id, const MenuItemVector* items) override;

  WebViewGuest* web_view_guest() const { return web_view_guest_; }

 private:
  content::WebContents* guest_web_contents() const {
    return web_view_guest()->web_contents();
  }

  WebViewGuest* const web_view_guest_;

  bool nw_disabled_;
  // This is used to ensure pending tasks will not fire after this object is
  // destroyed.
  base::WeakPtrFactory<ShellWebViewGuestDelegate> weak_ptr_factory_;

  DISALLOW_COPY_AND_ASSIGN(ShellWebViewGuestDelegate);
};

}  // namespace extensions

#endif  // CHROME_BROWSER_GUEST_VIEW_WEB_VIEW_CHROME_WEB_VIEW_GUEST_DELEGATE_H_

