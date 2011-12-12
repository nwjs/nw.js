//
// Copyright Intel Corp.  All rights reserved.  Permission is hereby
// granted, free of charge, to any person obtaining a copy of this
// software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without
// limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject
// to the following conditions:

// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#include <errno.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <webkit/webkit.h>
#include <ev.h>


static void destroy_cb(GtkWidget* widget, GtkWidget* window)
{
  ev_unref(EV_DEFAULT_UC);
}


static void title_change_cb (WebKitWebView* webview,
			     GParamSpec* pspec,
			     GtkWidget* window)
{
  const gchar* title = webkit_web_view_get_title (WEBKIT_WEB_VIEW (webview));
  if (title) {
    fprintf (stderr, "new title: %s\n", title);
    gtk_window_set_title (GTK_WINDOW (window), title);
  }
}

namespace nwebkit {
gchar* filename_to_url(const char* filename)
{
    if (!g_file_test(filename, G_FILE_TEST_EXISTS))
        return 0;

    GFile *gfile = g_file_new_for_path(filename);
    gchar *file_url = g_file_get_uri(gfile);
    g_object_unref(gfile);

    return file_url;
}

  WebKitWebView* nwebkit_view_init(const char* uri, int width, int height)
  {
    WebKitWebView *webview;
    gtk_init (NULL, NULL);

    GtkWidget *window;
    gchar *url = filename_to_url(uri);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    if (!width || !height)
      gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    else
      gtk_window_set_default_size(GTK_WINDOW(window), width, height);

    webview = WEBKIT_WEB_VIEW(webkit_web_view_new());
    GtkWidget *scrolled_win = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scrolled_win),
				    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    gtk_container_add (GTK_CONTAINER(scrolled_win), GTK_WIDGET(webview));
    gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET(scrolled_win));

    g_signal_connect (window, "destroy", G_CALLBACK(destroy_cb), NULL);
    g_signal_connect (webview, "notify::title",
		      G_CALLBACK(title_change_cb), window);

    webkit_web_view_load_uri(webview, url ? url : uri);
    g_free(url);

    gtk_widget_grab_focus(GTK_WIDGET(webview));
    gtk_widget_show_all(window);
    ev_ref(EV_DEFAULT_UC);

    return webview;
  }

}
