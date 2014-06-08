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

#include <gdk/gdk.h>

#include "base/values.h"
#include "chrome/browser/ui/gtk/gtk_window_util.h"
#include "extensions/common/draggable_region.h"
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/nw_shell.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_view.h"
#include "content/public/common/renderer_preferences.h"
#include "ui/base/x/x11_util.h"
#include "ui/gfx/gtk_util.h"
#include "ui/gfx/rect.h"
#include "ui/gfx/skia_utils_gtk.h"

namespace nw {

namespace {
  
// Dividing GTK's cursor blink cycle time (in milliseconds) by this value yields
// an appropriate value for content::RendererPreferences::caret_blink_interval.
// This matches the logic in the WebKit GTK port.
const double kGtkCursorBlinkCycleFactor = 2000.0;

}  // namespace 
  
NativeWindowGtk::NativeWindowGtk(const base::WeakPtr<content::Shell>& shell,
                                 base::DictionaryValue* manifest)
    : NativeWindow(shell, manifest),
      state_(GDK_WINDOW_STATE_WITHDRAWN),
      content_thinks_its_fullscreen_(false),
      frame_cursor_(NULL),
      resizable_(true),
      last_x_(-1), last_y_(-1),
      last_width_(-1), last_height_(-1)
{
  window_ = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));

  gtk_accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group(window_,gtk_accel_group);

  vbox_ = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox_);
  gtk_container_add(GTK_CONTAINER(window_), vbox_);

  // Set window icon.
  gfx::Image icon = app_icon();
  if (!icon.IsEmpty())
    gtk_window_set_icon(window_, icon.ToGdkPixbuf());

  // Always create toolbar since we need to create a url entry.
  CreateToolbar();

  // Check it in consturctor since it's impossible to add a toolbar afterward.
  bool toolbar = true;
  manifest->GetBoolean(switches::kmToolbar, &toolbar);
  if (toolbar)
    gtk_box_pack_start(GTK_BOX(vbox_), toolbar_, FALSE, FALSE, 0);

  gfx::NativeView native_view =
      web_contents()->GetView()->GetNativeView();
  gtk_widget_show(native_view);
  gtk_container_add(GTK_CONTAINER(vbox_), native_view);

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

  last_width_  = width;
  last_height_ = height;

  // Hide titlebar when {frame: false} specified.
  if (!has_frame_)
    gtk_window_set_decorated(window_, false);

  bool initial_focus = true;
  manifest->GetBoolean(switches::kmInitialFocus, &initial_focus);
  SetInitialFocus(initial_focus);

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
  g_signal_connect(window_, "configure-event",
                   G_CALLBACK(OnWindowConfigureEventThunk), this);
  if (!has_frame_) {
    g_signal_connect(window_, "button-press-event",
                     G_CALLBACK(OnButtonPressThunk), this);

    g_signal_connect(window_, "motion-notify-event",
                     G_CALLBACK(OnMouseMoveEventThunk), this);
  }

  SetWebKitColorStyle();
  gtk_widget_realize(GTK_WIDGET(window_));
}

NativeWindowGtk::~NativeWindowGtk() {
}

void NativeWindowGtk::Close() {
  if (shell_ && !shell_->ShouldCloseWindow())
    return;

  gtk_widget_destroy(GTK_WIDGET(window_));
}

void NativeWindowGtk::Move(const gfx::Rect& bounds) {
  gint x = static_cast<gint>(bounds.x());
  gint y = static_cast<gint>(bounds.y());
  gint width = static_cast<gint>(bounds.width());
  gint height = static_cast<gint>(bounds.height());

  gtk_window_move(window_, x, y);
  gtk_window_util::SetWindowSize(window_, gfx::Size(width, height));
}

void NativeWindowGtk::Focus(bool focus) {
  if (focus)
    gtk_window_present(window_);
  else
    gdk_window_lower(gtk_widget_get_window(GTK_WIDGET(window_)));
}

