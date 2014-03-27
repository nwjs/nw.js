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

#ifndef CONTENT_NW_SRC_API_SHORTCUT_GLOBAL_SHORTCUT_LISTENER_H_
#define CONTENT_NW_SRC_API_SHORTCUT_GLOBAL_SHORTCUT_LISTENER_H_

#include <map>

#include "base/basictypes.h"
#include "ui/events/keycodes/keyboard_codes.h"

namespace ui {
class Accelerator;
}

namespace nwapi {

// Platform-neutral implementation of a class that keeps track of observers and
// monitors keystrokes. It relays messages to the appropriate observer when a
// global shortcut has been struck by the user.
class GlobalShortcutListener {
 public:
  class Observer {
   public:
    // Called when your global shortcut (|accelerator|) is struck.
    virtual void OnKeyPressed(const ui::Accelerator& accelerator) = 0;
  };

  virtual ~GlobalShortcutListener();

  static GlobalShortcutListener* GetInstance();

  // Register an observer for when a certain |accelerator| is struck. Returns
  // true if register successfully, or false if 1) the specificied |accelerator|
  // has been registered by another caller or other native applications, or
  // 2) shortcut handling is suspended.
  //
  // Note that we do not support recognizing that an accelerator has been
  // registered by another application on all platforms. This is a per-platform
  // consideration.
  bool RegisterAccelerator(const ui::Accelerator& accelerator,
                           Observer* observer);

  // Stop listening for the given |accelerator|, does nothing if shortcut
  // handling is suspended.
  void UnregisterAccelerator(const ui::Accelerator& accelerator,
                             Observer* observer);

  // Stop listening for all accelerators of the given |observer|, does nothing
  // if shortcut handling is suspended.
  void UnregisterAccelerators(Observer* observer);

  // Suspend/Resume global shortcut handling. Note that when suspending,
  // RegisterAccelerator/UnregisterAccelerator/UnregisterAccelerators are not
  // allowed to be called until shortcut handling has been resumed.
  void SetShortcutHandlingSuspended(bool suspended);

  // Returns whether shortcut handling is currently suspended.
  bool IsShortcutHandlingSuspended() const;

 protected:
  GlobalShortcutListener();

  // Called by platform specific implementations of this class whenever a key
  // is struck. Only called for keys that have an observer registered.
  void NotifyKeyPressed(const ui::Accelerator& accelerator);

 private:
  // The following methods are implemented by platform-specific implementations
  // of this class.
  //
  // Start/StopListening are called when transitioning between zero and nonzero
  // registered accelerators. StartListening will be called after
  // RegisterAcceleratorImpl and StopListening will be called after
  // UnregisterAcceleratorImpl.
  //
  // For RegisterAcceleratorImpl, implementations return false if registration
  // did not complete successfully.
  virtual void StartListening() = 0;
  virtual void StopListening() = 0;
  virtual bool RegisterAcceleratorImpl(const ui::Accelerator& accelerator) = 0;
  virtual void UnregisterAcceleratorImpl(
      const ui::Accelerator& accelerator) = 0;

  // The map of accelerators that have been successfully registered as global
  // shortcuts and their observer.
  typedef std::map<ui::Accelerator, Observer*> AcceleratorMap;
  AcceleratorMap accelerator_map_;

  // Keeps track of whether shortcut handling is currently suspended.
  bool shortcut_handling_suspended_;

  DISALLOW_COPY_AND_ASSIGN(GlobalShortcutListener);
};

}  // namespace nwapi

#endif  // CONTENT_NW_SRC_API_SHORTCUT_GLOBAL_SHORTCUT_LISTENER_H_
