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

namespace nwebkit {

using namespace v8;

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
          ctx->afd))
      )
  {
    free (ctx->pfd);
    free (ctx->iow);

    ctx->afd = 1;
    while (ctx->afd < ctx->nfd)
      ctx->afd <<= 1;

    ctx->pfd = (GPollFD*) malloc (ctx->afd * sizeof (GPollFD));
    ctx->iow = (ev_io*) malloc (ctx->afd * sizeof (ev_io));
  }

  for (i = 0; i < ctx->nfd; ++i)
  {
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

  if (timeout >= 0)
  {
    ev_timer_set (&ctx->tw, timeout * 1e-3, 0.);
    ev_timer_start (EV_A_ &ctx->tw);
  }
}

static void check_cb (EV_P_ ev_check *w, int revents) {

  struct econtext *ctx = (struct econtext *)(((char *)w) - offsetof (struct econtext, cw));
  int i;

  for (i = 0; i < ctx->nfd; ++i)
  {
    ev_io *iow = ctx->iow + i;

    if (ev_is_pending (iow))
    {
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
  //extern "C" {
  void nwebkit_view_init(const char*, int, int);
  //}

// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value) {
  return *value ? *value : "<string conversion failed>";
}

static inline v8::Local<v8::Value> CompileRun(const char* source) {
  return v8::Script::Compile(v8::String::New(source))->Run();
}

static Handle<Value> GtkInit (const Arguments &args) {

  HandleScope scope;
  v8::Local<v8::Context> entered, current;
  Local<Object> thisObj = args.This();
  Local<Object> options = Local<Object>::Cast(args[1]);

  v8::String::Utf8Value str(options->Get(v8::String::New("url")));
  const char* cstr = ToCString(str);
  int width  = options->Get(v8::String::New("width"))->Int32Value();
  int height = options->Get(v8::String::New("height"))->Int32Value();
  nwebkit_view_init(cstr, width, height);
  return Undefined();
}

void init(Handle<Object> target) {
  HandleScope scope;

  if (!g_thread_supported())
    g_thread_init(NULL);

  target->Set(v8::String::NewSymbol("_init"),
	      v8::FunctionTemplate::New(GtkInit)->GetFunction());

  GMainContext *gc     = g_main_context_default();
  struct econtext *ctx = &default_context;

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
  NODE_MODULE(nwebkit, init)
} // namespace nwebkit
