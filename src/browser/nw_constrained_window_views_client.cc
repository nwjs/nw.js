// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/nw_constrained_window_views_client.h"

#include "base/memory/scoped_ptr.h"
#include "base/observer_list.h"
#include "components/constrained_window/constrained_window_views.h"
#include "components/constrained_window/constrained_window_views_client.h"
#include "components/web_modal/web_contents_modal_dialog_host.h"
#include "extensions/browser/guest_view/guest_view_base.h"
#include "ui/aura/window.h"
#include "ui/aura/window_observer.h"
#include "ui/aura/window_property.h"

namespace nw {
namespace {

// Provides the host environment for web modal dialogs. See
// web_modal::WebContentsModalDialogHost, and ModalDialogHost for more
// details.
class ModalDialogHostImpl : public web_modal::WebContentsModalDialogHost,
                            public aura::WindowObserver {
 public:
  // Returns a modal dialog host for |window|. If it doesn't exist it creates
  // one and stores it as owned property.
  static ModalDialogHost* Get(aura::Window* window);

 private:
  explicit ModalDialogHostImpl(aura::Window* host_window)
      : host_window_(host_window) {
    host_window_->AddObserver(this);
  }
  ~ModalDialogHostImpl() override {}

  // web_modal::ModalDialogHost:
  gfx::NativeView GetHostView() const override {
    return host_window_;
  }
  gfx::Point GetDialogPosition(const gfx::Size& size) override {
    gfx::Size app_window_size = host_window_->GetBoundsInScreen().size();
    return gfx::Point(app_window_size.width() / 2 - size.width() / 2,
                    app_window_size.height() / 2 - size.height() / 2);
  }
  void AddObserver(web_modal::ModalDialogHostObserver* observer) override {
    observer_list_.AddObserver(observer);
  }
  void RemoveObserver(web_modal::ModalDialogHostObserver* observer) override {
    observer_list_.RemoveObserver(observer);
  }

  // web_modal::WebContensModalDialogHost:
  gfx::Size GetMaximumDialogSize() override {
    return host_window_->bounds().size();
  }

  // aura::WindowObserver:
  void OnWindowDestroying(aura::Window* window) override {
    if (window != host_window_)
      return;
    host_window_->RemoveObserver(this);
    FOR_EACH_OBSERVER(web_modal::ModalDialogHostObserver,
                      observer_list_,
                      OnHostDestroying());
  }
  void OnWindowBoundsChanged(aura::Window* window,
                             const gfx::Rect& old_bounds,
                             const gfx::Rect& new_bounds) override {
    if (window != host_window_)
      return;
    FOR_EACH_OBSERVER(web_modal::ModalDialogHostObserver,
                      observer_list_,
                      OnPositionRequiresUpdate());
  }

  aura::Window* host_window_;
  ObserverList<web_modal::ModalDialogHostObserver> observer_list_;

  DISALLOW_COPY_AND_ASSIGN(ModalDialogHostImpl);
};

// A window property key to store the modal dialog host for
// dialogs created with the window as its parent.
DEFINE_OWNED_WINDOW_PROPERTY_KEY(web_modal::ModalDialogHost,
                                 kModalDialogHostKey,
                                 nullptr);

// static
web_modal::ModalDialogHost* ModalDialogHostImpl::Get(
    aura::Window* window) {
  web_modal::ModalDialogHost* host = window->GetProperty(kModalDialogHostKey);
  if (!host) {
    host = new ModalDialogHostImpl(window);
    window->SetProperty(kModalDialogHostKey, host);
  }
  return host;
}

class NWConstrainedWindowViewsClient
    : public constrained_window::ConstrainedWindowViewsClient {
 public:
  NWConstrainedWindowViewsClient() {}
  ~NWConstrainedWindowViewsClient() override {}

 private:
  // ConstrainedWindowViewsClient:
  content::WebContents* GetEmbedderWebContents(
      content::WebContents* initiator_web_contents) override {
    extensions::GuestViewBase* guest_view =
        extensions::GuestViewBase::FromWebContents(initiator_web_contents);
    return guest_view && guest_view->embedder_web_contents() ?
        guest_view->embedder_web_contents() : initiator_web_contents;
  }
  web_modal::ModalDialogHost* GetModalDialogHost(
      gfx::NativeWindow parent) override {
    return ModalDialogHostImpl::Get(parent);
  }
  gfx::NativeView GetDialogHostView(gfx::NativeWindow parent) override {
    return parent;
  }

  DISALLOW_COPY_AND_ASSIGN(NWConstrainedWindowViewsClient);
};

}  // namespace

void InstallConstrainedWindowViewsClient() {
  constrained_window::SetConstrainedWindowViewsClient(
      make_scoped_ptr(new NWConstrainedWindowViewsClient));
}

void UninstallConstrainedWindowViewsClient() {
  constrained_window::SetConstrainedWindowViewsClient(nullptr);
}

}  // namespace athena
