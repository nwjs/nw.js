// Copyright (c) 2011 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

#include "base/values.h"
#include "base/file_path.h"
#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "include/cef_runnable.h"
#include "nw/nw.h"
#include "nw/client_handler.h"
#include "nw/client_switches.h"
#include "nw/string_util.h"

char szWorkingDir[512];  // The current working directory

// The global ClientHandler reference.
extern CefRefPtr<ClientHandler> g_handler;

void destroy(void) {
  CefQuitMessageLoop();
}

void TerminationSignalHandler(int signatl) {
  destroy();
}

// Callback for when you click the back button.
void BackButtonClicked(GtkButton* button) {
  if (g_handler.get() && g_handler->GetBrowserId())
    g_handler->GetBrowser()->GoBack();
}

// Callback for when you click the forward button.
void ForwardButtonClicked(GtkButton* button) {
  if (g_handler.get() && g_handler->GetBrowserId())
    g_handler->GetBrowser()->GoForward();
}

// Callback for when you click the stop button.
void StopButtonClicked(GtkButton* button) {
  if (g_handler.get() && g_handler->GetBrowserId())
    g_handler->GetBrowser()->StopLoad();
}

// Callback for when you click the reload button.
void ReloadButtonClicked(GtkButton* button) {
  if (g_handler.get() && g_handler->GetBrowserId())
    g_handler->GetBrowser()->Reload();
}

// Callback for when you press enter in the URL box.
void URLEntryActivate(GtkEntry* entry) {
  if (!g_handler.get() || !g_handler->GetBrowserId())
    return;

  const gchar* url = gtk_entry_get_text(entry);
  g_handler->GetBrowser()->GetMainFrame()->LoadURL(std::string(url).c_str());
}

// WebViewDelegate::TakeFocus in the test webview delegate.
static gboolean HandleFocus(GtkWidget* widget,
                            GdkEventFocus* focus) {
  if (g_handler.get() && g_handler->GetBrowserId()) {
    // Give focus to the browser window.
    g_handler->GetBrowser()->GetHost()->SetFocus(true);
  }

  return TRUE;
}

