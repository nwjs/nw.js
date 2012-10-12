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

#include "base/win/wrapped_window_proc.h"
#include "content/nw/src/shell.h"
#include "ui/base/win/hwnd_util.h"

namespace nw {

namespace {

const wchar_t kWindowClass[] = L"NODE_WEBKIT";

}  // namespace

NativeWindowWin::NativeWindowWin(content::Shell* shell,
                                 const base::DictionaryValue* manifest)
    : NativeWindow(shell, manifest) {
  window_ = CreateWindow(kWindowClass, L"node-webkit",
                         WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                         CW_USEDEFAULT, 0,
                         width, height,
                         NULL, NULL, instance_handle_, NULL);
  ui::SetWindowUserData(window_, this);
  SetParent(web_contents_->GetView()->GetNativeView(), window_);
}

NativeWindowWin::~NativeWindowWin() {
}

void NativeWindowWin::Close() {
  DestroyWindow(window_);
}

void NativeWindowWin::Move(const gfx::Rect& pos) {
  SetWindowPos(window_, NULL, pos.x(), pos.y(), pos.width(), pos.height(),
               SWP_NOACTIVATE | SWP_NOZORDER);
}

void NativeWindowWin::Focus(bool focus) {
  if (focus)
    SetFocus(window_);
}

void NativeWindowWin::Show() {
  ShowWindow(window_, SW_SHOW);
}

void NativeWindowWin::Hide() {
  ShowWindow(window_, SW_HIDE);
}

void NativeWindowWin::Maximize() {
  ShowWindow(window_, SW_MAXIMIZE);
}

void NativeWindowWin::Unmaximize() {
  ShowWindow(window_, SW_RESTORE);
}

void NativeWindowWin::Minimize() {
  ShowWindow(window_, SW_MINIMIZE);
}

void NativeWindowWin::Restore() {
  ShowWindow(window_, SW_RESTORE);
}

void NativeWindowWin::SetFullscreen(bool fullscreen) {
}

void NativeWindowWin::SetMinimumSize(int width, int height) {
  min_width_ = width;
  min_height_ = height;
}

void NativeWindowWin::SetMaximumSize(int width, int height) {
  max_width_ = width;
  max_height_ = height;
}

void NativeWindowWin::SetResizable(bool resizable) {
}

void NativeWindowWin::SetPosition(const std::string& position) {
  RECT rc;
  GetWindowRect(window_, &rc);
  OffsetRect(&rc, -rc.left, -rc.top);
  int width = rc.right;
  int height = rc.bottom;

  int x, y;
  if (position == "center") {
    x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
    y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
  } else if (position == "mouse") {
    POINT point;
    GetCursorPos(&point);
    x = point.x - width / 2;
    y = point.y - height / 2;
  }
  MoveWindow(window_, x, y, width, height, FALSE);
}

void NativeWindowWin::FlashFrame(bool flash) {
}

void NativeWindowWin::SetTitle(const std::string& title) {
  string16 title_utf16 = UTF8ToUTF16(title);
  ::SetWindowText(window_, title_utf16.c_str());
}

void NativeWindowWin::AddToolbar() {
}

void NativeWindowWin::SetToolbarButtonEnabled(TOOLBAR_BUTTON button,
                                                bool enabled) {
}

void NativeWindowWin::SetToolbarUrlEntry(const std::string& url) {
}
  
void NativeWindowWin::SetToolbarIsLoading(bool loading) {
}

void NativeWindowWin::UpdateDraggableRegions(
    const std::vector<extensions::DraggableRegion>& regions) {
  LOG(ERROR) << "UpdateDraggableRegions";
}

void NativeWindowWin::HandleKeyboardEvent(
    const content::NativeWebKeyboardEvent& event) {
  // no-op
}
  
}  // namespace nw
