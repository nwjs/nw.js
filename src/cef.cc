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

#include "nwebkit.h"

#include <stdlib.h>

#include "content/renderer/render_thread_impl.h"
#include "content/renderer/render_view_impl.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebViewClient.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebFrame.h"

using v8::Persistent;
using v8::Function;


#define NWEBKIT_DEF_METHOD(obj,name,func)			\
  obj->Set(v8::String::NewSymbol(name),				\
	   v8::FunctionTemplate::New(func)->GetFunction())

namespace nwebkit {

using namespace v8;

static WebKit::WebView* _webview;
static Persistent<Function> _init_call_back;
static Persistent<Function> _onclose_callback;
static WebKit::WebView* nwebkit_view_init(const char* uri, int width, int height);

const char* ToCString(const v8::String::Utf8Value& value) {
  return *value ? *value : "<string conversion failed>";
}

static void load_status_cb()
{
  int argc = 1;
  v8::Local<v8::Value> argv[1];
  argv[0] = v8::Local<v8::Value>::New(Null());
  if (!_init_call_back.IsEmpty())
    _init_call_back->Call (_init_call_back, argc, argv);
}

static Handle<v8::Value> CefInit (const Arguments &args) {

  HandleScope scope;
  v8::Local<v8::Context> entered, current;
  Local<Object> options = Local<Object>::Cast(args[1]);

  Local<v8::Value> url = options->Get(v8::String::New("url"));
  v8::String::Utf8Value str(url);
  const char* cstr = ToCString(str);
  if (url->IsUndefined()) {
    cstr = "";
  }
  int width  = options->Get(v8::String::New("width"))->Int32Value();
  int height = options->Get(v8::String::New("height"))->Int32Value();
  _webview = nwebkit_view_init(cstr, width, height);

  // WebKitWebSettings *settings = webkit_web_settings_new ();
  // g_object_set (G_OBJECT(settings), "enable-file-access-from-file-uris",
  //       	TRUE, NULL);

  // webkit_web_view_set_settings (WEBKIT_WEB_VIEW(_webview), settings);

  if (args[2]->IsFunction()) {
    _init_call_back = Persistent<Function>::New(Handle<Function>::Cast(args[2]));
    // g_signal_connect (_webview, "notify::load-status", G_CALLBACK(load_status_cb), &_init_call_back);
  }else{
    _init_call_back.Clear ();
  }

  Local<v8::Value> onclose = options->Get(v8::String::New("onclose"));
  if (onclose->IsFunction()) {
    _onclose_callback = Persistent<Function>::New(Handle<Function>::Cast(onclose));
    // g_signal_connect (_webview, "unrealize",
    //                   G_CALLBACK(onclose_cb), &_onclose_callback);
  }else{
    _onclose_callback.Clear ();
  }
  return args.This();
}

static WebKit::WebView* nwebkit_view_init(const char* uri, int width, int height)
{
  WebKit::WebView *webview;
  RenderViewImpl* rv = (RenderViewImpl*)
    MessageLoopForUV::current()->pump_uv()->render_view();
  webview = rv->webview();
  rv->setCallback(load_status_cb);
  WebKit::WebFrame* main_frame = webview->mainFrame();
  GURL url(uri);
  WebKit::WebURLRequest request(url);
  main_frame->loadRequest(request);
  // rv->OpenURL(main_frame, url,
  //             content::Referrer(url, main_frame->referrerPolicy()),
  //             WebKit::WebNavigationPolicyCurrentTab);
  // if (width > 0 && height > 0) {
  //   WebKit::WebRect rec = rv->windowRect();
  //   rec.width = width;
  //   rec.height = height;
  //   rv->setWindowRect(rec);
  // }
  return webview;
}

void init(Handle<Object> target) {
  HandleScope scope;

  NWEBKIT_DEF_METHOD (target, "_init", CefInit);
  // NWEBKIT_DEF_METHOD (target, "open",  Open);
  // NWEBKIT_DEF_METHOD (target, "loadString", LoadString);
  // NWEBKIT_DEF_METHOD (target, "reload", Reload);
  // NWEBKIT_DEF_METHOD (target, "setViewMode", SetViewMode);

  // ContextWrap::Initialize (target);

}

NODE_MODULE(nwebkit, init)
} // namespace nwebkit
