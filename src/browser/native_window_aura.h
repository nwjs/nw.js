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

#ifndef CONTENT_NW_SRC_BROWSER_NATIVE_WINDOW_WIN_H_
#define CONTENT_NW_SRC_BROWSER_NATIVE_WINDOW_WIN_H_

#include "content/nw/src/browser/native_window.h"

#include "third_party/skia/include/core/SkRegion.h"
#if defined(OS_WIN)
#include "ui/base/win/hidden_window.h"
#endif
#include "ui/gfx/image/image_skia.h"
#include "ui/gfx/rect.h"
#include "ui/views/focus/widget_focus_manager.h"
#include "ui/views/widget/widget_delegate.h"
#include "ui/views/widget/widget_observer.h"

#include "ui/base/accelerators/accelerator.h"
#include "ui/base/accelerators/accelerator_manager.h"
#include "ui/views/controls/webview/unhandled_keyboard_event_handler.h"
#include "ui/events/keycodes/keyboard_codes.h"

namespace views {
class WebView;
}

namespace nwapi {
class Menu;
}

namespace nw {

class BrowserViewLayout;
class NativeWindowToolbarAura;

class NativeWindowAura : public NativeWindow,
                        public views::WidgetFocusChangeListener,
                        public views::WidgetDelegateView,
                        public views::WidgetObserver {
 public:
  explicit NativeWindowAura(const base::WeakPtr<content::Shell>& shell,
                           base::DictionaryValue* manifest);
  virtual ~NativeWindowAura();

  SkRegion* draggable_region() { return draggable_region_.get(); }
  NativeWindowToolbarAura* toolbar() { return toolbar_; }
  views::Widget* window() { return window_; }

  BrowserViewLayout* GetBrowserViewLayout() const;

  // NativeWindow implementation.
  virtual void Close() OVERRIDE;
  virtual void Move(const gfx::Rect& pos) OVERRIDE;
  virtual void Focus(bool focus) OVERRIDE;
  virtual void Show() OVERRIDE;
  virtual void Hide() OVERRIDE;
  virtual void Maximize() OVERRIDE;
  virtual void Unmaximize() OVERRIDE;
  virtual void Minimize() OVERRIDE;
  virtual void Restore() OVERRIDE;
  virtual void SetFullscreen(bool fullscreen) OVERRIDE;
  virtual bool IsFullscreen() OVERRIDE;
  virtual void SetTransparent(bool transparent) OVERRIDE;
  virtual void SetSize(const gfx::Size& size) OVERRIDE;
  virtual gfx::Size GetSize() OVERRIDE;
  virtual void SetMinimumSize(int width, int height) OVERRIDE;
  virtual void SetMaximumSize(int width, int height) OVERRIDE;
  virtual void SetResizable(bool resizable) OVERRIDE;
  virtual void SetAlwaysOnTop(bool top) OVERRIDE;
  virtual void SetShowInTaskbar(bool show = true) OVERRIDE;
  virtual void SetVisibleOnAllWorkspaces(bool all_workspaces) OVERRIDE;
  virtual void SetPosition(const std::string& position) OVERRIDE;
  virtual void SetPosition(const gfx::Point& position) OVERRIDE;
  virtual gfx::Point GetPosition() OVERRIDE;
  virtual void SetTitle(const std::string& title) OVERRIDE;
  virtual void FlashFrame(int count) OVERRIDE;
  virtual void SetKiosk(bool kiosk) OVERRIDE;
  virtual void SetBadgeLabel(const std::string& badge) OVERRIDE;
  virtual void SetProgressBar(double progress) OVERRIDE;
  virtual bool IsKiosk() OVERRIDE;
  virtual void SetMenu(nwapi::Menu* menu) OVERRIDE;
  virtual void SetToolbarButtonEnabled(TOOLBAR_BUTTON button,
                                       bool enabled) OVERRIDE;
  virtual void SetToolbarUrlEntry(const std::string& url) OVERRIDE;
  virtual void SetToolbarIsLoading(bool loading) OVERRIDE;
  virtual void SetInitialFocus(bool initial_focus) OVERRIDE;
  virtual bool InitialFocus() OVERRIDE;

  // WidgetDelegate implementation.
  virtual void OnWidgetMove() OVERRIDE;
  virtual views::View* GetContentsView() OVERRIDE;
  virtual views::ClientView* CreateClientView(views::Widget*) OVERRIDE;
  virtual views::NonClientFrameView* CreateNonClientFrameView(
      views::Widget* widget) OVERRIDE;
  virtual bool CanResize() const OVERRIDE;
  virtual bool CanMaximize() const OVERRIDE;
  virtual views::Widget* GetWidget() OVERRIDE;
  virtual const views::Widget* GetWidget() const OVERRIDE;
  virtual base::string16 GetWindowTitle() const OVERRIDE;
  virtual void DeleteDelegate() OVERRIDE;
  virtual views::View* GetInitiallyFocusedView() OVERRIDE;
  virtual gfx::ImageSkia GetWindowAppIcon() OVERRIDE;
  virtual gfx::ImageSkia GetWindowIcon() OVERRIDE;
  virtual bool ShouldShowWindowTitle() const OVERRIDE;
  virtual bool ShouldHandleOnSize()    const OVERRIDE;
  virtual void HandleWMStateUpdate() OVERRIDE;

  views::UnhandledKeyboardEventHandler unhandled_keyboard_event_handler_;



  // WidgetFocusChangeListener implementation.
  virtual void OnNativeFocusChange(gfx::NativeView focused_before,
                                   gfx::NativeView focused_now) OVERRIDE;

  // WidgetObserver implementation
  virtual void OnWidgetBoundsChanged(views::Widget* widget, const gfx::Rect& new_bounds) OVERRIDE;
  virtual void OnWidgetActivationChanged(views::Widget* widget,
                                         bool active) OVERRIDE;

  virtual bool AcceleratorPressed(const ui::Accelerator& accelerator) OVERRIDE;

  virtual bool CanHandleAccelerators() const OVERRIDE;

  virtual gfx::NativeView GetHostView() const OVERRIDE;
  virtual gfx::Point GetDialogPosition(const gfx::Size& size) OVERRIDE;
  virtual void AddObserver(web_modal::ModalDialogHostObserver* observer) OVERRIDE;
  virtual void RemoveObserver(web_modal::ModalDialogHostObserver* observer) OVERRIDE;
  virtual gfx::Size GetMaximumDialogSize() OVERRIDE;

 protected:
  // NativeWindow implementation.
  virtual void AddToolbar() OVERRIDE;
  virtual void UpdateDraggableRegions(
      const std::vector<extensions::DraggableRegion>& regions) OVERRIDE;
  virtual void HandleKeyboardEvent(
      const content::NativeWebKeyboardEvent& event) OVERRIDE;

  // views::View implementation.
  // virtual void Layout() OVERRIDE;
  virtual void ViewHierarchyChanged(
      const ViewHierarchyChangedDetails& details) OVERRIDE;
  virtual gfx::Size GetMinimumSize() const OVERRIDE;
  virtual gfx::Size GetMaximumSize() const OVERRIDE;
  virtual void OnFocus() OVERRIDE;

  // views::WidgetDelegate implementation.
  virtual bool ExecuteWindowsCommand(int command_id) OVERRIDE;
  virtual bool HandleSize(unsigned int param, const gfx::Size& size) OVERRIDE;
  virtual bool ExecuteAppCommand(int command_id) OVERRIDE;
  virtual void SaveWindowPlacement(const gfx::Rect& bounds,
                                   ui::WindowShowState show_state) OVERRIDE;
  virtual bool ShouldDescendIntoChildForEventHandling(
        gfx::NativeView child,
        const gfx::Point& location) OVERRIDE;
 private:
  friend class content::Shell;
  friend class nwapi::Menu;

  void OnViewWasResized();
  void InstallEasyResizeTargeterOnContainer();

  NativeWindowToolbarAura* toolbar_;
  views::WebView* web_view_;
  views::Widget* window_;
  bool is_fullscreen_;
  bool is_visible_on_all_workspaces_;

  // Flags used to prevent sending extra events.
  bool is_minimized_;
  bool is_maximized_;
  bool is_focus_;
  bool is_blur_;

  scoped_ptr<SkRegion> draggable_region_;
  // The window's menubar.
  nwapi::Menu* menu_;

  bool resizable_;
  std::string title_;
  gfx::Size minimum_size_;
  gfx::Size maximum_size_;

  bool initial_focus_;

  int last_width_;
  int last_height_;

  bool super_down_;
  bool meta_down_;
  ObserverList<web_modal::ModalDialogHostObserver> observer_list_;
  DISALLOW_COPY_AND_ASSIGN(NativeWindowAura);
};

}  // namespace nw

#endif  // CONTENT_NW_SRC_BROWSER_NATIVE_WINDOW_WIN_H_
