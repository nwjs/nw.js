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

#include "content/nw/src/browser/native_window_gtk.h"

#include "base/values.h"
#include "chrome/browser/ui/gtk/gtk_window_util.h"
#include "chrome/common/extensions/draggable_region.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/shell.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_view.h"
#include "content/public/common/renderer_preferences.h"
#include "ui/gfx/rect.h"
#include "ui/gfx/skia_utils_gtk.h"

namespace nw {

namespace {
  
// Dividing GTK's cursor blink cycle time (in milliseconds) by this value yields
// an appropriate value for content::RendererPreferences::caret_blink_interval.
// This matches the logic in the WebKit GTK port.
const double kGtkCursorBlinkCycleFactor = 2000.0;

}  // namespace 
  
NativeWindowGtk::NativeWindowGtk(content::Shell* shell,
                                 const base::DictionaryValue* manifest)
    : NativeWindow(shell, manifest),
      content_thinks_its_fullscreen_(false) {
  window_ = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));

  gfx::NativeView native_view =
      web_contents()->GetView()->GetNativeView();
  gtk_container_add(GTK_CONTAINER(window_), native_view);

  int width, height;
  manifest->GetInteger(switches::kmWidth, &width);
  manifest->GetInteger(switches::kmHeight, &height);

  // This is done to avoid a WM "feature" where setting the window size to
  // the monitor size causes the WM to set the EWMH for full screen mode.
  if (!has_frame_ &&
      gtk_window_util::BoundsMatchMonitorSize(
          window_, gfx::Rect(0, 0, width, height))) {
    gtk_window_set_default_size(window_, width, height - 1);
  } else {
    gtk_window_set_default_size(window_, width, height);
  }

  // Hide titlebar when {frame: false} specified.
  if (!has_frame_)
    gtk_window_set_decorated(window_, false);

  // Set to desktop
  bool as_desktop;
  if (manifest->GetBoolean(switches::kmAsDesktop, &as_desktop) && as_desktop)
    SetAsDesktop();

  // In some (older) versions of compiz, raising top-level windows when they
  // are partially off-screen causes them to get snapped back on screen, not
  // always even on the current virtual desktop.  If we are running under
  // compiz, suppress such raises, as they are not necessary in compiz anyway.
  if (ui::GuessWindowManager() == ui::WM_COMPIZ)
    suppress_window_raise_ = true;

  g_signal_connect(window_, "destroy",
                   G_CALLBACK(OnWindowDestroyedThunk), this);
  g_signal_connect(window_, "focus-in-event",
                   G_CALLBACK(OnFocusInThunk), this);
  g_signal_connect(window_, "focus-out-event",
                   G_CALLBACK(OnFocusOutThunk), this);
  g_signal_connect(window_, "window-state-event",
                   G_CALLBACK(OnWindowStateThunk), this);
  g_signal_connect(window_, "delete-event",
                   G_CALLBACK(OnWindowDeleteEventThunk), this);
  if (!has_frame_) {
    g_signal_connect(window_, "button-press-event",
                     G_CALLBACK(OnButtonPressThunk), this);
  }

  CreateToolbar();
  SetWebKitColorStyle();
}

NativeWindowGtk::~NativeWindowGtk() {
}

void NativeWindowGtk::Close() {
  gtk_widget_destroy(GTK_WIDGET(window_));
}

void NativeWindowGtk::Move(const gfx::Rect& bounds) {
  gint x = static_cast<gint>(bounds.x());
  gint y = static_cast<gint>(bounds.y());
  gint width = static_cast<gint>(bounds.width());
  gint height = static_cast<gint>(bounds.height());

  gtk_window_move(window_, x, y);
  gtk_window_resize(window_, width, height);
}

void NativeWindowGtk::Focus(bool focus) {
  if (focus)
    gtk_window_present(window_);
}

void NativeWindowGtk::Show() {
  gtk_widget_show_all(GTK_WIDGET(window_));
}

void NativeWindowGtk::Hide() {
  gtk_widget_hide(GTK_WIDGET(window_));
}

void NativeWindowGtk::Maximize() {
  gtk_window_maximize(window_);
}

void NativeWindowGtk::Unmaximize() {
  gtk_window_unmaximize(window_);
}

