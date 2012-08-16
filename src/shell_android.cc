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
