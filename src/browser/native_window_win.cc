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

#include "content/nw/src/browser/native_window_win.h"

#include "base/utf_string_conversions.h"
#include "base/values.h"
#include "base/win/windows_version.h"
#include "base/win/wrapped_window_proc.h"
#include "chrome/browser/platform_util.h"
#include "chrome/common/extensions/draggable_region.h"
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/browser/native_window_toolbar_win.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/nw_shell.h"
#include "content/public/browser/native_web_keyboard_event.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/web_contents.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "ui/base/hit_test.h"
#include "ui/base/win/hwnd_util.h"
#include "ui/gfx/path.h"
#include "ui/views/controls/webview/webview.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/views_delegate.h"
#include "ui/views/widget/widget.h"
#include "ui/views/widget/native_widget_win.h"
#include "ui/views/window/native_frame_view.h"

#include <time.h>
#include <stdlib.h>
#include <Dwmapi.h>

namespace nw {

namespace {

const int kResizeInsideBoundsSize = 5;
const int kResizeAreaCornerSize = 16;

// Returns true if |possible_parent| is a parent window of |child|.
bool IsParent(gfx::NativeView child, gfx::NativeView possible_parent) {
  if (!child)
    return false;
#if !defined(USE_AURA) && defined(OS_WIN)
  if (::GetWindow(child, GW_OWNER) == possible_parent)
    return true;
#endif
  gfx::NativeView parent = child;
  while ((parent = platform_util::GetParent(parent))) {
    if (possible_parent == parent)
      return true;
  }

  return false;
}

class NativeWindowClientView : public views::ClientView {
 public:
  NativeWindowClientView(views::Widget* widget,
                         views::View* contents_view,
                         content::Shell* shell)
      : views::ClientView(widget, contents_view),
        shell_(shell) {
  }
  virtual ~NativeWindowClientView() {}

  virtual bool CanClose() OVERRIDE {
    return shell_->ShouldCloseWindow();
  }

 private:
  content::Shell* shell_;
};

class NativeWindowFrameView : public views::NonClientFrameView {
 public:
  static const char kViewClassName[];

  explicit NativeWindowFrameView(NativeWindowWin* window);
  virtual ~NativeWindowFrameView();

  void Init(views::Widget* frame);

  // views::NonClientFrameView implementation.
  virtual gfx::Rect GetBoundsForClientView() const OVERRIDE;
  virtual gfx::Rect GetWindowBoundsForClientBounds(
      const gfx::Rect& client_bounds) const OVERRIDE;
  virtual int NonClientHitTest(const gfx::Point& point) OVERRIDE;
  virtual void GetWindowMask(const gfx::Size& size,
                             gfx::Path* window_mask) OVERRIDE;
  virtual void ResetWindowControls() OVERRIDE {}
  virtual void UpdateWindowIcon() OVERRIDE {}
  virtual void UpdateWindowTitle() OVERRIDE {}

  // views::View implementation.
  virtual gfx::Size GetPreferredSize() OVERRIDE;
  virtual void Layout() OVERRIDE;
  virtual std::string GetClassName() const OVERRIDE;
  virtual void OnPaint(gfx::Canvas* canvas) OVERRIDE;
  virtual gfx::Size GetMinimumSize() OVERRIDE;
  virtual gfx::Size GetMaximumSize() OVERRIDE;

 private:
  NativeWindowWin* window_;
  views::Widget* frame_;