void NativeWindowGtk::Show() {
  gtk_widget_show(GTK_WIDGET(window_));
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

bool NativeWindowGtk::IsFullscreen() {
  return content_thinks_its_fullscreen_;
}

bool NativeWindowGtk::IsMaximized() const {
  return (state_ & GDK_WINDOW_STATE_MAXIMIZED);
}

bool NativeWindowGtk::IsMinimized() const {
  return (state_ & GDK_WINDOW_STATE_ICONIFIED);
}

void NativeWindowGtk::SetSize(const gfx::Size& size) {
  gtk_window_util::SetWindowSize(window_, size);
}

gfx::Size NativeWindowGtk::GetSize() {
  GdkWindow* gdk_window = gtk_widget_get_window(GTK_WIDGET(window_));

  GdkRectangle frame_extents;
  gdk_window_get_frame_extents(gdk_window, &frame_extents);

  return gfx::Size(frame_extents.width, frame_extents.height); 
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
  resizable_ = resizable;
  // Should request widget size after setting unresizable, otherwise the 
  // window will shrink to a very small size.
  if (resizable == false) {
    gint width, height;
    gtk_window_get_size(window_, &width, &height);
    gtk_widget_set_size_request(GTK_WIDGET(window_), width, height);
  }

  gtk_window_set_resizable(window_, resizable);
}

void NativeWindowGtk::SetAlwaysOnTop(bool top) {
  gtk_window_set_keep_above(window_, top ? TRUE : FALSE);
}

void NativeWindowGtk::SetShowInTaskbar(bool show) {
  gtk_window_set_skip_taskbar_hint(window_, show ? FALSE : TRUE);
}

void NativeWindowGtk::SetPosition(const std::string& position) {
  if (position == "center")
    gtk_window_set_position(window_, GTK_WIN_POS_CENTER);
  else if (position == "mouse")
    gtk_window_set_position(window_, GTK_WIN_POS_MOUSE);
}

void NativeWindowGtk::SetPosition(const gfx::Point& position) {
  gtk_window_move(window_, position.x(), position.y());
}

gfx::Point NativeWindowGtk::GetPosition() {
  GdkWindow* gdk_window = gtk_widget_get_window(GTK_WIDGET(window_));

  GdkRectangle frame_extents;
  gdk_window_get_frame_extents(gdk_window, &frame_extents);

  return gfx::Point(frame_extents.x, frame_extents.y); 
}

void NativeWindowGtk::SetTitle(const std::string& title) {
  gtk_window_set_title(GTK_WINDOW(window_), title.c_str());
}

void NativeWindowGtk::FlashFrame(bool flash) {
  gtk_window_set_urgency_hint(window_, flash);
}

void NativeWindowGtk::SetBadgeLabel(const std::string& badge) {
  // TODO
}

void NativeWindowGtk::SetKiosk(bool kiosk) {
  SetFullscreen(kiosk);
}

bool NativeWindowGtk::IsKiosk() {
  return IsFullscreen();
}

void NativeWindowGtk::SetMenu(nwapi::Menu* menu) {
  menu->UpdateKeys(gtk_accel_group);
  gtk_box_pack_start(GTK_BOX(vbox_), menu->menu_, FALSE, FALSE, 0);
  gtk_box_reorder_child(GTK_BOX(vbox_), menu->menu_, 0);
}

void NativeWindowGtk::SetInitialFocus(bool initial_focus) {
  gtk_window_set_focus_on_map(GTK_WINDOW(window_), initial_focus);
}

bool NativeWindowGtk::InitialFocus() {
  return gtk_window_get_focus_on_map(GTK_WINDOW(window_));
}

void NativeWindowGtk::SetAsDesktop() {
  gtk_window_set_type_hint(window_, GDK_WINDOW_TYPE_HINT_DESKTOP);
  GdkScreen* screen = gtk_window_get_screen(window_);
  gtk_window_set_default_size(window_,
                              gdk_screen_get_width(screen),
                              gdk_screen_get_height(screen));
}

void NativeWindowGtk::AddToolbar() {
  DCHECK(toolbar_);
  gtk_widget_show_all(toolbar_);
}

void NativeWindowGtk::SetToolbarButtonEnabled(TOOLBAR_BUTTON button,
                                              bool enabled) {
  if (!toolbar_)
    return;

  GtkToolItem* target = NULL;
  switch (button) {
    case BUTTON_BACK:
      target = back_button_;
      break;
    case BUTTON_FORWARD:
      target = forward_button_;
      break;
    case BUTTON_REFRESH_OR_STOP:
      target = refresh_stop_button_;
      break;
    case BUTTON_DEVTOOLS:
      target = devtools_button_;
      break;
    case BUTTON_REFRESH_DEV:
      target = dev_reload_button_;
      break;
  }

  DCHECK(target);
  gtk_widget_set_sensitive(GTK_WIDGET(target), enabled);
}

void NativeWindowGtk::SetToolbarUrlEntry(const std::string& url) {
  if (!toolbar_)
    return;

  gtk_entry_set_text(GTK_ENTRY(url_entry_), url.c_str());
}
  
void NativeWindowGtk::SetToolbarIsLoading(bool loading) {
  if (!toolbar_)
    return;

  if (loading)
    gtk_tool_button_set_stock_id(GTK_TOOL_BUTTON(refresh_stop_button_),
                                 GTK_STOCK_STOP);
  else
    gtk_tool_button_set_stock_id(GTK_TOOL_BUTTON(refresh_stop_button_),
                                 GTK_STOCK_REFRESH);
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

  GtkStyle* entry_style = gtk_rc_get_style(url_entry_);
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
  toolbar_ = gtk_toolbar_new();
  gtk_toolbar_set_style(GTK_TOOLBAR(toolbar_), GTK_TOOLBAR_ICONS);

  back_button_ = gtk_tool_button_new_from_stock(GTK_STOCK_GO_BACK);
  g_signal_connect(back_button_, "clicked",
                   G_CALLBACK(&OnBackButtonClickedThunk), this);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar_), back_button_, -1);

  forward_button_ = gtk_tool_button_new_from_stock(GTK_STOCK_GO_FORWARD);
  g_signal_connect(forward_button_, "clicked",
                   G_CALLBACK(&OnForwardButtonClickedThunk), this);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar_), forward_button_, -1);

  refresh_stop_button_ = gtk_tool_button_new_from_stock(GTK_STOCK_STOP);
  g_signal_connect(refresh_stop_button_, "clicked",
                   G_CALLBACK(&OnRefreshStopButtonClickedThunk), this);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar_), refresh_stop_button_, -1);

  url_entry_ = gtk_entry_new();
  g_signal_connect(url_entry_, "activate",
                   G_CALLBACK(&OnURLEntryActivateThunk), this);

  GtkToolItem* tool_item = gtk_tool_item_new();
  gtk_container_add(GTK_CONTAINER(tool_item), url_entry_);
  gtk_tool_item_set_expand(tool_item, TRUE);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar_), tool_item, -1);

  devtools_button_ = gtk_tool_button_new_from_stock(GTK_STOCK_EXECUTE);
  g_signal_connect(devtools_button_, "clicked",
                   G_CALLBACK(&OnDevtoolsButtonClickedThunk), this);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar_), devtools_button_, -1);

  dev_reload_button_ = gtk_tool_button_new_from_stock(GTK_STOCK_REFRESH);
  g_signal_connect(dev_reload_button_, "clicked",
                   G_CALLBACK(&OnDevReloadButtonClickedThunk), this);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar_), dev_reload_button_, -1);
}

