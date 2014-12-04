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

#include "content/nw/src/browser/native_window_aura.h"

#if defined(OS_WIN)
#include <shobjidl.h>
#include <dwmapi.h>
#endif

#include "base/strings/utf_string_conversions.h"
#include "base/values.h"
#include "base/command_line.h"

#if defined(OS_WIN)
#include "base/win/scoped_comptr.h"
#include "base/win/windows_version.h"
#include "base/win/wrapped_window_proc.h"
#endif

#include "chrome/browser/platform_util.h"
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/browser/browser_view_layout.h"
#include "content/nw/src/browser/menubar_view.h"
#include "content/nw/src/browser/native_window_toolbar_aura.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/nw_shell.h"
#include "content/public/browser/native_web_keyboard_event.h"
#include "content/public/browser/render_view_host.h"
#include "content/browser/renderer_host/render_view_host_impl.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/content_switches.h"
#include "extensions/common/draggable_region.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "ui/base/hit_test.h"
#include "ui/gfx/native_widget_types.h"
#if defined(OS_WIN)
#include "ui/gfx/win/hwnd_util.h"
#include "ui/gfx/icon_util.h"
#include "ui/views/win/hwnd_util.h"
#endif
#include "ui/gfx/path.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/font_list.h"
#include "ui/gfx/platform_font.h"
#include "ui/gfx/image/image_skia_operations.h"
#include "ui/views/background.h"
#include "ui/views/controls/webview/webview.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/views_delegate.h"
#include "ui/views/views_switches.h"
#include "ui/views/widget/widget.h"
#include "ui/views/window/native_frame_view.h"
#include "ui/views/widget/native_widget_private.h"
#include "ui/events/event_handler.h"
#include "ui/wm/core/easy_resize_window_targeter.h"
#include "ui/aura/window.h"

#include "chrome/browser/ui/views/accelerator_table.h"
#include "base/basictypes.h"
#include "chrome/app/chrome_command_ids.h"
#include "ui/base/accelerators/accelerator.h"
#include "ui/events/event_constants.h"
#if defined(USE_ASH)
#include "ash/accelerators/accelerator_table.h"
#endif

namespace content {
  extern bool g_support_transparency;
  extern bool g_force_cpu_draw;
}


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
  while ((parent = (gfx::NativeView)platform_util::GetParent(parent)) != NULL) {
    if (possible_parent == parent)
      return true;
  }

  return false;
}

class NativeWindowClientView : public views::ClientView {
 public:
  NativeWindowClientView(views::Widget* widget,
                         views::View* contents_view,
                         const base::WeakPtr<content::Shell>& shell)
      : views::ClientView(widget, contents_view),
        shell_(shell) {
  }
  virtual ~NativeWindowClientView() {}

  virtual bool CanClose() override {
    if (shell_)
      return shell_->ShouldCloseWindow();
    else
      return true;
  }

 private:
  base::WeakPtr<content::Shell> shell_;
};

class NativeWindowFrameView : public views::NonClientFrameView {
 public:
  static const char kViewClassName[];

  explicit NativeWindowFrameView(NativeWindowAura* window);
  virtual ~NativeWindowFrameView();

  void Init(views::Widget* frame);

  // views::NonClientFrameView implementation.
  virtual gfx::Rect GetBoundsForClientView() const override;
  virtual gfx::Rect GetWindowBoundsForClientBounds(
      const gfx::Rect& client_bounds) const override;
  virtual int NonClientHitTest(const gfx::Point& point) override;
  virtual void GetWindowMask(const gfx::Size& size,
                             gfx::Path* window_mask) override;
  virtual void ResetWindowControls() override {}
  virtual void UpdateWindowIcon() override {}
  virtual void UpdateWindowTitle() override {}
  virtual void SizeConstraintsChanged() override {}

  // views::View implementation.
  virtual gfx::Size GetPreferredSize() const override;
  virtual void Layout() override;
  virtual const char* GetClassName() const override;
  virtual void OnPaint(gfx::Canvas* canvas) override;
  virtual gfx::Size GetMinimumSize() const override;
  virtual gfx::Size GetMaximumSize() const override;

 private:
  NativeWindowAura* window_;
  views::Widget* frame_;

  DISALLOW_COPY_AND_ASSIGN(NativeWindowFrameView);
};

const char NativeWindowFrameView::kViewClassName[] =
    "content/nw/src/browser/NativeWindowFrameView";

