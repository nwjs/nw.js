//
// Copyright (c) 2010 Illarionov Oleg, Marc Lehmann and Tim Smart.
// Copyright (c) 2011 Intel Corp.
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

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

// The EV-Glib section of this source was derived from the EV::Glib
// module which was relicensed with permission from Marc Lehmann.

#include "nwebkit.h"

#include <ev.h>
#include <stdlib.h>
#include <webkit/webkit.h>

using v8::Persistent;
using v8::Function;

#include "context_wrap.h"

#define NWEBKIT_DEF_METHOD(obj,name,func)			\
  obj->Set(v8::String::NewSymbol(name),				\
	   v8::FunctionTemplate::New(func)->GetFunction())

namespace nwebkit {

using namespace v8;
static WebKitWebView* _webview;
static Persistent<Function> _init_call_back;
struct econtext {
  GPollFD *pfd;
  ev_io *iow;
  int nfd, afd;
  gint maxpri;

  ev_prepare pw;
  ev_check cw;
  ev_timer tw;

  GMainContext *gc;
};

static  WebKitWebView* nwebkit_view_init(const char*, int, int);
static  gchar* filename_to_url(const char* filename);

static void timer_cb (EV_P_ ev_timer *w, int revents) {
  /* nop */
}

static void io_cb (EV_P_ ev_io *w, int revents) {
  /* nop */
}

static void prepare_cb (EV_P_ ev_prepare *w, int revents) {
  struct econtext *ctx = (struct econtext *)(((char *)w) - offsetof (struct econtext, pw));
  gint timeout;
  int i;

  g_main_context_dispatch (ctx->gc);

  g_main_context_prepare (ctx->gc, &ctx->maxpri);

  while (ctx->afd < (ctx->nfd = g_main_context_query  (
						       ctx->gc,
						       ctx->maxpri,
						       &timeout,
						       ctx->pfd,
						       ctx->afd))) {
    free (ctx->pfd);
    free (ctx->iow);

    ctx->afd = 1;
    while (ctx->afd < ctx->nfd)
      ctx->afd <<= 1;

    ctx->pfd = (GPollFD*) malloc (ctx->afd * sizeof (GPollFD));
    ctx->iow = (ev_io*) malloc (ctx->afd * sizeof (ev_io));
  }

  for (i = 0; i < ctx->nfd; ++i) {
    GPollFD *pfd = ctx->pfd + i;
    ev_io *iow = ctx->iow + i;

    pfd->revents = 0;

    ev_io_init (
		iow,
		io_cb,
		pfd->fd,
		(pfd->events & G_IO_IN ? EV_READ : 0)
		| (pfd->events & G_IO_OUT ? EV_WRITE : 0)
		);
    iow->data = (void *)pfd;
    ev_set_priority (iow, EV_MINPRI);
    ev_io_start (EV_A_ iow);
  }

  if (timeout >= 0) {
    //if (timeout == 0)
    //  g_main_context_dispatch (ctx->gc);

    ev_timer_set (&ctx->tw, timeout * 1e-3, 0.);
    ev_timer_start (EV_A_ &ctx->tw);
  }
}

static void check_cb (EV_P_ ev_check *w, int revents) {

  struct econtext *ctx = (struct econtext *)(((char *)w) - offsetof (struct econtext, cw));
  int i;

  for (i = 0; i < ctx->nfd; ++i) {
    ev_io *iow = ctx->iow + i;

    if (ev_is_pending (iow))	{
      GPollFD *pfd = ctx->pfd + i;
      int revents = ev_clear_pending (EV_A_ iow);

      pfd->revents |= pfd->events &
	((revents & EV_READ ? G_IO_IN : 0)
	 | (revents & EV_WRITE ? G_IO_OUT : 0));
    }

    ev_io_stop (EV_A_ iow);
  }

  if (ev_is_active (&ctx->tw))
    ev_timer_stop (EV_A_ &ctx->tw);

  if (ctx->nfd || GLIB_CHECK_VERSION (2, 30, 0))
    // on glib 2.28 passing nfd as 0 will hang
    // if we don't check with nfd == 0 on 2.30, it will also have
    // issues printing lots of warnings
    g_main_context_check (ctx->gc, ctx->maxpri, ctx->pfd, ctx->nfd);
}

static struct econtext default_context;

// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value) {
  return *value ? *value : "<string conversion failed>";
}

static inline v8::Local<v8::Value> CompileRun(const char* source) {
  return v8::Script::Compile(v8::String::New(source))->Run();
}

static void load_status_cb(WebKitWebView* webview, GParamSpec* pspec, void* cb)
{
  WebKitLoadStatus status = webkit_web_view_get_load_status (webview);
  if (status == WEBKIT_LOAD_FINISHED || status == WEBKIT_LOAD_FAILED) {
    Persistent<Function> callback = *(Persistent<Function>*)cb;
    int argc = 1;
    Local<Value> argv[1];
    if (status == WEBKIT_LOAD_FAILED) {
      argv[0] = Exception::Error (String::New ("webpage load failed"));
    }else{
      argv[0] = Local<Value>::New(Null());
    }
    callback->Call (callback, argc, argv);
  }
}