gfx::Rect NativeWindowGtk::GetBounds() {
  gint width, height;
  gtk_window_get_size(window_, &width, &height);
  gint x, y;
  gtk_window_get_position(window_, &x, &y);

  return gfx::Rect(x, y, width, height);
}

void NativeWindowGtk::OnBackButtonClicked(GtkWidget* widget) {
  if (shell())
    shell()->GoBackOrForward(-1);
}

void NativeWindowGtk::OnForwardButtonClicked(GtkWidget* widget) {
  if (shell())
    shell()->GoBackOrForward(1);
}

void NativeWindowGtk::OnRefreshStopButtonClicked(GtkWidget* widget) {
  if (shell())
    shell()->ReloadOrStop();
}

void NativeWindowGtk::OnDevReloadButtonClicked(GtkWidget* widget) {
  if (shell())
    shell()->Reload(content::Shell::RELOAD_DEV);
}

void NativeWindowGtk::OnURLEntryActivate(GtkWidget* entry) {
  const gchar* str = gtk_entry_get_text(GTK_ENTRY(entry));
  GURL url(str);
  if (!url.has_scheme())
    url = GURL(std::string("http://") + std::string(str));
  if (shell())
    shell()->LoadURL(GURL(url));
}

void NativeWindowGtk::OnDevtoolsButtonClicked(GtkWidget* entry) {
  if (shell())
    shell()->ShowDevTools();
}