NativeWindowFrameView::NativeWindowFrameView(NativeWindowAura* window)
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

gfx::Size NativeWindowFrameView::GetPreferredSize() const {
  gfx::Size pref = frame_->client_view()->GetPreferredSize();
  gfx::Rect bounds(0, 0, pref.width(), pref.height());
  return frame_->non_client_view()->GetWindowBoundsForClientBounds(
      bounds).size();
}

void NativeWindowFrameView::Layout() {
}

void NativeWindowFrameView::OnPaint(gfx::Canvas* canvas) {
}

const char* NativeWindowFrameView::GetClassName() const {
  return kViewClassName;
}

gfx::Size NativeWindowFrameView::GetMinimumSize() const {
  return frame_->client_view()->GetMinimumSize();
}

gfx::Size NativeWindowFrameView::GetMaximumSize() const {
  return frame_->client_view()->GetMaximumSize();
}

}  // namespace

NativeWindowAura* NativeWindowAura::GetBrowserViewForNativeWindow(
    gfx::NativeWindow window) {
  views::Widget* widget = views::Widget::GetWidgetForNativeWindow(window);
  return widget ?
      reinterpret_cast<NativeWindowAura*>(widget->GetNativeWindowProperty(
          "__BROWSER_VIEW__")) : nullptr;
}

NativeWindowAura::NativeWindowAura(const base::WeakPtr<content::Shell>& shell,
                                 base::DictionaryValue* manifest)
    : NativeWindow(shell, manifest),
      toolbar_(NULL),
      web_view_(NULL),
      is_fullscreen_(false),
      is_visible_on_all_workspaces_(false),
      is_minimized_(false),
      is_maximized_(false),
      is_focus_(false),
      is_blur_(false),
      menu_(NULL),
      resizable_(true),
      minimum_size_(0, 0),
      maximum_size_(),
      initial_focus_(true),
      last_width_(-1), last_height_(-1) {
  manifest->GetBoolean("focus", &initial_focus_);
  manifest->GetBoolean("fullscreen", &is_fullscreen_);
  manifest->GetBoolean("resizable", &resizable_);
  manifest->GetBoolean("visible-on-all-workspaces", &is_visible_on_all_workspaces_);

  window_ = new views::Widget;
  views::Widget::InitParams params(views::Widget::InitParams::TYPE_WINDOW);
  params.delegate = this;
  params.remove_standard_frame = !has_frame();
  params.use_system_default_icon = true;
  if (content::g_support_transparency && transparent_)
    params.opacity = views::Widget::InitParams::TRANSLUCENT_WINDOW;
  if (is_fullscreen_)
    params.show_state = ui::SHOW_STATE_FULLSCREEN;
  params.visible_on_all_workspaces = is_visible_on_all_workspaces_;
#if defined(OS_WIN)
  if (has_frame())
    window_->set_frame_type(views::Widget::FRAME_TYPE_FORCE_NATIVE);
#endif
  window_->Init(params);

  // WS_CAPTION is needed or the size will be miscalculated on maximizing
  // see upstream issue #224924
#if defined(OS_WIN)
  HWND hwnd = views::HWNDForWidget(window_->GetTopLevelWidget());
  int current_style = ::GetWindowLong(hwnd, GWL_STYLE);
  ::SetWindowLong(hwnd, GWL_STYLE, current_style | WS_CAPTION);
#endif

  if (!has_frame())
    InstallEasyResizeTargeterOnContainer();

  views::WidgetFocusManager::GetInstance()->AddFocusChangeListener(this);

  int width, height;
  manifest->GetInteger(switches::kmWidth, &width);
  manifest->GetInteger(switches::kmHeight, &height);
  gfx::Rect window_bounds =
    window_->non_client_view()->GetWindowBoundsForClientBounds(
        gfx::Rect(width,height));
  last_width_  = width;
  last_height_ = height;
  window_->AddObserver(this);

  window_->SetSize(window_bounds.size());
  window_->CenterWindow(window_bounds.size());

  window_->UpdateWindowIcon();

  OnViewWasResized();
  window_->SetInitialFocus(ui::SHOW_STATE_NORMAL);

  window_->SetNativeWindowProperty("__BROWSER_VIEW__", this);
}

