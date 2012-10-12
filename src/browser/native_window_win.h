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

namespace nw {

class NativeWindowWin : public NativeWindow {
 public:
  explicit NativeWindowWin(content::Shell* shell,
                           const base::DictionaryValue* manifest);
  virtual ~NativeWindowWin();

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
  virtual void SetMinimumSize(int width, int height) OVERRIDE;
  virtual void SetMaximumSize(int width, int height) OVERRIDE;
  virtual void SetResizable(bool resizable) OVERRIDE;
  virtual void SetPosition(const std::string& position) OVERRIDE;
  virtual void SetTitle(const std::string& title) OVERRIDE;
  virtual void FlashFrame(bool flash) OVERRIDE;
  virtual void SetToolbarButtonEnabled(TOOLBAR_BUTTON button,
                                       bool enabled) OVERRIDE;
  virtual void SetToolbarUrlEntry(const std::string& url) OVERRIDE;
  virtual void SetToolbarIsLoading(bool loading) OVERRIDE;

 protected:
  // NativeWindow implementation.
  virtual void AddToolbar() OVERRIDE;
  virtual void AddDebugMenu() OVERRIDE;
  virtual void UpdateDraggableRegions(
      const std::vector<extensions::DraggableRegion>& regions) OVERRIDE;
  virtual void HandleKeyboardEvent(
      const content::NativeWebKeyboardEvent& event) OVERRIDE;

 private:

  DISALLOW_COPY_AND_ASSIGN(NativeWindowWin);
};

}  // namespace nw

#endif  // CONTENT_NW_SRC_BROWSER_NATIVE_WINDOW_WIN_H_
