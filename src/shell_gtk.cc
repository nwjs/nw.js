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

#include "shell.h"

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "base/command_line.h"
#include "base/logging.h"
#include "base/string_piece.h"
#include "base/utf_string_conversions.h"
#include "base/values.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/native_web_keyboard_event.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_view.h"
#include "content/public/common/renderer_preferences.h"
#include "shell_browser_context.h"
#include "shell_content_browser_client.h"
#include "shell_switches.h"
#include "third_party/skia/include/core/SkColor.h"

namespace content {

namespace {

// Callback for Debug > Show web inspector... menu item.
gboolean ShowWebInspectorActivated(GtkWidget* widget, Shell* shell) {
  shell->ShowDevTools();
  return FALSE;  // Don't stop this message.
}

GtkWidget* AddMenuEntry(GtkWidget* menu_widget, const char* text,
                        GCallback callback, Shell* shell) {
  GtkWidget* entry = gtk_menu_item_new_with_mnemonic(text);
  g_signal_connect(entry, "activate", callback, shell);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_widget), entry);
  return entry;
}

GtkWidget* CreateMenu(GtkWidget* menu_bar, const char* text) {
  GtkWidget* menu_widget = gtk_menu_new();
  GtkWidget* menu_header = gtk_menu_item_new_with_label(text);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_header), menu_widget);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_header);
  return menu_widget;
}

GtkWidget* CreateMenuBar(Shell* shell) {
  GtkWidget* menu_bar = gtk_menu_bar_new();
  GtkWidget* debug_menu = CreateMenu(menu_bar, "Debug");
  AddMenuEntry(debug_menu, "_Show Developer Tools...",
               G_CALLBACK(ShowWebInspectorActivated), shell);
  return menu_bar;
}

}  // namespace

void Shell::PlatformInitialize() {
}

void Shell::PlatformCleanUp() {
  // Nothing to clean up; GTK will clean up the widgets shortly after.
}

void Shell::PlatformEnableUIControl(UIControl control, bool is_enabled) {
  GtkToolItem* item = NULL;
  switch (control) {
    case BACK_BUTTON:
      item = back_button_;
      break;
    case FORWARD_BUTTON:
      item = forward_button_;
      break;
    case STOP_BUTTON:
      item = stop_button_;
      break;
    default:
      NOTREACHED() << "Unknown UI control";
      return;
  }
  gtk_widget_set_sensitive(GTK_WIDGET(item), is_enabled);
}

void Shell::PlatformSetAddressBarURL(const GURL& url) {
  gtk_entry_set_text(GTK_ENTRY(url_edit_view_), url.spec().c_str());
}

void Shell::PlatformSetIsLoading(bool loading) {
  if (loading)
    gtk_spinner_start(GTK_SPINNER(spinner_));
  else
    gtk_spinner_stop(GTK_SPINNER(spinner_));
}