NativeWindowAura::~NativeWindowAura() {
#if defined(OS_WIN)
  FOR_EACH_OBSERVER(web_modal::ModalDialogHostObserver,
                    observer_list_,
                    OnHostDestroying());
#endif
  views::WidgetFocusManager::GetInstance()->RemoveFocusChangeListener(this);
}

void NativeWindowAura::Close() {
  window_->Close();
}

void NativeWindowAura::Move(const gfx::Rect& bounds) {
  window_->SetBounds(bounds);
}

void NativeWindowAura::Focus(bool focus) {
  window_->Activate();
}

void NativeWindowAura::Show() {
  VLOG(1) << "NativeWindowAura::Show(); initial_focus = " << initial_focus_;
  if (is_maximized_)
    window_->native_widget_private()->ShowWithWindowState(ui::SHOW_STATE_MAXIMIZED);
  else if (!initial_focus_) {
    window_->set_focus_on_creation(false);
    window_->native_widget_private()->ShowWithWindowState(ui::SHOW_STATE_INACTIVE);
  } else if (is_fullscreen_) {
    window_->native_widget_private()->ShowWithWindowState(ui::SHOW_STATE_FULLSCREEN);
  } else
    window_->native_widget_private()->Show();
}

void NativeWindowAura::Hide() {
  window_->Hide();
}

void NativeWindowAura::Maximize() {
  window_->Maximize();
}

void NativeWindowAura::Unmaximize() {
  window_->Restore();
}

void NativeWindowAura::Minimize() {
  window_->Minimize();
}

void NativeWindowAura::Restore() {
  window_->Restore();
}

void NativeWindowAura::SetFullscreen(bool fullscreen) {
  is_fullscreen_ = fullscreen;
  window_->SetFullscreen(fullscreen);
  if (shell()) {
    if (fullscreen)
      shell()->SendEvent("enter-fullscreen");
    else
      shell()->SendEvent("leave-fullscreen");
  }
}

bool NativeWindowAura::IsFullscreen() {
  return is_fullscreen_;
}

void NativeWindowAura::SetTransparent(bool transparent) {
  if (!content::g_support_transparency)
    return;
#if defined(OS_WIN)
  // Check for Windows Vista or higher, transparency isn't supported in 
  // anything lower. 
  if (base::win::GetVersion() < base::win::VERSION_VISTA) {
    NOTREACHED() << "The operating system does not support transparency.";
    transparent_ = false;
    return;
  }

  // Check to see if composition is disabled, if so we have to throw an 
  // error, there's no graceful recovery, yet. TODO: Graceful recovery.
  BOOL enabled = FALSE;
  HRESULT result = ::DwmIsCompositionEnabled(&enabled);
  if (!enabled || !SUCCEEDED(result)) {
    NOTREACHED() << "Windows DWM composition is not enabled, transparency is not supported.";
    transparent_ = false;
    return;
  }

  HWND hWnd = views::HWNDForWidget(window_);

  const int marginVal = transparent ? -1 : 0;
  MARGINS mgMarInset = { marginVal, marginVal, marginVal, marginVal };
  if (DwmExtendFrameIntoClientArea(hWnd, &mgMarInset) != S_OK) {
    NOTREACHED() << "Windows DWM extending to client area failed, transparency is not supported.";
    transparent_ = false;
    return;
  }

  // this is needed, or transparency will fail if it defined on startup
  bool change_window_style = false;
  const LONG lastExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
  const LONG exStyle = WS_EX_COMPOSITED;
  const LONG newExStyle = transparent ? lastExStyle | exStyle : lastExStyle & ~exStyle;
  SetWindowLong(hWnd, GWL_EXSTYLE, newExStyle);
  change_window_style |= lastExStyle != newExStyle;

  if (change_window_style) {
    window_->FrameTypeChanged();
  }

  if (content::g_force_cpu_draw && transparent) {
    // Quick FIX where window content is not updated
    Minimize();
    Restore();
  }

#elif defined(USE_X11) && !defined(OS_CHROMEOS)

  static char cachedRes = -1;
  if ( cachedRes<0 ) {
    const CommandLine& command_line = *CommandLine::ForCurrentProcess();
    cachedRes = !command_line.HasSwitch(switches::kDisableGpu) ||
      !command_line.HasSwitch(views::switches::kEnableTransparentVisuals);
  }

  if (cachedRes && transparent) {
    LOG(INFO) << "if transparency does not work, try with --enable-transparent-visuals --disable-gpu";
  }

 #endif

  if (toolbar_) {
    toolbar_->set_background(transparent ? views::Background::CreateSolidBackground(SK_ColorTRANSPARENT) : 
      views::Background::CreateStandardPanelBackground());
    toolbar_->SchedulePaint();
  }

  content::RenderViewHostImpl* rvh = static_cast<content::RenderViewHostImpl*>(shell_->web_contents()->GetRenderViewHost());
  if (rvh) {
    rvh->SetBackgroundOpaque(!transparent);
  }

  transparent_ = transparent;
}

