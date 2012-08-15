// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/shell.h"

#include <jni.h>

#include "base/android/scoped_java_ref.h"
#include "base/android/jni_string.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/string_piece.h"
#include "content/public/common/content_switches.h"
#include "content/shell/android/shell_manager.h"
#include "jni/Shell_jni.h"

using base::android::AttachCurrentThread;
using base::android::ConvertUTF8ToJavaString;

namespace content {

void Shell::PlatformInitialize() {
  CommandLine* command_line = CommandLine::ForCurrentProcess();
  DCHECK(command_line->HasSwitch(switches::kForceCompositingMode));
  DCHECK(command_line->HasSwitch(switches::kEnableThreadedCompositing));
}

void Shell::PlatformCleanUp() {
}

void Shell::PlatformEnableUIControl(UIControl control, bool is_enabled) {
}

void Shell::PlatformSetAddressBarURL(const GURL& url) {
  JNIEnv* env = AttachCurrentThread();
  ScopedJavaLocalRef<jstring> j_url =
        ConvertUTF8ToJavaString(env, url.spec());
  Java_Shell_onUpdateUrl(env, java_object_.obj(), j_url.obj());
}

void Shell::PlatformSetIsLoading(bool loading) {
}

void Shell::PlatformCreateWindow(int width, int height) {
  java_object_.Reset(AttachCurrentThread(), CreateShellView());
}

void Shell::PlatformSetContents() {
  JNIEnv* env = AttachCurrentThread();
  Java_Shell_initFromNativeTabContents(
      env, java_object_.obj(), reinterpret_cast<jint>(web_contents()));
}

void Shell::PlatformResizeSubViews() {
  // Not needed; subviews are bound.
}

void Shell::PlatformSetTitle(const string16& title) {
  NOTIMPLEMENTED();
}

void Shell::LoadProgressChanged(double progress) {
  JNIEnv* env = AttachCurrentThread();
  Java_Shell_onLoadProgressChanged(env, java_object_.obj(), progress);
}

void Shell::Close() {
  // TODO(tedchoc): Implement Close method for android shell
  NOTIMPLEMENTED();
}

// static
bool Shell::Register(JNIEnv* env) {
  return RegisterNativesImpl(env);
}

}  // namespace content