// Callback for when the main window is destroyed.
gboolean NativeWindowGtk::OnWindowDestroyed(GtkWidget* window) {
  OnNativeWindowDestory();
  return FALSE;
}

// Window is focused.
gboolean NativeWindowGtk::OnFocusIn(GtkWidget* window, GdkEventFocus*) {
  gtk_widget_grab_focus(web_contents()->GetView()->GetContentNativeView());

  if (shell())
    shell()->SendEvent("focus");
  return FALSE;
}

// Window lost focus.
gboolean NativeWindowGtk::OnFocusOut(GtkWidget* window, GdkEventFocus*) {
  if (shell())
    shell()->SendEvent("blur");
  return FALSE;
}

// Window state has changed.
gboolean NativeWindowGtk::OnWindowState(GtkWidget* window,
                                        GdkEventWindowState* event) {
  state_ = event->new_window_state;

  switch (event->changed_mask) {
    case GDK_WINDOW_STATE_ICONIFIED:
      if (shell()) {
        if (event->new_window_state & GDK_WINDOW_STATE_ICONIFIED)
          shell()->SendEvent("minimize");
        else
          shell()->SendEvent("restore");
      }
      break;
    case GDK_WINDOW_STATE_MAXIMIZED:
      if (shell()) {
        if (event->new_window_state & GDK_WINDOW_STATE_MAXIMIZED)
          shell()->SendEvent("maximize");
        else
          shell()->SendEvent("unmaximize");
      }
      break;
    case GDK_WINDOW_STATE_FULLSCREEN:
      if (shell()) {
        if (event->new_window_state & GDK_WINDOW_STATE_FULLSCREEN)
          shell()->SendEvent("enter-fullscreen");
        else
          shell()->SendEvent("leave-fullscreen");
      }

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
  if (shell() && !shell()->ShouldCloseWindow())
    return TRUE;

  return FALSE;
}

gboolean NativeWindowGtk::OnWindowConfigureEvent(GtkWidget* window,
                                             GdkEvent* event)
{
  int x, y;
  int w, h;

  x = event->configure.x;
  y = event->configure.y;
  if (x != last_x_ || y != last_y_) {
    last_x_ = x;
    last_y_ = y;
    base::ListValue args;
    args.AppendInteger(x);
    args.AppendInteger(y);
    if (shell())
      shell()->SendEvent("move", args);
  }

  w = event->configure.width;
  h = event->configure.height;
  if (w != last_width_ || h != last_height_) {
    last_width_ = w;
    last_height_ = h;
    base::ListValue args;
    args.AppendInteger(w);
    args.AppendInteger(h);
    if (shell())
      shell()->SendEvent("resize", args);
  }
  return FALSE;
}

bool NativeWindowGtk::GetWindowEdge(int x, int y, GdkWindowEdge* edge) {
  if (has_frame_)
    return false;

  if (IsMaximized() || IsFullscreen())
    return false;

  return gtk_window_util::GetWindowEdge(GetBounds().size(), 0, x, y, edge);
}

gboolean NativeWindowGtk::OnMouseMoveEvent(GtkWidget* widget,
                                           GdkEventMotion* event) {
  if (has_frame_) {
    // Reset the cursor.
    if (frame_cursor_) {
      frame_cursor_ = NULL;
      gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(window_)), NULL);
    }
    return FALSE;
  }

  if (!resizable_)
    return FALSE;

  int win_x, win_y;
  GdkWindow* gdk_window = gtk_widget_get_window(GTK_WIDGET(window_));
  gdk_window_get_origin(gdk_window, &win_x, &win_y);
  gfx::Point point(static_cast<int>(event->x_root - win_x),
                   static_cast<int>(event->y_root - win_y));

  // Update the cursor if we're on the custom frame border.
  GdkWindowEdge edge;
  bool has_hit_edge = GetWindowEdge(point.x(), point.y(), &edge);
  GdkCursorType new_cursor = GDK_LAST_CURSOR;
  if (has_hit_edge)
    new_cursor = gtk_window_util::GdkWindowEdgeToGdkCursorType(edge);

  GdkCursorType last_cursor = GDK_LAST_CURSOR;
  if (frame_cursor_)
    last_cursor = frame_cursor_->type;

  if (last_cursor != new_cursor) {
    frame_cursor_ = has_hit_edge ? gfx::GetCursor(new_cursor) : NULL;
    gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(window_)),
                          frame_cursor_);
  }
  return FALSE;
}