void NativeWindowAura::SetSize(const gfx::Size& size) {
  window_->SetSize(size);
}

gfx::Size NativeWindowAura::GetSize() {
  return window_->GetWindowBoundsInScreen().size();
}

void NativeWindowAura::SetMinimumSize(int width, int height) {
  minimum_size_.set_width(width);
  minimum_size_.set_height(height);
}

void NativeWindowAura::SetMaximumSize(int width, int height) {
  maximum_size_.set_width(width);
  maximum_size_.set_height(height);
}

void NativeWindowAura::SetResizable(bool resizable) {
  resizable_ = resizable;

#if 0 //FIXME
  // Show/Hide the maximize button.
  DWORD style = ::GetWindowLong(views::HWNDForWidget(window_), GWL_STYLE);
  if (resizable)
    style |= WS_MAXIMIZEBOX;
  else
    style &= ~WS_MAXIMIZEBOX;
  ::SetWindowLong(views::HWNDForWidget(window_), GWL_STYLE, style);
#endif
}

void NativeWindowAura::SetShowInTaskbar(bool show) {
#if defined(OS_WIN)
  if (show == false && base::win::GetVersion() < base::win::VERSION_VISTA) {
    // Change the owner of native window. Only needed on Windows XP.
    ::SetWindowLong(views::HWNDForWidget(window_),
                    GWLP_HWNDPARENT,
                    (LONG)ui::GetHiddenWindow());
  }

  base::win::ScopedComPtr<ITaskbarList> taskbar;
  HRESULT result = taskbar.CreateInstance(CLSID_TaskbarList, NULL,
                                          CLSCTX_INPROC_SERVER);
  if (FAILED(result)) {
    VLOG(1) << "Failed creating a TaskbarList object: " << result;
    return;
  }

  result = taskbar->HrInit();
  if (FAILED(result)) {
    LOG(ERROR) << "Failed initializing an ITaskbarList interface.";
    return;
  }

  if (show)
    result = taskbar->AddTab(views::HWNDForWidget(window_));
  else
    result = taskbar->DeleteTab(views::HWNDForWidget(window_));

  if (FAILED(result)) {
    LOG(ERROR) << "Failed to change the show in taskbar attribute";
    return;
  }
#endif
}

void NativeWindowAura::SetAlwaysOnTop(bool top) {
  window_->StackAtTop();
  // SetAlwaysOnTop should be called after StackAtTop because otherwise
  // the top-most flag will be removed.
  window_->SetAlwaysOnTop(top);
}

void NativeWindowAura::SetVisibleOnAllWorkspaces(bool all_workspaces) {
  is_visible_on_all_workspaces_ = all_workspaces;
  window_->SetVisibleOnAllWorkspaces(all_workspaces);
}

void NativeWindowAura::OnWidgetActivationChanged(views::Widget* widget, bool active) {
  if (active) {
    if (shell())
      shell()->SendEvent("focus");
    is_focus_ = true;
    is_blur_ = false;
  }else{
    if (shell())
      shell()->SendEvent("blur");
    is_focus_ = false;
    is_blur_ = true;
  }
}

void NativeWindowAura::OnWidgetBoundsChanged(views::Widget* widget, const gfx::Rect& new_bounds)  {
  int w = new_bounds.width();
  int h = new_bounds.height();
  if (shell() && (w != last_width_ || h != last_height_)) {
    base::ListValue args;
    args.AppendInteger(w);
    args.AppendInteger(h);
    shell()->SendEvent("resize", args);
    last_width_ = w;
    last_height_ = h;
  }
}