void NativeWindowGtk::Minimize() {
  gtk_window_iconify(window_);
}

void NativeWindowGtk::Restore() {
  gtk_window_present(window_);
}

void NativeWindowGtk::SetFullscreen(bool fullscreen) {
  content_thinks_its_fullscreen_ = fullscreen;
  if (fullscreen)
    gtk_window_fullscreen(window_);
  else
    gtk_window_unfullscreen(window_);
}

void NativeWindowGtk::SetMinimumSize(int width, int height) {
  GdkGeometry geometry = { 0 };
  geometry.min_width = width;
  geometry.min_height = height;
  int hints = GDK_HINT_POS | GDK_HINT_MIN_SIZE;
  gtk_window_set_geometry_hints(
      window_, GTK_WIDGET(window_), &geometry, (GdkWindowHints)hints);
}

void NativeWindowGtk::SetMaximumSize(int width, int height) {
  GdkGeometry geometry = { 0 };
  geometry.max_width = width;
  geometry.max_height = height;
  int hints = GDK_HINT_POS | GDK_HINT_MAX_SIZE;
  gtk_window_set_geometry_hints(
      window_, GTK_WIDGET(window_), &geometry, (GdkWindowHints)hints);
}

void NativeWindowGtk::SetResizable(bool resizable) {
  // Should request widget size after setting unresizable, otherwise the 
  // window will shrink to a very small size.
  if (resizable == false) {
    gint width, height;
    gtk_window_get_size(window_, &width, &height);
    gtk_widget_set_size_request(GTK_WIDGET(window_), width, height);
  }

  gtk_window_set_resizable(window_, resizable);
}

void NativeWindowGtk::SetPosition(const std::string& position) {
  if (position == "center")
    gtk_window_set_position(window_, GTK_WIN_POS_CENTER);
  else if (position == "mouse")
    gtk_window_set_position(window_, GTK_WIN_POS_MOUSE);
}

void NativeWindowGtk::SetTitle(const std::string& title) {
  gtk_window_set_title(GTK_WINDOW(window_), title.c_str());
}

void NativeWindowGtk::FlashFrame(bool flash) {
  gtk_window_set_urgency_hint(window_, flash);
}

void NativeWindowGtk::SetAsDesktop() {
  gtk_window_set_type_hint(window_, GDK_WINDOW_TYPE_HINT_DESKTOP);
  GdkScreen* screen = gtk_window_get_screen(window_);
  gtk_window_set_default_size(window_,
                              gdk_screen_get_width(screen),
                              gdk_screen_get_height(screen));
}

void NativeWindowGtk::AddToolbar() {
}

void NativeWindowGtk::SetToolbarButtonEnabled(TOOLBAR_BUTTON button,
                                                bool enabled) {
}

void NativeWindowGtk::SetToolbarUrlEntry(const std::string& url) {
}
  
void NativeWindowGtk::SetToolbarIsLoading(bool loading) {
}

void NativeWindowGtk::AddDebugMenu() {
}

void NativeWindowGtk::UpdateDraggableRegions(
    const std::vector<extensions::DraggableRegion>& regions) {
  // Draggable region is not supported for non-frameless window.
  if (has_frame_)
    return;

  SkRegion draggable_region;

  // By default, the whole window is non-draggable. We need to explicitly
  // include those draggable regions.
  for (std::vector<extensions::DraggableRegion>::const_iterator iter =
           regions.begin();
       iter != regions.end(); ++iter) {
    const extensions::DraggableRegion& region = *iter;
    draggable_region.op(
        region.bounds.x(),
        region.bounds.y(),
        region.bounds.right(),
        region.bounds.bottom(),
        region.draggable ? SkRegion::kUnion_Op : SkRegion::kDifference_Op);
  }

  draggable_region_ = draggable_region;
}

void NativeWindowGtk::HandleKeyboardEvent(
    const content::NativeWebKeyboardEvent& event) {
  // no-op
}