void Shell::PlatformCreateWindow(int width, int height) {
  window_ = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
  gtk_window_set_default_size(window_, width, height);
  g_signal_connect(G_OBJECT(window_), "destroy",
                   G_CALLBACK(OnWindowDestroyedThunk), this);

  std::string title = "node-webkit";
  
  bool as_desktop = false;
  window_manifest_->GetBoolean(switches::kmAsDesktop, &as_desktop);
  
  if (window_manifest_ && !as_desktop) {
    // window.x and window.y
    int x, y;
    if (window_manifest_->GetInteger(switches::kmX, &x) &&
        window_manifest_->GetInteger(switches::kmY, &y)) {
      gtk_window_move(window_, x, y);
    } else {
      // window.postion
      std::string desription;
      if (window_manifest_->GetString(switches::kmPosition, &desription)) {
        if (desription == "center")
          gtk_window_set_position(window_, GTK_WIN_POS_CENTER);
        else if (desription == "mouse")
          gtk_window_set_position(window_, GTK_WIN_POS_MOUSE);
      }
    }

    GdkGeometry geometry = { 0 };
    int hints = GDK_HINT_POS;
    if (min_width_ > 0) {
      hints |= GDK_HINT_MIN_SIZE;
      geometry.min_width = min_width_;
    }
    if (min_height_ > 0) {
      hints |= GDK_HINT_MIN_SIZE;
      geometry.min_height = min_height_;
    }
    if (max_width_ > 0) {
      hints |= GDK_HINT_MAX_SIZE;
      geometry.max_width = max_width_;
    } 
    if (max_height_ > 0) {
      hints |= GDK_HINT_MAX_SIZE;
      geometry.max_height = max_height_;
    }
    if (hints != GDK_HINT_POS) {
      gtk_window_set_geometry_hints(
          window_, GTK_WIDGET(window_), &geometry, (GdkWindowHints)hints);
    }

    // window.title
    window_manifest_->GetString(switches::kmTitle, &title);
  }
  
  if (as_desktop)
  {
      gtk_window_set_type_hint (window_, GDK_WINDOW_TYPE_HINT_DESKTOP);  
		GdkScreen* screen = gtk_window_get_screen(window_);
		gtk_window_set_default_size(window_, gdk_screen_get_width(screen), gdk_screen_get_height(screen));
  }

  gtk_window_set_title(window_, title.c_str());

  vbox_ = gtk_vbox_new(FALSE, 0);

  // Create the menu bar.
  GtkWidget* menu_bar = CreateMenuBar(this);
  if (is_show_devtools_) {
    gtk_box_pack_start(GTK_BOX(vbox_), menu_bar, FALSE, FALSE, 0);
  }

  // Create the object that mediates accelerators.
  GtkAccelGroup* accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window_), accel_group);

  // Set global window handling accelerators:
  gtk_accel_group_connect(
      accel_group, GDK_w, GDK_CONTROL_MASK,
      GTK_ACCEL_VISIBLE,
      g_cclosure_new(G_CALLBACK(OnCloseWindowKeyPressedThunk),
                     this, NULL));

  gtk_accel_group_connect(
      accel_group, GDK_n, GDK_CONTROL_MASK,
      GTK_ACCEL_VISIBLE,
      g_cclosure_new(G_CALLBACK(OnNewWindowKeyPressedThunk),
                    this, NULL));

  GtkWidget* toolbar = gtk_toolbar_new();
  // Turn off the labels on the toolbar buttons.
  gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);

  back_button_ = gtk_tool_button_new_from_stock(GTK_STOCK_GO_BACK);
  g_signal_connect(back_button_, "clicked",
                   G_CALLBACK(&OnBackButtonClickedThunk), this);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), back_button_, -1 /* append */);
  gtk_widget_add_accelerator(GTK_WIDGET(back_button_), "clicked", accel_group,
                             GDK_Left, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

  forward_button_ = gtk_tool_button_new_from_stock(GTK_STOCK_GO_FORWARD);
  g_signal_connect(forward_button_, "clicked",
                   G_CALLBACK(&OnForwardButtonClickedThunk), this);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), forward_button_, -1 /* append */);
  gtk_widget_add_accelerator(GTK_WIDGET(forward_button_), "clicked",
                             accel_group,
                             GDK_Right, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

  reload_button_ = gtk_tool_button_new_from_stock(GTK_STOCK_REFRESH);
  g_signal_connect(reload_button_, "clicked",
                   G_CALLBACK(&OnReloadButtonClickedThunk), this);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), reload_button_, -1 /* append */);
  gtk_widget_add_accelerator(GTK_WIDGET(reload_button_), "clicked",
                             accel_group,
                             GDK_r, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  stop_button_ = gtk_tool_button_new_from_stock(GTK_STOCK_STOP);
  g_signal_connect(stop_button_, "clicked",
                   G_CALLBACK(&OnStopButtonClickedThunk), this);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), stop_button_, -1 /* append */);

  url_edit_view_ = gtk_entry_new();
  g_signal_connect(G_OBJECT(url_edit_view_), "activate",
                   G_CALLBACK(&OnURLEntryActivateThunk), this);

  gtk_accel_group_connect(
      accel_group, GDK_l, GDK_CONTROL_MASK,
      GTK_ACCEL_VISIBLE,
      g_cclosure_new(G_CALLBACK(OnHighlightURLViewThunk),
                     this, NULL));

  GtkToolItem* tool_item = gtk_tool_item_new();
  gtk_container_add(GTK_CONTAINER(tool_item), url_edit_view_);
  gtk_tool_item_set_expand(tool_item, TRUE);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_item, -1 /* append */);

  // Center a 20x20 spinner in a 26x24 area.
  GtkWidget* spinner_alignment = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_alignment_set_padding(GTK_ALIGNMENT(spinner_alignment), 2, 2, 4, 4);
  spinner_ = gtk_spinner_new();
  gtk_widget_set_size_request(spinner_, 20, 20);
  gtk_container_add(GTK_CONTAINER(spinner_alignment), spinner_);

  spinner_item_ = gtk_tool_item_new();
  gtk_container_add(GTK_CONTAINER(spinner_item_), spinner_alignment);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), spinner_item_, -1 /* append */);

  if (is_toolbar_open_)
    gtk_box_pack_start(GTK_BOX(vbox_), toolbar, FALSE, FALSE, 0);

  gtk_container_add(GTK_CONTAINER(window_), vbox_);
  gtk_widget_show_all(GTK_WIDGET(window_));

  if (CommandLine::ForCurrentProcess()->HasSwitch(switches::kDumpRenderTree))
    gtk_widget_set_uposition(GTK_WIDGET(window_), 10000, 10000);

  SizeTo(width, height);
}

