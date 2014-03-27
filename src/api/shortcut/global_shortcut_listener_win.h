// Copyright (c) 2014 Intel Corp
// Copyright (c) 2014 The Chromium Authors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell co
// pies of the Software, and to permit persons to whom the Software is furnished
// to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IM
// PLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNES
// S FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
// OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WH
// ETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef CONTENT_NW_SRC_API_SHORTCUT_GLOBAL_SHORTCUT_LISTENER_WIN_H_
#define CONTENT_NW_SRC_API_SHORTCUT_GLOBAL_SHORTCUT_LISTENER_WIN_H_

#include <windows.h>

#include "content/nw/src/api/shortcut/global_shortcut_listener.h"
#include "ui/gfx/win/singleton_hwnd.h"

namespace nwapi {

// Windows-specific implementation of the GlobalShortcutListener class that
// listens for global shortcuts. Handles setting up a keyboard hook and
// forwarding its output to the base class for processing.
class GlobalShortcutListenerWin : public GlobalShortcutListener,
                                  public gfx::SingletonHwnd::Observer {
 public:
  GlobalShortcutListenerWin();
  virtual ~GlobalShortcutListenerWin();

 private:
  // The implementation of our Window Proc, called by SingletonHwnd.
  virtual void OnWndProc(HWND hwnd,
                         UINT message,
                         WPARAM wparam,
                         LPARAM lparam) OVERRIDE;

  // GlobalShortcutListener implementation.
  virtual void StartListening() OVERRIDE;
  virtual void StopListening() OVERRIDE;
  virtual bool RegisterAcceleratorImpl(
      const ui::Accelerator& accelerator) OVERRIDE;
  virtual void UnregisterAcceleratorImpl(
      const ui::Accelerator& accelerator) OVERRIDE;

  // Whether this object is listening for global shortcuts.
  bool is_listening_;

  // A map of registered accelerators and their registration ids.
  typedef std::map<ui::Accelerator, int> HotkeyIdMap;
  HotkeyIdMap hotkey_ids_;

  DISALLOW_COPY_AND_ASSIGN(GlobalShortcutListenerWin);
};

}  // namespace nwapi

#endif  // CONTENT_NW_SRC_API_SHORTCUT_GLOBAL_SHORTCUT_LISTENER_WIN_H_
