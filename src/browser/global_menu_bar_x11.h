// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// NW fix: modified from src/chrome/browser/ui/views/frame/global_menu_bar_x11.h

#ifndef CONTENT_NW_SRC_BROWSER_GLOBAL_MENU_BAR_X11_H_
#define CONTENT_NW_SRC_BROWSER_GLOBAL_MENU_BAR_X11_H_

#include <map>
#include <string>

#include "base/compiler_specific.h"
#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "base/scoped_observer.h"
#include "components/prefs/pref_change_registrar.h"
#include "ui/base/glib/glib_signal.h"
#include "ui/views/widget/desktop_aura/desktop_window_tree_host_observer_x11.h"
#include "ui/views/views_export.h"

namespace ui {
class Accelerator;
class MenuModel;
}

namespace views {
class DesktopWindowTreeHostX11;
}

namespace nw {
typedef struct _DbusmenuMenuitem DbusmenuMenuitem;
typedef struct _DbusmenuServer   DbusmenuServer;

// Controls the Mac style menu bar on Unity.
//
// Unity has an Apple-like menu bar at the top of the screen that changes
// depending on the active window. In the GTK port, we had a hidden GtkMenuBar
// object in each GtkWindow which existed only to be scrapped by the
// libdbusmenu-gtk code. Since we don't have GtkWindows anymore, we need to
// interface directly with the lower level libdbusmenu-glib, which we
// opportunistically dlopen() since not everyone is running Ubuntu.
class VIEWS_EXPORT GlobalMenuBarX11 : public views::DesktopWindowTreeHostObserverX11 {
 public:
  GlobalMenuBarX11(views::DesktopWindowTreeHostX11* host, ui::MenuModel* model);
  ~GlobalMenuBarX11() override;

  // Creates the object path for DbusemenuServer which is attached to |xid|.
  static std::string GetPathForWindow(unsigned long xid);
  static bool IsGlobalMenuBarEnabled();

  // Overridden from views::DesktopWindowTreeHostObserverX11:
  void OnWindowMapped(unsigned long xid) override;
  void OnWindowUnmapped(unsigned long xid) override;

 private:

  // Builds a separator.
  DbusmenuMenuitem* BuildSeparator();

  // Creates an individual menu item from a title and command, and subscribes
  // to the activation signal.
  DbusmenuMenuitem* BuildMenuItem(const std::string& label, int tag_id);

  // Creates a DbusmenuServer, and attaches all the menu items.
  void InitServer(unsigned long xid);

  // Creates a whole menu defined with |model|
  DbusmenuMenuitem* BuildStaticMenuFromModel(DbusmenuMenuitem* parent, ui::MenuModel* model);

  // Sets the accelerator for |item|.
  void RegisterAccelerator(DbusmenuMenuitem* item,
                           const ui::Accelerator& accelerator);

  // This will remove all menu items in |menu|.
  void ClearMenuSection(DbusmenuMenuitem* menu);

  CHROMEG_CALLBACK_1(GlobalMenuBarX11, void, OnItemActivated, DbusmenuMenuitem*,
                     unsigned int);
  CHROMEG_CALLBACK_0(GlobalMenuBarX11, void, OnSubMenuShow, DbusmenuMenuitem*);

  views::DesktopWindowTreeHostX11* host_;

  DbusmenuServer* server_;
  DbusmenuMenuitem* root_item_;

  ui::MenuModel* model_;

  // For callbacks may be run after destruction.
  base::WeakPtrFactory<GlobalMenuBarX11> weak_ptr_factory_;

  DISALLOW_COPY_AND_ASSIGN(GlobalMenuBarX11);
};

} // namspace nw
#endif  // CONTENT_NW_SRC_BROWSER_GLOBAL_MENU_BAR_X11_H_