void Shell::PlatformSetContents() {
  WebContentsView* content_view = web_contents_->GetView();
  gtk_container_add(GTK_CONTAINER(vbox_), content_view->GetNativeView());

  // As an additional requirement on Linux, we must set the colors for the
  // render widgets in webkit.
  content::RendererPreferences* prefs =
      web_contents_->GetMutableRendererPrefs();
  prefs->focus_ring_color = SkColorSetARGB(255, 229, 151, 0);
  prefs->thumb_active_color = SkColorSetRGB(244, 244, 244);
  prefs->thumb_inactive_color = SkColorSetRGB(234, 234, 234);
  prefs->track_color = SkColorSetRGB(211, 211, 211);

  prefs->active_selection_bg_color = SkColorSetRGB(30, 144, 255);
  prefs->active_selection_fg_color = SK_ColorWHITE;
  prefs->inactive_selection_bg_color = SkColorSetRGB(200, 200, 200);
  prefs->inactive_selection_fg_color = SkColorSetRGB(50, 50, 50);
}

void Shell::SizeTo(int width, int height) {
  content_width_ = width;
  content_height_ = height;
  if (web_contents_.get()) {
    gtk_widget_set_size_request(web_contents_->GetNativeView(), width, height);
  }
}

void Shell::PlatformResizeSubViews() {
  SizeTo(content_width_, content_height_);
}

void Shell::Close() {
  gtk_widget_destroy(GTK_WIDGET(window_));
}

void Shell::Move(const gfx::Rect& bounds) {
  gint x = static_cast<gint>(bounds.x());
  gint y = static_cast<gint>(bounds.y());
  gint width = static_cast<gint>(bounds.width());
  gint height = static_cast<gint>(bounds.height());

  gtk_window_move(window_, x, y);
  gtk_window_resize(window_, width, height);
}

void Shell::OnBackButtonClicked(GtkWidget* widget) {
  GoBackOrForward(-1);
}

void Shell::OnForwardButtonClicked(GtkWidget* widget) {
  GoBackOrForward(1);
}

void Shell::OnReloadButtonClicked(GtkWidget* widget) {
  Reload();
}

void Shell::OnStopButtonClicked(GtkWidget* widget) {
  Stop();
}

void Shell::OnURLEntryActivate(GtkWidget* entry) {
  const gchar* str = gtk_entry_get_text(GTK_ENTRY(entry));
  GURL url(str);
  if (!url.has_scheme())
    url = GURL(std::string("http://") + std::string(str));
  LoadURL(GURL(url));
}

// Callback for when the main window is destroyed.
gboolean Shell::OnWindowDestroyed(GtkWidget* window) {
  delete this;
  return FALSE;  // Don't stop this message.
}

gboolean Shell::OnCloseWindowKeyPressed(GtkAccelGroup* accel_group,
                                        GObject* acceleratable,
                                        guint keyval,
                                        GdkModifierType modifier) {
  gtk_widget_destroy(GTK_WIDGET(window_));
  return TRUE;
}

gboolean Shell::OnNewWindowKeyPressed(GtkAccelGroup* accel_group,
                                      GObject* acceleratable,
                                      guint keyval,
                                      GdkModifierType modifier) {
  ShellBrowserContext* browser_context =
      static_cast<ShellContentBrowserClient*>(
        GetContentClient()->browser())->browser_context();
  Shell::CreateNewWindow(browser_context,
                         GURL(),
                         NULL,
                         MSG_ROUTING_NONE,
                         NULL);
  return TRUE;
}

gboolean Shell::OnHighlightURLView(GtkAccelGroup* accel_group,
                                   GObject* acceleratable,
                                   guint keyval,
                                   GdkModifierType modifier) {
  gtk_widget_grab_focus(GTK_WIDGET(url_edit_view_));
  return TRUE;
}

void Shell::PlatformSetTitle(const string16& title) {
  std::string title_utf8 = UTF16ToUTF8(title);
  gtk_window_set_title(GTK_WINDOW(window_), title_utf8.c_str());
}

}  // namespace content
