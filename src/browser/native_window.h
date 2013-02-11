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
#include "base/compiler_specific.h"
#include "ui/gfx/image/image.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/gfx/point.h"
#include "ui/gfx/size.h"

namespace api {
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

class NativeWindow {
 public:
  virtual ~NativeWindow();

  static NativeWindow* Create(content::Shell* shell,
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
  virtual void SetPosition(const std::string& position) = 0;
  virtual void SetPosition(const gfx::Point& position) = 0;
  virtual gfx::Point GetPosition() = 0;
  virtual gfx::Point GetMousePosition() = 0;
  virtual void BeginOffclientMouseMove() = 0;
  virtual void EndOffclientMouseMove() = 0;
  virtual void SetTitle(const std::string& title) = 0;
  virtual void FlashFrame(bool flash) = 0;
  virtual void SetKiosk(bool kiosk) = 0;
  virtual bool IsKiosk() = 0;
  virtual void SetTransparent() = 0;
  virtual bool IsTransparent() = 0;
  virtual void SetMenu(api::Menu* menu) = 0;

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

  content::Shell* shell() const { return shell_; }
  content::WebContents* web_contents() const;
  bool has_frame() const { return has_frame_; }
  const gfx::Image& app_icon() const { return app_icon_; }
  void CapturePage(const std::string& image_format);

 protected:
  explicit NativeWindow(content::Shell* shell,
                        base::DictionaryValue* manifest);

  // Weak reference to parent.
  content::Shell* shell_;

  bool has_frame_;

  // Icon showed in the task bar.
  gfx::Image app_icon_;

  scoped_refptr<CapturePageHelper> capture_page_helper_;

 private:
  void LoadAppIconFromPackage(base::DictionaryValue* manifest);

  DISALLOW_COPY_AND_ASSIGN(NativeWindow);
};

}  // namespace nw

#endif  // CONTENT_NW_SRC_BROWSER_NATIVE_WINDOW_H_