void NativeWindowGtk::SetWebKitColorStyle() {
  // Set WebKit's styles according to current GTK theme.
  content::RendererPreferences* prefs =
      web_contents()->GetMutableRendererPrefs();
  GtkStyle* frame_style = gtk_rc_get_style(GTK_WIDGET(window_));
  prefs->focus_ring_color =
      gfx::GdkColorToSkColor(frame_style->bg[GTK_STATE_SELECTED]);
  prefs->thumb_active_color = SkColorSetRGB(244, 244, 244);
  prefs->thumb_inactive_color = SkColorSetRGB(234, 234, 234);
  prefs->track_color = SkColorSetRGB(211, 211, 211);

  GtkStyle* entry_style = gtk_rc_get_style(entry_);
  prefs->active_selection_bg_color =
      gfx::GdkColorToSkColor(entry_style->base[GTK_STATE_SELECTED]);
  prefs->active_selection_fg_color =
      gfx::GdkColorToSkColor(entry_style->text[GTK_STATE_SELECTED]);
  prefs->inactive_selection_bg_color =
      gfx::GdkColorToSkColor(entry_style->base[GTK_STATE_ACTIVE]);
  prefs->inactive_selection_fg_color =
      gfx::GdkColorToSkColor(entry_style->text[GTK_STATE_ACTIVE]);

  const base::TimeDelta cursor_blink_time = gfx::GetCursorBlinkCycle();
  prefs->caret_blink_interval =
      cursor_blink_time.InMilliseconds() ?
      cursor_blink_time.InMilliseconds() / kGtkCursorBlinkCycleFactor :
      0;
}

void NativeWindowGtk::CreateToolbar() {
  GtkWidget* toolbar = gtk_toolbar_new();
  gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);

  entry_ = gtk_entry_new();

  GtkToolItem* tool_item = gtk_tool_item_new();
  gtk_container_add(GTK_CONTAINER(tool_item), entry_);
  gtk_tool_item_set_expand(tool_item, TRUE);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_item, -1 /* append */);
}

gfx::Rect NativeWindowGtk::GetBounds() {
  gint width, height;
  gtk_window_get_size(window_, &width, &height);
  gint x, y;
  gtk_window_get_position(window_, &x, &y);

  return gfx::Rect(x, y, width, height);
}

// Callback for when the main window is destroyed.
void NativeWindowGtk::OnWindowDestroyed(GtkWidget* window) {
  delete shell();
}

// Window is focused.
gboolean NativeWindowGtk::OnFocusIn(GtkWidget* window, GdkEventFocus*) {
  shell()->SendEvent("focus");
  return FALSE;
}

// Window lost focus.
gboolean NativeWindowGtk::OnFocusOut(GtkWidget* window, GdkEventFocus*) {
  shell()->SendEvent("blur");
  return FALSE;
}

// Window state has changed.
gboolean NativeWindowGtk::OnWindowState(GtkWidget* window,
                                        GdkEventWindowState* event) {
  switch (event->changed_mask) {
    case GDK_WINDOW_STATE_ICONIFIED:
      if (event->new_window_state & GDK_WINDOW_STATE_ICONIFIED)
        shell()->SendEvent("minimize");
      else
        shell()->SendEvent("restore");
      break;
    case GDK_WINDOW_STATE_MAXIMIZED:
      if (event->new_window_state & GDK_WINDOW_STATE_MAXIMIZED)
        shell()->SendEvent("maximize");
      else
        shell()->SendEvent("demaximize");
      break;
    case GDK_WINDOW_STATE_FULLSCREEN:
      if (event->new_window_state & GDK_WINDOW_STATE_FULLSCREEN)
        shell()->SendEvent("enter-fullscreen");
      else
        shell()->SendEvent("leave-fullscreen");

      if (content_thinks_its_fullscreen_ &&
          !(event->new_window_state & GDK_WINDOW_STATE_FULLSCREEN)) {
        content_thinks_its_fullscreen_ = false;
        content::RenderViewHost* rvh = web_contents()->GetRenderViewHost();
        if (rvh)
          rvh->ExitFullscreen();
      }
      break;
    default:
      break;
  }
  return FALSE;
}

// Window will be closed.
gboolean NativeWindowGtk::OnWindowDeleteEvent(GtkWidget* widget,
                                              GdkEvent* event) {
  if (!shell()->ShouldCloseWindow())
    return TRUE;

  return FALSE;
}

}  // namespace nw
