// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/android/shell_manager.h"

#include "base/android/jni_android.h"
#include "base/android/jni_string.h"
#include "base/android/scoped_java_ref.h"
#include "base/bind.h"
#include "base/lazy_instance.h"
#include "content/shell/shell.h"
#include "content/shell/shell_browser_context.h"
#include "content/shell/shell_content_browser_client.h"
#include "content/public/browser/android/draw_delegate.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/web_contents.h"
#include "content/shell/android/draw_context.h"
#include "content/shell/shell.h"
#include "googleurl/src/gurl.h"
#include "jni/ShellManager_jni.h"
#include "ui/gfx/size.h"

#include <android/native_window_jni.h>

using base::android::ScopedJavaLocalRef;
using content::DrawContext;
using content::DrawDelegate;

namespace {

struct GlobalState {
  GlobalState()
      : context(NULL) {
  }
  base::android::ScopedJavaGlobalRef<jobject> j_obj;
  DrawContext* context;
};

base::LazyInstance<GlobalState> g_global_state = LAZY_INSTANCE_INITIALIZER;

static void SurfaceUpdated(
    uint64 texture,
    content::RenderWidgetHostView* view,
    const DrawDelegate::SurfacePresentedCallback& callback) {
  uint32 sync_point = g_global_state.Get().context->Draw(texture);
  callback.Run(sync_point);
}

} // anonymous namespace

namespace content {

jobject CreateShellView() {
  JNIEnv* env = base::android::AttachCurrentThread();
  return Java_ShellManager_createShell(
      env, g_global_state.Get().j_obj.obj()).Release();
}

// Register native methods
bool RegisterShellManager(JNIEnv* env) {
  return RegisterNativesImpl(env);
}

static void Init(JNIEnv* env, jclass clazz, jobject obj) {
  g_global_state.Get().j_obj.Reset(
      base::android::ScopedJavaLocalRef<jobject>(env, obj));
  DrawDelegate::SurfaceUpdatedCallback cb = base::Bind(
      &SurfaceUpdated);
  DrawDelegate::GetInstance()->SetUpdateCallback(cb);
}

static void SurfaceCreated(
    JNIEnv* env, jclass clazz, jobject jsurface) {
  ANativeWindow* native_window = ANativeWindow_fromSurface(env, jsurface);
  if (native_window) {
    if (g_global_state.Get().context)
      delete g_global_state.Get().context;

    g_global_state.Get().context = new DrawContext(native_window);
    ANativeWindow_release(native_window);
  }
}

static void SurfaceDestroyed(JNIEnv* env, jclass clazz) {
  if (g_global_state.Get().context)
    delete g_global_state.Get().context;
}

static void SurfaceSetSize(
    JNIEnv* env, jclass clazz, jint width, jint height) {
  gfx::Size size = gfx::Size(width, height);
  DrawDelegate::GetInstance()->SetBounds(size);
  DCHECK(g_global_state.Get().context);
  g_global_state.Get().context->Reshape(width, height);
}

void LaunchShell(JNIEnv* env, jclass clazz, jstring jurl) {
  ShellBrowserContext* browserContext =
      static_cast<ShellContentBrowserClient*>(
          GetContentClient()->browser())->browser_context();
  GURL url(base::android::ConvertJavaStringToUTF8(env, jurl));
  Shell::CreateNewWindow(browserContext,
                         url,
                         NULL,
                         MSG_ROUTING_NONE,
                         NULL);
}

}  // namespace content