int main(int argc, char* argv[]) {
  CefMainArgs main_args(argc, argv);
  CefRefPtr<ClientApp> app(new ClientApp);

  // Execute the secondary process, if any.
  int exit_code = CefExecuteProcess(main_args, app.get());
  if (exit_code >= 0)
    return exit_code;

  if (!getcwd(szWorkingDir, sizeof (szWorkingDir)))
    return -1;

  GtkWidget* window;

  gtk_init(&argc, &argv);

  // Parse command line arguments.
  AppInitCommandLine(argc, argv);

  CefSettings settings;

  // Populate the settings based on command line arguments.
  AppGetSettings(settings, app);
  base::DictionaryValue *window_manifest = NULL;
  AppGetManifest()->GetDictionary("window", &window_manifest);

  // Initialize CEF.
  CefInitialize(main_args, settings, app.get());

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  int width = 800;
  int height = 600;
  if (window_manifest) {
    window_manifest->GetInteger(nw::kmWidth, &width);
    window_manifest->GetInteger(nw::kmHeight, &height);
  }

  gtk_window_set_default_size(GTK_WINDOW(window), width, height);

  if (window_manifest) {
    std::string desription;
    if (window_manifest->GetString(nw::kmPosition, &desription)) {
      if (desription == "center")
        gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
      else if (desription == "mouse")
        gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_MOUSE);
    }

    GdkGeometry geometry = { 0 };
    int hints = GDK_HINT_POS;
    int tmp = -1;
    if (window_manifest->GetInteger(nw::kmMinWidth, &tmp)) {
      hints |= GDK_HINT_MIN_SIZE;
      geometry.min_width = tmp;
    } else if (window_manifest->GetInteger(nw::kmMinHeight, &tmp)) {
      hints |= GDK_HINT_MIN_SIZE;
      geometry.min_height = tmp;
    } else if (window_manifest->GetInteger(nw::kmMaxWidth, &tmp)) {
      hints |= GDK_HINT_MAX_SIZE;
      geometry.max_width = tmp;
    } else if (window_manifest->GetInteger(nw::kmMaxHeight, &tmp)) {
      hints |= GDK_HINT_MAX_SIZE;
      geometry.max_height = tmp;
    }
    if (hints != GDK_HINT_POS) {
      gtk_window_set_geometry_hints(
          GTK_WINDOW(window), window, &geometry, (GdkWindowHints)hints);
    }

    std::string icon_path;
    if (window_manifest->GetString(nw::kmIcon, &icon_path)) {
      std::string root;
      window_manifest->GetString(nw::kmRoot, &root);
      FilePath path = FilePath(root).Append(icon_path);

      GError *error;
      gtk_window_set_icon_from_file(
          GTK_WINDOW(window), path.value().c_str(), &error);
    }
  }

  g_signal_connect(window, "focus", G_CALLBACK(&HandleFocus), NULL);

  GtkWidget* vbox = gtk_vbox_new(FALSE, 0);

  GtkWidget* toolbar = gtk_toolbar_new();
  // Turn off the labels on the toolbar buttons.
  gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);

  GtkToolItem* back = gtk_tool_button_new_from_stock(GTK_STOCK_GO_BACK);
  g_signal_connect(back, "clicked",
                   G_CALLBACK(BackButtonClicked), NULL);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), back, -1 /* append */);

  GtkToolItem* forward = gtk_tool_button_new_from_stock(GTK_STOCK_GO_FORWARD);
  g_signal_connect(forward, "clicked",
                   G_CALLBACK(ForwardButtonClicked), NULL);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), forward, -1 /* append */);

  GtkToolItem* reload = gtk_tool_button_new_from_stock(GTK_STOCK_REFRESH);
  g_signal_connect(reload, "clicked",
                   G_CALLBACK(ReloadButtonClicked), NULL);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), reload, -1 /* append */);

  GtkToolItem* stop = gtk_tool_button_new_from_stock(GTK_STOCK_STOP);
  g_signal_connect(stop, "clicked",
                   G_CALLBACK(StopButtonClicked), NULL);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), stop, -1 /* append */);

  GtkWidget* m_editWnd = gtk_entry_new();
  g_signal_connect(G_OBJECT(m_editWnd), "activate",
                   G_CALLBACK(URLEntryActivate), NULL);

  GtkToolItem* tool_item = gtk_tool_item_new();
  gtk_container_add(GTK_CONTAINER(tool_item), m_editWnd);
  gtk_tool_item_set_expand(tool_item, TRUE);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_item, -1);  // append

  bool is_toolbar_open = true;
  if (window_manifest)
    window_manifest->GetBoolean(nw::kmToolbar, &is_toolbar_open);

  if (is_toolbar_open)
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

  g_signal_connect(G_OBJECT(window), "destroy",
                   G_CALLBACK(gtk_widget_destroyed), &window);
  g_signal_connect(G_OBJECT(window), "destroy",
                   G_CALLBACK(destroy), NULL);

  // Create the handler.
  g_handler = new ClientHandler();
  g_handler->SetMainHwnd(vbox);
  g_handler->SetEditHwnd(m_editWnd);
  g_handler->SetButtonHwnds(GTK_WIDGET(back), GTK_WIDGET(forward),
                            GTK_WIDGET(reload), GTK_WIDGET(stop));

  // Create the browser view.
  CefWindowInfo window_info;
  CefBrowserSettings browserSettings;

  // Populate the settings based on command line arguments.
  AppGetBrowserSettings(browserSettings);

  window_info.SetAsChild(vbox);

  CefBrowserHost::CreateBrowserSync(
      window_info, g_handler.get(),
      g_handler->GetStartupURL(), browserSettings);

  gtk_container_add(GTK_CONTAINER(window), vbox);
  gtk_widget_show_all(GTK_WIDGET(window));

  // Install an signal handler so we clean up after ourselves.
  signal(SIGINT, TerminationSignalHandler);
  signal(SIGTERM, TerminationSignalHandler);

  CefRunMessageLoop();

  CefShutdown();

  return 0;
}

// Global functions

std::string AppGetWorkingDirectory() {
  return szWorkingDir;
}
