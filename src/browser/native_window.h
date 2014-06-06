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

#ifndef CONTENT_NW_SRC_BROWSER_NATIVE_WINDOW_H_
#define CONTENT_NW_SRC_BROWSER_NATIVE_WINDOW_H_

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/compiler_specific.h"
#include "content/nw/src/nw_shell.h"
#include "ui/gfx/image/image.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/gfx/point.h"
#include "ui/gfx/size.h"

#if defined(OS_WIN)
#include "components/web_modal/web_contents_modal_dialog_host.h"
#endif

namespace nwapi {
class Menu;
}

namespace base {
class DictionaryValue;
}

namespace content {
struct NativeWebKeyboardEvent;
class Shell;
class WebContents;
}

namespace extensions {
struct DraggableRegion;
}

namespace gfx {
class Rect;
}

namespace nw {

class CapturePageHelper;

#if defined(OS_WIN)
class NativeWindow : public web_modal::WebContentsModalDialogHost {
#else
class NativeWindow {
#endif
 public:
  virtual ~NativeWindow();

  static NativeWindow* Create(const base::WeakPtr<content::Shell>& shell,
                              base::DictionaryValue* manifest);

  void InitFromManifest(base::DictionaryValue* manifest);

  virtual void Close() = 0;
  virtual void Move(const gfx::Rect& pos) = 0;
  virtual void Focus(bool focus) = 0;
  virtual void Show() = 0;
  virtual void Hide() = 0;
  virtual void Maximize() = 0;
  virtual void Unmaximize() = 0;
  virtual void Minimize() = 0;
  virtual void Restore() = 0;
  virtual void SetFullscreen(bool fullscreen) = 0;
  virtual bool IsFullscreen() = 0;
  virtual void SetSize(const gfx::Size& size) = 0;
  virtual gfx::Size GetSize() = 0;
  virtual void SetMinimumSize(int width, int height) = 0;
  virtual void SetMaximumSize(int width, int height) = 0;
  virtual void SetResizable(bool resizable) = 0;
  virtual void SetAlwaysOnTop(bool top) = 0;
  virtual void SetShowInTaskbar(bool show = true) = 0;
  virtual void SetPosition(const std::string& position) = 0;
  virtual void SetPosition(const gfx::Point& position) = 0;
  virtual gfx::Point GetPosition() = 0;
  virtual void SetTitle(const std::string& title) = 0;
  virtual void FlashFrame(int count) = 0;
  virtual void SetBadgeLabel(const std::string& badge) = 0;
  virtual void SetProgressBar(double progress) = 0;
  virtual void SetKiosk(bool kiosk) = 0;
  virtual bool IsKiosk() = 0;
  virtual void SetMenu(nwapi::Menu* menu) = 0;
  virtual void ClearMenu() {}
  virtual void SetInitialFocus(bool accept_focus) = 0;
  virtual bool InitialFocus() = 0;

  // Toolbar related controls.
  enum TOOLBAR_BUTTON {
    BUTTON_BACK = 0,
    BUTTON_FORWARD,
    BUTTON_REFRESH_OR_STOP,
    BUTTON_DEVTOOLS,
    BUTTON_REFRESH_DEV
  };
  virtual void AddToolbar() = 0;
  virtual void SetToolbarButtonEnabled(TOOLBAR_BUTTON button,
                                       bool enabled) = 0;
  virtual void SetToolbarUrlEntry(const std::string& url) = 0;
  virtual void SetToolbarIsLoading(bool loading) = 0;

  virtual void UpdateDraggableRegions(
      const std::vector<extensions::DraggableRegion>& regions) = 0;
  virtual void HandleKeyboardEvent(
      const content::NativeWebKeyboardEvent& event) = 0;

#if defined(OS_WIN)
  virtual gfx::NativeView GetHostView() const OVERRIDE = 0;
  virtual gfx::Point GetDialogPosition(const gfx::Size& size) OVERRIDE = 0;
  virtual void AddObserver(web_modal::ModalDialogHostObserver* observer) OVERRIDE = 0;
  virtual void RemoveObserver(web_modal::ModalDialogHostObserver* observer) OVERRIDE = 0;
  virtual gfx::Size GetMaximumDialogSize() OVERRIDE = 0;
#endif
  content::Shell* shell() const { return shell_.get(); }
  content::WebContents* web_contents() const;
  bool has_frame() const { return has_frame_; }
  const gfx::Image& app_icon() const { return app_icon_; }
  void CapturePage(const std::string& image_format);

 protected:
  void OnNativeWindowDestory() {
    if (shell_)
      delete shell_.get();
  }
  explicit NativeWindow(const base::WeakPtr<content::Shell>& shell,
                        base::DictionaryValue* manifest);

  // Weak reference to parent.
  base::WeakPtr<content::Shell> shell_;

  bool has_frame_;

  // Icon showed in the task bar.
  gfx::Image app_icon_;

  scoped_refptr<CapturePageHelper> capture_page_helper_;
  friend class content::Shell;

 private:
  void LoadAppIconFromPackage(base::DictionaryValue* manifest);

  DISALLOW_COPY_AND_ASSIGN(NativeWindow);
};

}  // namespace nw

#endif  // CONTENT_NW_SRC_BROWSER_NATIVE_WINDOW_H_