void NativeWindowAura::SetPosition(const std::string& position) {
  if (position == "center") {
    gfx::Rect bounds = window_->GetWindowBoundsInScreen();
    window_->CenterWindow(gfx::Size(bounds.width(), bounds.height()));
  } else if (position == "mouse") {
#if defined(OS_WIN) //FIXME
    gfx::Rect bounds = window_->GetWindowBoundsInScreen();
    POINT pt;
    if (::GetCursorPos(&pt)) {
      int x = pt.x - (bounds.width() >> 1);
      int y = pt.y - (bounds.height() >> 1);
      bounds.set_x(x > 0 ? x : 0);
      bounds.set_y(y > 0 ? y : 0);
      window_->SetBoundsConstrained(bounds);
    }
#endif
  }
}

void NativeWindowAura::SetPosition(const gfx::Point& position) {
  gfx::Rect bounds = window_->GetWindowBoundsInScreen();
  window_->SetBounds(gfx::Rect(position, bounds.size()));
}

gfx::Point NativeWindowAura::GetPosition() {
  return window_->GetWindowBoundsInScreen().origin();
}

void NativeWindowAura::FlashFrame(int count) {
#if defined(OS_WIN) //FIXME
  FLASHWINFO fwi;
  fwi.cbSize = sizeof(fwi);
  fwi.hwnd = views::HWNDForWidget(window_);
  if (count != 0) {
    fwi.dwFlags = FLASHW_ALL;
    fwi.uCount = count < 0 ? 4 : count;
    fwi.dwTimeout = 0;
  }
  else {
    fwi.dwFlags = FLASHW_STOP;
  }
  FlashWindowEx(&fwi);
#endif
}

#if defined(OS_WIN)
HICON createBadgeIcon(const HWND hWnd, const TCHAR *value, const int sizeX, const int sizeY) {
  // canvas for the overlay icon
  gfx::Canvas canvas(gfx::Size(sizeX, sizeY), 1, false);

  // drawing red circle
  SkPaint paint;
  paint.setColor(SK_ColorRED);
  canvas.DrawCircle(gfx::Point(sizeX / 2, sizeY / 2), sizeX / 2, paint);

  // drawing the text
  gfx::PlatformFont *platform_font = gfx::PlatformFont::CreateDefault();
  const int fontSize = sizeY*0.65f;
  gfx::Font font(platform_font->GetFontName(), fontSize);
  platform_font->Release();
  platform_font = NULL;
  const int yMargin = (sizeY - fontSize) / 2;
  canvas.DrawStringRectWithFlags(value, gfx::FontList(font), SK_ColorWHITE, gfx::Rect(sizeX, fontSize + yMargin + 1), gfx::Canvas::TEXT_ALIGN_CENTER);

  // return the canvas as windows native icon handle
  return IconUtil::CreateHICONFromSkBitmap(canvas.ExtractImageRep().sk_bitmap());
}
#endif

void NativeWindowAura::SetBadgeLabel(const std::string& badge) {
#if defined(OS_WIN)
  base::win::ScopedComPtr<ITaskbarList3> taskbar;
  HRESULT result = taskbar.CreateInstance(CLSID_TaskbarList, NULL,
    CLSCTX_INPROC_SERVER);

  if (FAILED(result)) {
    VLOG(1) << "Failed creating a TaskbarList3 object: " << result;
    return;
  }

  result = taskbar->HrInit();
  if (FAILED(result)) {
    LOG(ERROR) << "Failed initializing an ITaskbarList3 interface.";
    return;
  }

  HICON icon = NULL;
  HWND hWnd = views::HWNDForWidget(window_);
  if (badge.size())
    icon = createBadgeIcon(hWnd, base::UTF8ToUTF16(badge).c_str(), 32, 32);

  taskbar->SetOverlayIcon(hWnd, icon, L"Status");
  DestroyIcon(icon);
#endif
}

void NativeWindowAura::SetProgressBar(double progress) {
#if defined(OS_WIN)
  base::win::ScopedComPtr<ITaskbarList3> taskbar;
  HRESULT result = taskbar.CreateInstance(CLSID_TaskbarList, NULL,
                                          CLSCTX_INPROC_SERVER);

  if (FAILED(result)) {
    VLOG(1) << "Failed creating a TaskbarList3 object: " << result;
    return;
  }

  result = taskbar->HrInit();
  if (FAILED(result)) {
    LOG(ERROR) << "Failed initializing an ITaskbarList3 interface.";
    return;
  }

  HWND hWnd = views::HWNDForWidget(window_);

  TBPFLAG tbpFlag = TBPF_NOPROGRESS;

  if (progress > 1) {
    tbpFlag = TBPF_INDETERMINATE;
  }
  else if (progress >= 0) {
    tbpFlag = TBPF_NORMAL;
    taskbar->SetProgressValue(hWnd, progress * 100, 100);
  }

  taskbar->SetProgressState(hWnd, tbpFlag);
#endif
}

