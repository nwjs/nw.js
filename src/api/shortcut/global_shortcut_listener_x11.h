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

#ifndef CHROME_BROWSER_EXTENSIONS_GLOBAL_SHORTCUT_LISTENER_X11_H_
#define CHROME_BROWSER_EXTENSIONS_GLOBAL_SHORTCUT_LISTENER_X11_H_

#include <X11/Xlib.h>
#include <set>

#include "base/message_loop/message_pump_dispatcher.h"
#include "chrome/browser/extensions/global_shortcut_listener.h"

#if defined(TOOLKIT_GTK)
#include <gtk/gtk.h>
#include "ui/base/gtk/gtk_signal.h"
#endif  // defined(TOOLKIT_GTK)

namespace extensions {

// X11-specific implementation of the GlobalShortcutListener class that
// listens for global shortcuts. Handles basic keyboard intercepting and
// forwards its output to the base class for processing.
class GlobalShortcutListenerX11
    :
#if !defined(TOOLKIT_GTK)
      public base::MessagePumpDispatcher,
#endif
      public GlobalShortcutListener {
 public:
  GlobalShortcutListenerX11();
  virtual ~GlobalShortcutListenerX11();

#if !defined(TOOLKIT_GTK)
  // base::MessagePumpDispatcher implementation.
  virtual uint32_t Dispatch(const base::NativeEvent& event) OVERRIDE;
#endif

 private:
  // GlobalShortcutListener implementation.
  virtual void StartListening() OVERRIDE;
  virtual void StopListening() OVERRIDE;
  virtual bool RegisterAcceleratorImpl(
      const ui::Accelerator& accelerator) OVERRIDE;
  virtual void UnregisterAcceleratorImpl(
      const ui::Accelerator& accelerator) OVERRIDE;

#if defined(TOOLKIT_GTK)
  // Callback for XEvents of the default root window.
  CHROMEG_CALLBACK_1(GlobalShortcutListenerX11, GdkFilterReturn,
                     OnXEvent, GdkXEvent*, GdkEvent*);
#endif

  // Invoked when a global shortcut is pressed.
  void OnXKeyPressEvent(::XEvent* x_event);

  // Whether this object is listening for global shortcuts.
  bool is_listening_;

  // The x11 default display and the native root window.
  ::Display* x_display_;
  ::Window x_root_window_;

  // A set of registered accelerators.
  typedef std::set<ui::Accelerator> RegisteredHotKeys;
  RegisteredHotKeys registered_hot_keys_;

  DISALLOW_COPY_AND_ASSIGN(GlobalShortcutListenerX11);
};

}  // namespace extensions

#endif  // CHROME_BROWSER_EXTENSIONS_GLOBAL_SHORTCUT_LISTENER_X11_H_
