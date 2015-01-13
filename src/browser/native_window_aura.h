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
  static NativeWindowAura* GetBrowserViewForNativeWindow(gfx::NativeWindow window);

  SkRegion* draggable_region() { return draggable_region_.get(); }
  NativeWindowToolbarAura* toolbar() { return toolbar_; }
  views::Widget* window() { return window_; }

  BrowserViewLayout* GetBrowserViewLayout() const;

  // NativeWindow implementation.
  virtual void Close() override;
  virtual void Move(const gfx::Rect& pos) override;
  virtual void Focus(bool focus) override;
  virtual void Show() override;
  virtual void Hide() override;
  virtual void Maximize() override;
  virtual void Unmaximize() override;
  virtual void Minimize() override;
  virtual void Restore() override;
  virtual void SetFullscreen(bool fullscreen) override;
  virtual bool IsFullscreen() override;
  virtual void SetTransparent(bool transparent) override;
  virtual void SetSize(const gfx::Size& size) override;
  virtual gfx::Size GetSize() override;
  virtual void SetMinimumSize(int width, int height) override;
  virtual void SetMaximumSize(int width, int height) override;
  virtual void SetResizable(bool resizable) override;
  virtual void SetAlwaysOnTop(bool top) override;
  virtual void SetShowInTaskbar(bool show = true) override;
  virtual void SetVisibleOnAllWorkspaces(bool all_workspaces) override;
  virtual void SetPosition(const std::string& position) override;
  virtual void SetPosition(const gfx::Point& position) override;
  virtual gfx::Point GetPosition() override;
  virtual void SetTitle(const std::string& title) override;
  virtual void FlashFrame(int count) override;
  virtual void SetKiosk(bool kiosk) override;
  virtual void SetBadgeLabel(const std::string& badge) override;
  virtual void SetProgressBar(double progress) override;
  virtual bool IsKiosk() override;
  virtual void SetMenu(nwapi::Menu* menu) override;
  virtual void SetToolbarButtonEnabled(TOOLBAR_BUTTON button,
                                       bool enabled) override;
  virtual void SetToolbarUrlEntry(const std::string& url) override;
  virtual void SetToolbarIsLoading(bool loading) override;
  virtual void SetInitialFocus(bool initial_focus) override;
  virtual bool InitialFocus() override;

  // WidgetDelegate implementation.
  virtual void OnWidgetMove() override;
  virtual views::View* GetContentsView() override;
  virtual views::ClientView* CreateClientView(views::Widget*) override;
  virtual views::NonClientFrameView* CreateNonClientFrameView(
      views::Widget* widget) override;
  virtual bool CanResize() const override;
  virtual bool CanMaximize() const override;
  virtual bool CanMinimize() const override;
  virtual views::Widget* GetWidget() override;
  virtual const views::Widget* GetWidget() const override;
  virtual base::string16 GetWindowTitle() const override;
  virtual void DeleteDelegate() override;
  virtual views::View* GetInitiallyFocusedView() override;
  virtual gfx::ImageSkia GetWindowAppIcon() override;
  virtual gfx::ImageSkia GetWindowIcon() override;
  virtual bool ShouldShowWindowTitle() const override;
  virtual bool ShouldHandleOnSize()    const override;
  virtual void HandleWMStateUpdate() override;

  views::UnhandledKeyboardEventHandler unhandled_keyboard_event_handler_;



  // WidgetFocusChangeListener implementation.
  virtual void OnNativeFocusChange(gfx::NativeView focused_before,
                                   gfx::NativeView focused_now) override;

  // WidgetObserver implementation
  virtual void OnWidgetBoundsChanged(views::Widget* widget, const gfx::Rect& new_bounds) override;
  virtual void OnWidgetActivationChanged(views::Widget* widget,
                                         bool active) override;

  virtual bool AcceleratorPressed(const ui::Accelerator& accelerator) override;

  virtual bool CanHandleAccelerators() const override;

  virtual gfx::NativeView GetHostView() const override;
  virtual gfx::Point GetDialogPosition(const gfx::Size& size) override;
  virtual void AddObserver(web_modal::ModalDialogHostObserver* observer) override;
  virtual void RemoveObserver(web_modal::ModalDialogHostObserver* observer) override;
  virtual gfx::Size GetMaximumDialogSize() override;

 protected:
  // NativeWindow implementation.
  virtual void AddToolbar() override;
  virtual void UpdateDraggableRegions(
      const std::vector<extensions::DraggableRegion>& regions) override;
  virtual void HandleKeyboardEvent(
      const content::NativeWebKeyboardEvent& event) override;

  // views::View implementation.
  // virtual void Layout() override;
  virtual void ViewHierarchyChanged(
      const ViewHierarchyChangedDetails& details) override;
  virtual gfx::Size GetMinimumSize() const override;
  virtual gfx::Size GetMaximumSize() const override;
  virtual void OnFocus() override;

  // views::WidgetDelegate implementation.
  virtual bool ExecuteWindowsCommand(int command_id) override;
  virtual bool HandleSize(unsigned int param, const gfx::Size& size) override;
  virtual bool ExecuteAppCommand(int command_id) override;
  virtual void SaveWindowPlacement(const gfx::Rect& bounds,
                                   ui::WindowShowState show_state) override;
  virtual bool ShouldDescendIntoChildForEventHandling(
        gfx::NativeView child,
        const gfx::Point& location) override;
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