void NativeWindowAura::SetKiosk(bool kiosk) {
  SetFullscreen(kiosk);
}

bool NativeWindowAura::IsKiosk() {
  return IsFullscreen();
}

void NativeWindowAura::SetMenu(nwapi::Menu* menu) {
  window_->set_has_menu_bar(true);
  menu_ = menu;
#if defined(OS_LINUX)
  MenuBarView* menubar = new MenuBarView();
  GetBrowserViewLayout()->set_menu_bar(menubar);
  AddChildView(menubar);
  menubar->UpdateMenu(menu->model());
  Layout();
  SchedulePaint();
#endif
  // The menu is lazily built.
#if defined(OS_WIN) //FIXME
  menu->Rebuild();
  menu->SetWindow(this);

  // menu is nwapi::Menu, menu->menu_ is NativeMenuWin,
  ::SetMenu(views::HWNDForWidget(window_), menu->menu_->GetNativeMenu());

#endif
  menu->UpdateKeys( window_->GetFocusManager() );
}

BrowserViewLayout* NativeWindowAura::GetBrowserViewLayout() const {
  return static_cast<BrowserViewLayout*>(GetLayoutManager());
}

void NativeWindowAura::SetTitle(const std::string& title) {
  title_ = title;
  window_->UpdateWindowTitle();
}

void NativeWindowAura::AddToolbar() {
  toolbar_ = new NativeWindowToolbarAura(shell());
  GetBrowserViewLayout()->set_tool_bar(toolbar_);
  AddChildViewAt(toolbar_, 0);
}

void NativeWindowAura::SetToolbarButtonEnabled(TOOLBAR_BUTTON button,
                                              bool enabled) {
  if (toolbar_)
    toolbar_->SetButtonEnabled(button, enabled);
}

void NativeWindowAura::SetToolbarUrlEntry(const std::string& url) {
  if (toolbar_)
    toolbar_->SetUrlEntry(url);
}

void NativeWindowAura::SetToolbarIsLoading(bool loading) {
  if (toolbar_)
    toolbar_->SetIsLoading(loading);
}

views::View* NativeWindowAura::GetContentsView() {
  return this;
}

views::ClientView* NativeWindowAura::CreateClientView(views::Widget* widget) {
  return new NativeWindowClientView(widget, GetContentsView(), shell_);
}

views::NonClientFrameView* NativeWindowAura::CreateNonClientFrameView(
    views::Widget* widget) {
  if (has_frame())
    return new views::NativeFrameView(GetWidget());

  NativeWindowFrameView* frame_view = new NativeWindowFrameView(this);
  frame_view->Init(window_);
  return frame_view;
}

void NativeWindowAura::OnWidgetMove() {
  gfx::Point origin = GetPosition();
  if (shell()) {
    base::ListValue args;
    args.AppendInteger(origin.x());
    args.AppendInteger(origin.y());
    shell()->SendEvent("move", args);
  }
}

bool NativeWindowAura::CanResize() const {
  return resizable_;
}

bool NativeWindowAura::CanMaximize() const {
  return resizable_;
}

views::Widget* NativeWindowAura::GetWidget() {
  return window_;
}

const views::Widget* NativeWindowAura::GetWidget() const {
  return window_;
}

base::string16 NativeWindowAura::GetWindowTitle() const {
  return base::UTF8ToUTF16(title_);
}

void NativeWindowAura::DeleteDelegate() {
  OnNativeWindowDestory();
}

bool NativeWindowAura::ShouldShowWindowTitle() const {
  return has_frame();
}

bool NativeWindowAura::ShouldHandleOnSize() const {
  return true;
}

void NativeWindowAura::OnNativeFocusChange(gfx::NativeView focused_before,
                                          gfx::NativeView focused_now) {
  gfx::NativeView this_window = GetWidget()->GetNativeView();
  if (IsParent(focused_now, this_window) ||
      IsParent(this_window, focused_now))
    return;

  if (focused_now == this_window) {
    if (!is_focus_ && shell())
      shell()->SendEvent("focus");
    is_focus_ = true;
    is_blur_ = false;
  } else if (focused_before == this_window) {
    if (!is_blur_ && shell())
      shell()->SendEvent("blur");
    is_focus_ = false;
    is_blur_ = true;
  }
}

