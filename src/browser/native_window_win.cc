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
#include "base/win/wrapped_window_proc.h"
#include "chrome/common/extensions/draggable_region.h"
#include "content/nw/src/browser/native_window_toolbar_win.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/nw_shell.h"
#include "content/public/browser/native_web_keyboard_event.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host_view.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "ui/base/hit_test.h"
#include "ui/base/win/hwnd_util.h"
#include "ui/gfx/path.h"
#include "ui/views/controls/webview/webview.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/views_delegate.h"
#include "ui/views/widget/widget.h"
#include "ui/views/window/non_client_view.h"

namespace nw {

namespace {

const int kResizeInsideBoundsSize = 5;
const int kResizeAreaCornerSize = 16;

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

  // Check for possible draggable region in the client area for the frameless
  // window.
  if (window_->draggable_region() &&
      window_->draggable_region()->contains(point.x(), point.y()))
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
      resizable_(true),
      minimum_size_(0, 0),
      maximum_size_(INT_MAX, INT_MAX) {
  window_ = new views::Widget;
  views::Widget::InitParams params(views::Widget::InitParams::TYPE_WINDOW);
  params.delegate = this;
  params.remove_standard_frame = !has_frame();
  params.use_system_default_icon = true;
  window_->Init(params);

  int width, height;
  manifest->GetInteger(switches::kmWidth, &width);
  manifest->GetInteger(switches::kmHeight, &height);

  window_->SetBounds(gfx::Rect(width, height));

  OnViewWasResized();
}

NativeWindowWin::~NativeWindowWin() {
}

void NativeWindowWin::Close() {
  window_->Close();
}

void NativeWindowWin::Move(const gfx::Rect& bounds) {
  window_->SetBounds(bounds);
}

void NativeWindowWin::Focus(bool focus) {
  window_->Activate();
}

void NativeWindowWin::Show() {
  window_->Show();
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
  // TODO(jeremya) we need to call RenderViewHost::ExitFullscreen() if we
  // ever drop the window out of fullscreen in response to something that
  // wasn't the app calling webkitCancelFullScreen().
}

void NativeWindowWin::SetMinimumSize(int width, int height) {
  minimum_size_.set_width(width);
  minimum_size_.set_height(width);
}

void NativeWindowWin::SetMaximumSize(int width, int height) {
  maximum_size_.set_width(width);
  maximum_size_.set_height(width);
}

void NativeWindowWin::SetResizable(bool resizable) {
  resizable_ = resizable;
}

void NativeWindowWin::SetPosition(const std::string& position) {
  if (position == "center") {
    gfx::Rect bounds = window_->GetWindowBoundsInScreen();
    window_->CenterWindow(gfx::Size(bounds.width(), bounds.height()));
  }
}

void NativeWindowWin::FlashFrame(bool flash) {
  window_->FlashFrame(flash);
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
}

void NativeWindowWin::SetToolbarUrlEntry(const std::string& url) {
}
  
void NativeWindowWin::SetToolbarIsLoading(bool loading) {
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
    return NULL;

  NativeWindowFrameView* frame_view = new NativeWindowFrameView(this);
  frame_view->Init(window_);
  return frame_view;
}

bool NativeWindowWin::CanResize() const {
  return resizable_;
}

bool NativeWindowWin::CanMaximize() const {
  return true;
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

gfx::ImageSkia NativeWindowWin::GetWindowAppIcon() {
  // TODO return icon from manifest here.
  return gfx::ImageSkia();
}

gfx::ImageSkia NativeWindowWin::GetWindowIcon() {
  // TODO return icon from manifest here.
  return gfx::ImageSkia();
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
    toolbar_->SetBounds(0, 0, width(), 30);
    web_view_->SetBounds(0, 30, width(), height() - 30);
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
  int radius = 1;
  gfx::Path path;
  if (window_->IsMaximized() || window_->IsFullscreen()) {
    // Don't round the corners when the window is maximized or fullscreen.
    path.addRect(0, 0, width, height);
  } else {
    if (!has_frame()) {
      path.moveTo(0, radius);
      path.lineTo(radius, 0);
      path.lineTo(width - radius, 0);
      path.lineTo(width, radius);
    } else {
      // Don't round the top corners in chrome-style frame mode.
      path.moveTo(0, 0);
      path.lineTo(width, 0);
    }
    path.lineTo(width, height - radius - 1);
    path.lineTo(width - radius - 1, height);
    path.lineTo(radius + 1, height);
    path.lineTo(0, height - radius - 1);
    path.close();
  }
  SetWindowRgn(web_contents()->GetNativeView(), path.CreateNativeRegion(), 1);

  SkRegion* rgn = new SkRegion;
  if (!window_->IsFullscreen()) {
    if (draggable_region())
      rgn->op(*draggable_region(), SkRegion::kUnion_Op);
    if (!window_->IsMaximized()) {
      if (!has_frame())
        rgn->op(0, 0, width, kResizeInsideBoundsSize, SkRegion::kUnion_Op);
      rgn->op(0, 0, kResizeInsideBoundsSize, height, SkRegion::kUnion_Op);
      rgn->op(width - kResizeInsideBoundsSize, 0, width, height,
          SkRegion::kUnion_Op);
      rgn->op(0, height - kResizeInsideBoundsSize, width, height,
          SkRegion::kUnion_Op);
    }
  }
  if (web_contents()->GetRenderViewHost()->GetView())
    web_contents()->GetRenderViewHost()->GetView()->SetClickthroughRegion(rgn);
}

}  // namespace nw