// Capture mouse click on window.
gboolean NativeWindowGtk::OnButtonPress(GtkWidget* widget,
                                        GdkEventButton* event) {
  DCHECK(!has_frame_);
  // Make the button press coordinate relative to the browser window.
  int win_x, win_y;
  GdkWindow* gdk_window = gtk_widget_get_window(GTK_WIDGET(window_));
  gdk_window_get_origin(gdk_window, &win_x, &win_y);

  GdkWindowEdge edge;
  gfx::Point point(static_cast<int>(event->x_root - win_x),
                   static_cast<int>(event->y_root - win_y));
  bool has_hit_edge = resizable_ && GetWindowEdge(point.x(), point.y(), &edge);
  bool has_hit_titlebar =
      !draggable_region_.isEmpty() && draggable_region_.contains(event->x, event->y);

  if (event->button == 1) {
    if (GDK_BUTTON_PRESS == event->type) {
      // Raise the window after a click on either the titlebar or the border to
      // match the behavior of most window managers, unless that behavior has
      // been suppressed.
      if ((has_hit_titlebar || has_hit_edge) && !suppress_window_raise_)
        gdk_window_raise(GTK_WIDGET(widget)->window);

      if (has_hit_edge) {
        gtk_window_begin_resize_drag(window_, edge, event->button,
                                     static_cast<gint>(event->x_root),
                                     static_cast<gint>(event->y_root),
                                     event->time);
        return TRUE;
      } else if (has_hit_titlebar) {
        return gtk_window_util::HandleTitleBarLeftMousePress(
            window_, GetBounds(), event);
      }
    } else if (GDK_2BUTTON_PRESS == event->type) {
      if (has_hit_titlebar && resizable_) {
        // Maximize/restore on double click.
        if (IsMaximized()) {
          gtk_window_unmaximize(window_);
          //gtk_window_util::UnMaximize(GTK_WINDOW(widget),
          //    GetBounds(), restored_bounds_);
        } else {
          gtk_window_maximize(window_);
        }
        return TRUE;
      }
    }
  } else if (event->button == 2) {
    if (has_hit_titlebar || has_hit_edge)
      gdk_window_lower(gdk_window);
    return TRUE;
  }
  return FALSE;
}

}  // namespace nw