  DISALLOW_COPY_AND_ASSIGN(NativeWindowFrameView);
};

const char NativeWindowFrameView::kViewClassName[] =
    "content/nw/src/browser/NativeWindowFrameView";

NativeWindowFrameView::NativeWindowFrameView(NativeWindowWin* window)
    : window_(window),
      frame_(NULL) {
}

NativeWindowFrameView::~NativeWindowFrameView() {
}

void NativeWindowFrameView::Init(views::Widget* frame) {
  frame_ = frame;
}

gfx::Rect NativeWindowFrameView::GetBoundsForClientView() const {
  return bounds();
}

gfx::Rect NativeWindowFrameView::GetWindowBoundsForClientBounds(
      const gfx::Rect& client_bounds) const {
  gfx::Rect window_bounds = client_bounds;
  // Enforce minimum size (1, 1) in case that client_bounds is passed with
  // empty size. This could occur when the frameless window is being
  // initialized.
  if (window_bounds.IsEmpty()) {
    window_bounds.set_width(1);
    window_bounds.set_height(1);
  }
  return window_bounds;
}

int NativeWindowFrameView::NonClientHitTest(const gfx::Point& point) {
  if (frame_->IsFullscreen())
    return HTCLIENT;

  // Check the frame first, as we allow a small area overlapping the contents
  // to be used for resize handles.
  bool can_ever_resize = frame_->widget_delegate() ?
      frame_->widget_delegate()->CanResize() :
      false;
  // Don't allow overlapping resize handles when the window is maximized or
  // fullscreen, as it can't be resized in those states.
  int resize_border =
      frame_->IsMaximized() || frame_->IsFullscreen() ? 0 :
      kResizeInsideBoundsSize;
  int frame_component = GetHTComponentForFrame(point,
                                               resize_border,
                                               resize_border,
                                               kResizeAreaCornerSize,
                                               kResizeAreaCornerSize,
                                               can_ever_resize);
  if (frame_component != HTNOWHERE)
    return frame_component;

  // Ajust the point if we have a toolbar.
  gfx::Point adjusted_point(point);
  if (window_->toolbar())
    adjusted_point.set_y(adjusted_point.y() - window_->toolbar()->height());

  // Check for possible draggable region in the client area for the frameless
  // window.
  if (window_->draggable_region() &&
      window_->draggable_region()->contains(adjusted_point.x(),
                                            adjusted_point.y()))
    return HTCAPTION;

  int client_component = frame_->client_view()->NonClientHitTest(point);
  if (client_component != HTNOWHERE)
    return client_component;

  // Caption is a safe default.
  return HTCAPTION;
}

void NativeWindowFrameView::GetWindowMask(const gfx::Size& size,
                                         gfx::Path* window_mask) {
  // We got nothing to say about no window mask.
}

gfx::Size NativeWindowFrameView::GetPreferredSize() {
  gfx::Size pref = frame_->client_view()->GetPreferredSize();
  gfx::Rect bounds(0, 0, pref.width(), pref.height());
  return frame_->non_client_view()->GetWindowBoundsForClientBounds(
      bounds).size();
}

void NativeWindowFrameView::Layout() {
}

void NativeWindowFrameView::OnPaint(gfx::Canvas* canvas) {
}

std::string NativeWindowFrameView::GetClassName() const {
  return kViewClassName;
}

gfx::Size NativeWindowFrameView::GetMinimumSize() {
  return frame_->client_view()->GetMinimumSize();
}

gfx::Size NativeWindowFrameView::GetMaximumSize() {
  return frame_->client_view()->GetMaximumSize();
}

}  // namespace

NativeWindowWin::NativeWindowWin(content::Shell* shell,
                                 base::DictionaryValue* manifest)
    : NativeWindow(shell, manifest),
      web_view_(NULL),
      toolbar_(NULL),
      is_fullscreen_(false),
      is_transparent_(false),
      is_minimized_(false),
      is_focus_(false),
      is_blur_(false),
      menu_(NULL),
      resizable_(true),
      minimum_size_(0, 0),
      maximum_size_() {
  window_ = new views::Widget;
  views::Widget::InitParams params(views::Widget::InitParams::TYPE_WINDOW);
  params.delegate = this;
  params.remove_standard_frame = !has_frame();
  params.use_system_default_icon = true;
  window_->Init(params);

  views::WidgetFocusManager::GetInstance()->AddFocusChangeListener(this);

  int width, height;
  manifest->GetInteger(switches::kmWidth, &width);
  manifest->GetInteger(switches::kmHeight, &height);
  gfx::Rect window_bounds = 
    window_->non_client_view()->GetWindowBoundsForClientBounds(
        gfx::Rect(width,height));
  window_->SetSize(window_bounds.size());
  window_->CenterWindow(window_bounds.size());
  window_->UpdateWindowIcon();

  OnViewWasResized();
}

NativeWindowWin::~NativeWindowWin() {
  views::WidgetFocusManager::GetInstance()->RemoveFocusChangeListener(this);
}

void NativeWindowWin::Close() {
  window_->Close();
}

void NativeWindowWin::Move(const gfx::Rect& bounds) {
  window_->SetBounds(bounds);

  if(IsTransparent()) {
    MARGINS mgMarInset = { -1, -1, -1, -1 };
    DwmExtendFrameIntoClientArea(window_->GetNativeWindow(), &mgMarInset);
  }
}

void NativeWindowWin::Focus(bool focus) {
  window_->Activate();
}

void NativeWindowWin::Show() {
  window_->Show();

  // We have to re-establish our composition by shaking the compositing surface
  // TODO: Find a better way of doing this.
  if(IsTransparent()) {
    Maximize();
    Unmaximize();
  }
}

void NativeWindowWin::Hide() {
  window_->Hide();
}

void NativeWindowWin::Maximize() {
  window_->Maximize();
}

void NativeWindowWin::Unmaximize() {
  window_->Restore();
}

void NativeWindowWin::Minimize() {
  window_->Minimize();
}

void NativeWindowWin::Restore() {
  window_->Restore();
}

void NativeWindowWin::SetFullscreen(bool fullscreen) {
  is_fullscreen_ = fullscreen;
  window_->SetFullscreen(fullscreen);
  if (fullscreen)
    shell()->SendEvent("enter-fullscreen");
  else
    shell()->SendEvent("leave-fullscreen");
}

bool NativeWindowWin::IsFullscreen() {
  return is_fullscreen_;
}

void NativeWindowWin::SetTransparent() {
  is_transparent_ = true;
  
  // Check for Windows Vista or higher, transparency isn't supported in 
  // anything lower. 
  if (base::win::GetVersion() < base::win::VERSION_VISTA) {
    NOTREACHED() << "The operating system does not support transparency.";
    is_transparent_ = false;
    return;
  }

  // Check to see if composition is disabled, if so we have to throw an 
  // error, there's no graceful recovery, yet. TODO: Graceful recovery.
  BOOL enabled = FALSE;
  HRESULT result = ::DwmIsCompositionEnabled(&enabled);
  if (!enabled || !SUCCEEDED(result)) {
    NOTREACHED() << "Windows DWM composition is not enabled, transparency is not supported.";
    is_transparent_ = false;
    return;
  }

  // These override any other window settings, which isn't the greatest idea
  // however transparent windows (in Windows) are very tricky and are not 
  // usable with any other styles.
  SetWindowLong(window_->GetNativeWindow(), GWL_STYLE, WS_POPUP | WS_SYSMENU | WS_BORDER); 
  SetWindowLong(window_->GetNativeWindow(), GWL_EXSTYLE , WS_EX_COMPOSITED);

  MARGINS mgMarInset = { -1, -1, -1, -1 };
  if(DwmExtendFrameIntoClientArea(window_->GetNativeWindow(), &mgMarInset) != S_OK) {
    NOTREACHED() << "Windows DWM extending to client area failed, transparency is not supported.";
    is_transparent_ = false;
    return;
  }

  // Send a message to swap frames and refresh contexts
  SetWindowPos(window_->GetNativeWindow(), NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

bool NativeWindowWin::IsTransparent() {
  return is_transparent_;
}

void NativeWindowWin::SetSize(const gfx::Size& size) {
  window_->SetSize(size);
}

gfx::Size NativeWindowWin::GetSize() {
  return window_->GetWindowBoundsInScreen().size();
}

void NativeWindowWin::SetMinimumSize(int width, int height) {
  minimum_size_.set_width(width);
  minimum_size_.set_height(height);
}

void NativeWindowWin::SetMaximumSize(int width, int height) {
  maximum_size_.set_width(width);
  maximum_size_.set_height(height);
}

void NativeWindowWin::SetResizable(bool resizable) {
  resizable_ = resizable;

  // Show/Hide the maximize button.
  DWORD style = ::GetWindowLong(window_->GetNativeView(), GWL_STYLE);
  if (resizable)
    style |= WS_MAXIMIZEBOX;
  else
    style &= ~WS_MAXIMIZEBOX;
  ::SetWindowLong(window_->GetNativeView(), GWL_STYLE, style);
}

void NativeWindowWin::SetAlwaysOnTop(bool top) {
  window_->SetAlwaysOnTop(top);
}

void NativeWindowWin::SetPosition(const std::string& position) {
  if (position == "center") {
    gfx::Rect bounds = window_->GetWindowBoundsInScreen();
    window_->CenterWindow(gfx::Size(bounds.width(), bounds.height()));
  }
}

void NativeWindowWin::SetPosition(const gfx::Point& position) {
  gfx::Rect bounds = window_->GetWindowBoundsInScreen();
  window_->SetBounds(gfx::Rect(position, bounds.size()));

  if(IsTransparent()) {
    MARGINS mgMarInset = { -1, -1, -1, -1 };
    DwmExtendFrameIntoClientArea(window_->GetNativeWindow(), &mgMarInset);
  }
}

gfx::Point NativeWindowWin::GetPosition() {
  return window_->GetWindowBoundsInScreen().origin();
}

gfx::Point NativeWindowWin::GetMousePosition() {
  POINT p;
  GetCursorPos(&p);
  return gfx::Point(p.x,p.y);
}

void NativeWindowWin::BeginOffclientMouseMove() {
  SetCapture(window_->GetNativeWindow());
}

void NativeWindowWin::EndOffclientMouseMove() {
  ReleaseCapture();
}

void NativeWindowWin::FlashFrame(bool flash) {
  window_->FlashFrame(flash);
}

void NativeWindowWin::SetKiosk(bool kiosk) {
  SetFullscreen(kiosk);
}

bool NativeWindowWin::IsKiosk() {
  return IsFullscreen();
}

void NativeWindowWin::SetMenu(api::Menu* menu) {
  window_->set_has_menu_bar(true);
  menu_ = menu;

  // The menu is lazily built.
  menu->Rebuild();

  // menu is api::Menu, menu->menu_ is NativeMenuWin,
  ::SetMenu(window_->GetNativeWindow(), menu->menu_->GetNativeMenu());
}

void NativeWindowWin::SetTitle(const std::string& title) {
  title_ = title;
  window_->UpdateWindowTitle();
}

void NativeWindowWin::AddToolbar() {
  toolbar_ = new NativeWindowToolbarWin(shell());
  AddChildViewAt(toolbar_, 0);
}

void NativeWindowWin::SetToolbarButtonEnabled(TOOLBAR_BUTTON button,
                                              bool enabled) {
  if (toolbar_)
    toolbar_->SetButtonEnabled(button, enabled);
}

void NativeWindowWin::SetToolbarUrlEntry(const std::string& url) {
  if (toolbar_)
    toolbar_->SetUrlEntry(url);
}
  
void NativeWindowWin::SetToolbarIsLoading(bool loading) {
  if (toolbar_)
    toolbar_->SetIsLoading(loading);
}

views::View* NativeWindowWin::GetContentsView() {
  return this;
}

views::ClientView* NativeWindowWin::CreateClientView(views::Widget* widget) {
  return new NativeWindowClientView(widget, GetContentsView(), shell());
}

views::NonClientFrameView* NativeWindowWin::CreateNonClientFrameView(
    views::Widget* widget) {
  if (has_frame())
    return new views::NativeFrameView(GetWidget());

  NativeWindowFrameView* frame_view = new NativeWindowFrameView(this);
  frame_view->Init(window_);
  return frame_view;
}

bool NativeWindowWin::CanResize() const {
  return resizable_;
}

bool NativeWindowWin::CanMaximize() const {
  return resizable_;
}

views::Widget* NativeWindowWin::GetWidget() {
  return window_;
}

const views::Widget* NativeWindowWin::GetWidget() const {
  return window_;
}

string16 NativeWindowWin::GetWindowTitle() const {
  return UTF8ToUTF16(title_);
}

void NativeWindowWin::DeleteDelegate() {
  delete shell();
}

bool NativeWindowWin::ShouldShowWindowTitle() const {
  return has_frame();
}

void NativeWindowWin::OnNativeFocusChange(gfx::NativeView focused_before,
                                          gfx::NativeView focused_now) {
  gfx::NativeView this_window = GetWidget()->GetNativeView();
  if (IsParent(focused_now, this_window) ||
      IsParent(this_window, focused_now))
    return;

  if (focused_now == this_window) {
    if (!is_focus_)
      shell()->SendEvent("focus");
    is_focus_ = true;
    is_blur_ = false;
  } else if (focused_before == this_window) {
    if (!is_blur_)
      shell()->SendEvent("blur");
    is_focus_ = false;
    is_blur_ = true;
  }
}

gfx::ImageSkia NativeWindowWin::GetWindowAppIcon() {
  gfx::Image icon = app_icon();
  if (icon.IsEmpty())
    return gfx::ImageSkia();

  return *icon.ToImageSkia();
}

gfx::ImageSkia NativeWindowWin::GetWindowIcon() {
  return GetWindowAppIcon();
}

views::View* NativeWindowWin::GetInitiallyFocusedView() {
  return web_view_;
}

void NativeWindowWin::UpdateDraggableRegions(
    const std::vector<extensions::DraggableRegion>& regions) {
  // Draggable region is not supported for non-frameless window.
  if (has_frame())
    return;

  SkRegion* draggable_region = new SkRegion;

  // By default, the whole window is non-draggable. We need to explicitly
  // include those draggable regions.
  for (std::vector<extensions::DraggableRegion>::const_iterator iter =
           regions.begin();
       iter != regions.end(); ++iter) {
    const extensions::DraggableRegion& region = *iter;
    draggable_region->op(
        region.bounds.x(),
        region.bounds.y(),
        region.bounds.right(),
        region.bounds.bottom(),
        region.draggable ? SkRegion::kUnion_Op : SkRegion::kDifference_Op);
  }

  draggable_region_.reset(draggable_region);
  OnViewWasResized();
}

void NativeWindowWin::HandleKeyboardEvent(
    const content::NativeWebKeyboardEvent& event) {
  // Any unhandled keyboard/character messages should be defproced.
  // This allows stuff like F10, etc to work correctly.
  DefWindowProc(event.os_event.hwnd, event.os_event.message,
                event.os_event.wParam, event.os_event.lParam);
}

void NativeWindowWin::Layout() {
  DCHECK(web_view_);
  if (toolbar_) {
    toolbar_->SetBounds(0, 0, width(), 34);
    web_view_->SetBounds(0, 34, width(), height() - 34);
  } else {
    web_view_->SetBounds(0, 0, width(), height());
  }
  OnViewWasResized();
}

void NativeWindowWin::ViewHierarchyChanged(
    bool is_add, views::View *parent, views::View *child) {
  if (is_add && child == this) {
    views::BoxLayout* layout = new views::BoxLayout(
        views::BoxLayout::kVertical, 0, 0, 0);
    SetLayoutManager(layout);

    web_view_ = new views::WebView(NULL);
    web_view_->SetWebContents(web_contents());
    AddChildView(web_view_);
  }
}

gfx::Size NativeWindowWin::GetMinimumSize() {
  return minimum_size_;
}

gfx::Size NativeWindowWin::GetMaximumSize() {
  return maximum_size_;
}

void NativeWindowWin::OnFocus() {
  web_view_->RequestFocus();
}

bool NativeWindowWin::ExecuteWindowsCommand(int command_id) {
  // Windows uses the 4 lower order bits of |command_id| for type-specific
  // information so we must exclude this when comparing.
  static const int sc_mask = 0xFFF0;

  if ((command_id & sc_mask) == SC_MINIMIZE) {
    is_minimized_ = true;
    shell()->SendEvent("minimize");
  } else if ((command_id & sc_mask) == SC_RESTORE && is_minimized_) {
    is_minimized_ = false;
    shell()->SendEvent("restore");
  } else if ((command_id & sc_mask) == SC_RESTORE && !is_minimized_) {
    shell()->SendEvent("unmaximize");
  } else if ((command_id & sc_mask) == SC_MAXIMIZE) {
    shell()->SendEvent("maximize");
  }

  return false;
}

bool NativeWindowWin::ExecuteAppCommand(int command_id) {
  if (menu_) {
    menu_->menu_delegate_->ExecuteCommand(command_id);
    menu_->menu_->UpdateStates();
  }

  return false;
}

void NativeWindowWin::SaveWindowPlacement(const gfx::Rect& bounds,
                                          ui::WindowShowState show_state) {
  // views::WidgetDelegate::SaveWindowPlacement(bounds, show_state);
}
  
void NativeWindowWin::OnViewWasResized() {
  // Set the window shape of the RWHV.
  DCHECK(window_);
  DCHECK(web_view_);
  gfx::Size sz = web_view_->size();
  int height = sz.height(), width = sz.width();
  gfx::Path path;
  path.addRect(0, 0, width, height);
  SetWindowRgn(web_contents()->GetNativeView(), path.CreateNativeRegion(), 1);

  SkRegion* rgn = new SkRegion;
  if (!window_->IsFullscreen()) {
    if (draggable_region())
      rgn->op(*draggable_region(), SkRegion::kUnion_Op);
    if (!window_->IsMaximized()) {
      if (!has_frame()) {
        rgn->op(0, 0, width, kResizeInsideBoundsSize, SkRegion::kUnion_Op);
        rgn->op(0, 0, kResizeInsideBoundsSize, height, SkRegion::kUnion_Op);
        rgn->op(width - kResizeInsideBoundsSize, 0, width, height,
            SkRegion::kUnion_Op);
        rgn->op(0, height - kResizeInsideBoundsSize, width, height,
            SkRegion::kUnion_Op);
      }
    }
  }
  if (web_contents()->GetRenderViewHost()->GetView())
    web_contents()->GetRenderViewHost()->GetView()->SetClickthroughRegion(rgn);
}

}  // namespace nw