static Handle<Value> GtkInit (const Arguments &args) {

  HandleScope scope;
  v8::Local<v8::Context> entered, current;
  Local<Object> options = Local<Object>::Cast(args[1]);

  Local<Value> url = options->Get(v8::String::New("url"));
  v8::String::Utf8Value str(url);
  const char* cstr = ToCString(str);
  if (url->IsUndefined()) {
    cstr = "";
  }
  int width  = options->Get(v8::String::New("width"))->Int32Value();
  int height = options->Get(v8::String::New("height"))->Int32Value();
  _webview = nwebkit_view_init(cstr, width, height);

  WebKitWebSettings *settings = webkit_web_settings_new ();
  g_object_set (G_OBJECT(settings), "enable-file-access-from-file-uris",
		TRUE, NULL);

  webkit_web_view_set_settings (WEBKIT_WEB_VIEW(_webview), settings);

  if (args[2]->IsFunction()) {
    _init_call_back = Persistent<Function>::New(Handle<Function>::Cast(args[2]));
    g_signal_connect (_webview, "notify::load-status", G_CALLBACK(load_status_cb), &_init_call_back);
  }else{
    _init_call_back.Clear ();
  }
  return args.This();
}

static Handle<Value> Open (const Arguments &args) {

  HandleScope scope;

  Local<String> arg0 = args[0]->ToString();

  v8::String::Utf8Value str(arg0);
  const char* uri = ToCString(str);
  if (arg0->IsUndefined()) {
    uri = "";
  }
  gchar *url = filename_to_url(uri);
  webkit_web_view_load_uri(_webview, url ? url : uri);

  g_free(url);

  return args.This();
}

static Handle<Value> LoadString (const Arguments &args) {

  HandleScope scope;

  Local<String> arg0 = args[0]->ToString();

  String::Utf8Value str(arg0);
  const char* content = ToCString(str);
  const char* mime_type = NULL;
  const char* base_uri = "";
  String::Utf8Value arg1(args[1]->ToString());
  String::Utf8Value arg2(args[2]->ToString());

  if (args[1]->IsString())
    mime_type = *arg1;
  if (args[2]->IsString())
    base_uri = *arg2;

  webkit_web_view_load_string (_webview, content, mime_type, NULL, base_uri);

  return args.This();
}

static Handle<Value> Reload (const Arguments &args) {
  webkit_web_view_reload (_webview);
  return args.This();
}

static Handle<Value> SetViewMode (const Arguments &args) {

  int mode;
  if (args[0]->IsNumber()) {
    mode = args[1]->Int32Value();
  } else {
    mode = WEBKIT_WEB_VIEW_VIEW_MODE_WINDOWED;
  }

  webkit_web_view_set_view_mode (_webview, (WebKitWebViewViewMode)mode);

  return args.This();
}


void init(Handle<Object> target) {
  HandleScope scope;

  if (!g_thread_supported())
    g_thread_init(NULL);

  NWEBKIT_DEF_METHOD (target, "_init", GtkInit);
  NWEBKIT_DEF_METHOD (target, "open",  Open);
  NWEBKIT_DEF_METHOD (target, "loadString", LoadString);
  NWEBKIT_DEF_METHOD (target, "reload", Reload);
  NWEBKIT_DEF_METHOD (target, "setViewMode", SetViewMode);

  ContextWrap::Initialize (target);
  GMainContext *gc     = g_main_context_default();
  struct econtext *ctx = &default_context;

  // ev thread need to be the owner of the context so it can be wake
  // up properly by g_source_attach(idle source) from the i/o thread
  // launched by webkit/libsoup

  g_main_context_acquire (gc);
  ctx->gc  = g_main_context_ref(gc);
  ctx->nfd = 0;
  ctx->afd = 0;
  ctx->iow = 0;
  ctx->pfd = 0;

  ev_prepare_init (&ctx->pw, prepare_cb);
  ev_set_priority (&ctx->pw, EV_MINPRI);
  ev_prepare_start (EV_DEFAULT_UC_ &ctx->pw);
  ev_unref(EV_DEFAULT_UC);

  ev_check_init (&ctx->cw, check_cb);
  ev_set_priority (&ctx->cw, EV_MAXPRI);
  ev_check_start (EV_DEFAULT_UC_ &ctx->cw);
  ev_unref(EV_DEFAULT_UC);

  ev_init (&ctx->tw, timer_cb);
  ev_set_priority (&ctx->tw, EV_MINPRI);
}

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
    gtk_window_set_title (GTK_WINDOW (window), title);
  }
}


static gchar* filename_to_url(const char* filename)
{
  if (!g_file_test(filename, G_FILE_TEST_EXISTS))
    return 0;

  GFile *gfile = g_file_new_for_path(filename);
  gchar *file_url = g_file_get_uri(gfile);
  g_object_unref(gfile);

  return file_url;
}

static WebKitWebView* nwebkit_view_init(const char* uri, int width, int height)
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

NODE_MODULE(nwebkit, init)
} // namespace nwebkit
