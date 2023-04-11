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

#ifndef CONTENT_NW_SRC_API_TRAY_TRAY_H_
#define CONTENT_NW_SRC_API_TRAY_TRAY_H_ 

#include "base/compiler_specific.h"
#include "content/nw/src/api/base/base.h"

#include <string>

#if defined(OS_MAC)
#if __OBJC__
@class NSStatusItem;
@class MacTrayObserver;
#else
class NSStatusItem;
class MacTrayObserver;
#endif  // __OBJC__
#elif 0
#include <gtk/gtk.h>
#include "chrome/browser/ui/libgtk2ui/gtk2_signal.h"
#elif defined(OS_WIN) || defined(OS_LINUX)
class StatusIcon;
class StatusTray;
#endif  // defined(OS_MAC)

namespace nw {

class Menu;
class TrayObserver;

class Tray : public Base {
 public:
  Tray(int id,
       const base::WeakPtr<ObjectManager>& object_manager,
       const base::Value::Dict& option,
       const std::string& extension_id);
  ~Tray() override;

  void Call(const std::string& method,
            const base::Value::List& arguments,
            content::RenderFrameHost* rvh = nullptr) override;

 private:
  // Platform-independent implementations
  void Create(const base::Value::Dict& option);
  void ShowAfterCreate();
  void Destroy();
  void SetTitle(const std::string& title);
  void SetIcon(const std::string& icon_path);
  void SetTooltip(const std::string& tooltip);
  void SetMenu(Menu* menu);
  void Remove();
  // Alternate icons only work with Macs
  void SetAltIcon(const std::string& alticon_path);
  // Template icons only work with Macs 
  void SetIconsAreTemplates(bool areTemplates);

#if defined(OS_MAC)
  __block NSStatusItem* status_item_;
  MacTrayObserver* status_observer_;
  bool iconsAreTemplates; 
#elif 0
  GtkStatusIcon* status_item_;

  // Reference to the associated menu.
  Menu* menu_;

  // Callback invoked when user left-clicks on the status icon.
  CHROMEGTK_CALLBACK_0(Tray, void, OnClick);
  // Callback invoked when user right-clicks on the status icon.
  CHROMEGTK_CALLBACK_2(Tray, void, OnPopupMenu, guint, guint);
#elif defined(OS_WIN) || defined(OS_LINUX)
  // The global presentation of system tray.
  static StatusTray* status_tray_;

  // The icons on the system tray.
  raw_ptr<StatusIcon> status_icon_;

  // Click observer.
  raw_ptr<TrayObserver> status_observer_;
#endif

};

}  // namespace nw

#endif  // CONTENT_NW_SRC_API_TRAY_TRAY_H_