gfx::ImageSkia NativeWindowAura::GetWindowAppIcon() {
  gfx::Image icon = app_icon();
  if (icon.IsEmpty())
    return gfx::ImageSkia();

  gfx::ImageSkia icon2 = *icon.ToImageSkia();
#if defined(OS_WIN)
  int size = ::GetSystemMetrics(SM_CXICON);
  return gfx::ImageSkiaOperations::CreateResizedImage(icon2,
     skia::ImageOperations::RESIZE_BEST,
     gfx::Size(size, size));
#else
  return icon2;
#endif
}

gfx::ImageSkia NativeWindowAura::GetWindowIcon() {
  gfx::Image icon = app_icon();
  if (icon.IsEmpty())
    return gfx::ImageSkia();

  return *icon.ToImageSkia();
}

views::View* NativeWindowAura::GetInitiallyFocusedView() {
  return web_view_;
}

void NativeWindowAura::UpdateDraggableRegions(
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

void NativeWindowAura::HandleKeyboardEvent(
    const content::NativeWebKeyboardEvent& event) {
  unhandled_keyboard_event_handler_.HandleKeyboardEvent(event,
                                                        GetFocusManager());
  // Any unhandled keyboard/character messages should be defproced.
  // This allows stuff like F10, etc to work correctly.

  // DefWindowProc(event.os_event.hwnd, event.os_event.message,
  //               event.os_event.wParam, event.os_event.lParam);
}

#if 0
void NativeWindowAura::Layout() {
  DCHECK(web_view_);
  if (toolbar_) {
    toolbar_->SetBounds(0, 0, width(), 34);
    web_view_->SetBounds(0, 34, width(), height() - 34);
  } else {
    web_view_->SetBounds(0, 0, width(), height());
  }
  OnViewWasResized();
}
#endif

void NativeWindowAura::ViewHierarchyChanged(
    const ViewHierarchyChangedDetails& details) {
  if (details.is_add && details.child == this) {
    BrowserViewLayout* layout = new BrowserViewLayout();
    SetLayoutManager(layout);

    web_view_ = new views::WebView(NULL);
    web_view_->SetWebContents(web_contents());
    layout->set_web_view(web_view_);
    AddChildView(web_view_);
  }
}

gfx::Size NativeWindowAura::GetMinimumSize() const {
  return minimum_size_;
}

gfx::Size NativeWindowAura::GetMaximumSize() const {
  return maximum_size_;
}

void NativeWindowAura::OnFocus() {
  web_view_->RequestFocus();
}

bool NativeWindowAura::InitialFocus() {
  return initial_focus_;
}

bool NativeWindowAura::AcceleratorPressed(const ui::Accelerator& accelerator) {
  return true;
}

bool NativeWindowAura::CanHandleAccelerators() const {
  return true;
}

void NativeWindowAura::SetInitialFocus(bool initial_focus) {
  initial_focus_ = initial_focus;
}

bool NativeWindowAura::ExecuteWindowsCommand(int command_id) {
#if defined(OS_WIN)
  // Windows uses the 4 lower order bits of |command_id| for type-specific
  // information so we must exclude this when comparing.
  static const int sc_mask = 0xFFF0;

  if ((command_id & sc_mask) == SC_MINIMIZE) {
    is_minimized_ = true;
    if (shell())
      shell()->SendEvent("minimize");
  } else if ((command_id & sc_mask) == SC_RESTORE && is_minimized_) {
    is_minimized_ = false;
    if (shell())
      shell()->SendEvent("restore");
  } else if ((command_id & sc_mask) == SC_RESTORE && !is_minimized_) {
    is_maximized_ = false;
    if (shell())
      shell()->SendEvent("unmaximize");
  }
#endif
  return false;
}

void NativeWindowAura::HandleWMStateUpdate() {
  if (window_->IsMaximized()) {
    if (!is_maximized_ && shell())
      shell()->SendEvent("maximize");
    is_maximized_ = true;
  }else if (is_maximized_) {
    if (shell())
      shell()->SendEvent("unmaximize");
    is_maximized_ = false;
  }

  if (window_->IsMinimized()) {
    if (!is_minimized_ && shell())
      shell()->SendEvent("minimize");
    is_minimized_ = true;
  }else if (is_minimized_) {
    if (shell())
      shell()->SendEvent("restore");
    is_minimized_ = false;
  }
}

bool NativeWindowAura::HandleSize(unsigned int param, const gfx::Size& size) {
#if defined(OS_WIN)
  if (param == SIZE_MAXIMIZED) {
    is_maximized_ = true;
    if (shell())
      shell()->SendEvent("maximize");
  }else if (param == SIZE_RESTORED && is_maximized_) {
    is_maximized_ = false;
    if (shell())
      shell()->SendEvent("unmaximize");
  }
#endif
  return false;
}

bool NativeWindowAura::ExecuteAppCommand(int command_id) {
#if defined(OS_WIN)
  if (menu_) {
    menu_->menu_delegate_->ExecuteCommand(command_id, 0);
    menu_->menu_->UpdateStates();
  }
#endif
  return false;
}

void NativeWindowAura::SaveWindowPlacement(const gfx::Rect& bounds,
                                          ui::WindowShowState show_state) {
  // views::WidgetDelegate::SaveWindowPlacement(bounds, show_state);
}

void NativeWindowAura::OnViewWasResized() {
  // Set the window shape of the RWHV.
#if defined(OS_WIN)
  DCHECK(window_);
  DCHECK(web_view_);
  gfx::Size sz = web_view_->size();
  int height = sz.height(), width = sz.width();
  gfx::Path path;
  path.addRect(0, 0, width, height);
  SetWindowRgn((HWND)web_contents()->GetNativeView(),
               (HRGN)path.CreateNativeRegion(),
               1);

  SkRegion* rgn = new SkRegion;
  if (!window_->IsFullscreen()) {
    if (draggable_region())
      rgn->op(*draggable_region(), SkRegion::kUnion_Op);

    if (!has_frame() && CanResize() && !window_->IsMaximized()) {
      rgn->op(0, 0, width, kResizeInsideBoundsSize, SkRegion::kUnion_Op);
      rgn->op(0, 0, kResizeInsideBoundsSize, height, SkRegion::kUnion_Op);
      rgn->op(width - kResizeInsideBoundsSize, 0, width, height,
          SkRegion::kUnion_Op);
      rgn->op(0, height - kResizeInsideBoundsSize, width, height,
          SkRegion::kUnion_Op);
    }
  }
#if 0 //FIXME
  if (web_contents()->GetRenderViewHost()->GetView())
    web_contents()->GetRenderViewHost()->GetView()->SetClickthroughRegion(rgn);
#endif
#endif
}

void NativeWindowAura::InstallEasyResizeTargeterOnContainer()  {
  aura::Window* window = window_->GetNativeWindow();
  gfx::Insets inset(kResizeInsideBoundsSize, kResizeInsideBoundsSize,
                    kResizeInsideBoundsSize, kResizeInsideBoundsSize);
  // Add the EasyResizeWindowTargeter on the window, not its root window. The
  // root window does not have a delegate, which is needed to handle the event
  // in Linux.
  window->SetEventTargeter(scoped_ptr<ui::EventTargeter>(
     new wm::EasyResizeWindowTargeter(window, inset, inset)));
}

bool NativeWindowAura::ShouldDescendIntoChildForEventHandling(
    gfx::NativeView child,
    const gfx::Point& location) {
#if defined(USE_AURA)
  if (child->Contains(web_view_->web_contents()->GetNativeView())) {
    // App window should claim mouse events that fall within the draggable
    // region.
    return !draggable_region_.get() ||
      !draggable_region_->contains(location.x(), location.y());
  }
#endif

  return true;
}

gfx::NativeView NativeWindowAura::GetHostView() const {
  return window_->GetNativeView();
}

gfx::Size NativeWindowAura::GetMaximumDialogSize() {
  return window_->GetWindowBoundsInScreen().size();
}

gfx::Point NativeWindowAura::GetDialogPosition(const gfx::Size& size) {
  gfx::Size app_window_size = window_->GetWindowBoundsInScreen().size();
  return gfx::Point(app_window_size.width() / 2 - size.width() / 2,
                    app_window_size.height() / 2 - size.height() / 2);
}

void NativeWindowAura::AddObserver(web_modal::ModalDialogHostObserver* observer) {
  if (observer && !observer_list_.HasObserver(observer))
    observer_list_.AddObserver(observer);
}

void NativeWindowAura::RemoveObserver(web_modal::ModalDialogHostObserver* observer) {
  observer_list_.RemoveObserver(observer);
}

}  // namespace nw
