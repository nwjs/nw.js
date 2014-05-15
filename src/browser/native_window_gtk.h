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

#ifndef CONTENT_NW_SRC_BROWSER_NATIVE_WINDOW_GTK_H_
#define CONTENT_NW_SRC_BROWSER_NATIVE_WINDOW_GTK_H_

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "content/nw/src/browser/native_window.h"
#include "third_party/skia/include/core/SkRegion.h"
#include "ui/base/gtk/gtk_signal.h"

namespace nw {

class NativeWindowGtk : public NativeWindow {
 public:
  explicit NativeWindowGtk(const base::WeakPtr<content::Shell>& shell,
                           base::DictionaryValue* manifest);
  virtual ~NativeWindowGtk();

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
  virtual bool IsFullscreen() OVERRIDE;
  virtual void SetSize(const gfx::Size& size) OVERRIDE;
  virtual gfx::Size GetSize() OVERRIDE;
  virtual void SetMinimumSize(int width, int height) OVERRIDE;
  virtual void SetMaximumSize(int width, int height) OVERRIDE;
  virtual void SetResizable(bool resizable) OVERRIDE;
  virtual void SetAlwaysOnTop(bool top) OVERRIDE;
  virtual void SetShowInTaskbar(bool show = true) OVERRIDE;
  virtual void SetPosition(const std::string& position) OVERRIDE;
  virtual void SetPosition(const gfx::Point& position) OVERRIDE;
  virtual gfx::Point GetPosition() OVERRIDE;
  virtual void SetTitle(const std::string& title) OVERRIDE;
  virtual void FlashFrame(bool flash) OVERRIDE;
  virtual void SetBadgeLabel(const std::string& badge) OVERRIDE;
  virtual void SetKiosk(bool kiosk) OVERRIDE;
  virtual bool IsKiosk() OVERRIDE;
  virtual void SetMenu(nwapi::Menu* menu) OVERRIDE;
  virtual void SetInitialFocus(bool initial_focus) OVERRIDE;
  virtual bool InitialFocus() OVERRIDE;
  virtual void SetToolbarButtonEnabled(TOOLBAR_BUTTON button,
                                       bool enabled) OVERRIDE;
  virtual void SetToolbarUrlEntry(const std::string& url) OVERRIDE;
  virtual void SetToolbarIsLoading(bool loading) OVERRIDE;

  GtkWindow* window() const { return window_; }

  bool IsMinimized() const;
  bool IsMaximized() const;
 protected:
  // NativeWindow implementation.
  virtual void AddToolbar() OVERRIDE;
  virtual void UpdateDraggableRegions(
      const std::vector<extensions::DraggableRegion>& regions) OVERRIDE;
  virtual void HandleKeyboardEvent(
      const content::NativeWebKeyboardEvent& event) OVERRIDE;

  void SetAsDesktop();

 private:
  // Set WebKit's style from current theme.
  void SetWebKitColorStyle();

  //Use to bind shortcut
  GtkAccelGroup *gtk_accel_group;

  // Create toolbar.
  void CreateToolbar();

  // Get the position and size of the current window.
  gfx::Rect GetBounds();

  // If the point (|x|, |y|) is within the resize border area of the window,
  // returns true and sets |edge| to the appropriate GdkWindowEdge value.
  // Otherwise, returns false.
  bool GetWindowEdge(int x, int y, GdkWindowEdge* edge);

  CHROMEGTK_CALLBACK_0(NativeWindowGtk, void, OnBackButtonClicked);
  CHROMEGTK_CALLBACK_0(NativeWindowGtk, void, OnForwardButtonClicked);
  CHROMEGTK_CALLBACK_0(NativeWindowGtk, void, OnRefreshStopButtonClicked);
  CHROMEGTK_CALLBACK_0(NativeWindowGtk, void, OnURLEntryActivate);
  CHROMEGTK_CALLBACK_0(NativeWindowGtk, void, OnDevtoolsButtonClicked);
  CHROMEGTK_CALLBACK_0(NativeWindowGtk, void, OnDevReloadButtonClicked);

  CHROMEGTK_CALLBACK_0(NativeWindowGtk, gboolean, OnWindowDestroyed);
  CHROMEGTK_CALLBACK_1(NativeWindowGtk, gboolean, OnFocusIn, GdkEventFocus*);
  CHROMEGTK_CALLBACK_1(NativeWindowGtk, gboolean, OnFocusOut, GdkEventFocus*);
  CHROMEGTK_CALLBACK_1(NativeWindowGtk, gboolean, OnWindowState,
                       GdkEventWindowState*);
  CHROMEGTK_CALLBACK_1(NativeWindowGtk, gboolean, OnWindowDeleteEvent,
                       GdkEvent*);
  CHROMEGTK_CALLBACK_1(NativeWindowGtk, gboolean, OnButtonPress,
                       GdkEventButton*);
  CHROMEGTK_CALLBACK_1(NativeWindowGtk, gboolean, OnMouseMoveEvent,
                       GdkEventMotion*);
  CHROMEGTK_CALLBACK_1(NativeWindowGtk, gboolean, OnWindowConfigureEvent,
                       GdkEvent*);

  GtkWindow* window_;
  GtkWidget* toolbar_;
  GtkWidget* url_entry_;
  GtkWidget* vbox_;
  GtkToolItem* back_button_;
  GtkToolItem* forward_button_;
  GtkToolItem* refresh_stop_button_;
  GtkToolItem* devtools_button_;
  GtkToolItem* dev_reload_button_;
  GdkWindowState state_;

  // True if the RVH is in fullscreen mode. The window may not actually be in
  // fullscreen, however: some WMs don't support fullscreen.
  bool content_thinks_its_fullscreen_;

  // The region is treated as title bar, can be dragged to move
  // and double clicked to maximize.
  SkRegion draggable_region_;

  // If true, don't call gdk_window_raise() when we get a click in the title
  // bar or window border.  This is to work around a compiz bug.
  bool suppress_window_raise_;

  // The current window cursor.  We set it to a resize cursor when over the
  // custom frame border.  We set it to NULL if we want the default cursor.
  GdkCursor* frame_cursor_;

  // True if the window should be resizable by the user.
  bool resizable_;

  int last_x_;
  int last_y_;
  int last_width_;
  int last_height_;

  DISALLOW_COPY_AND_ASSIGN(NativeWindowGtk);
};

}  // namespace nw

#endif  // CONTENT_NW_SRC_BROWSER_NATIVE_WINDOW_GTK_H_
