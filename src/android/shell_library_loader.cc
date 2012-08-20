// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/basictypes.h"
#include "base/debug/debugger.h"
#include "base/logging.h"
#include "base/android/jni_android.h"
#include "base/android/jni_registrar.h"
#include "content/public/app/android_library_loader_hooks.h"
#include "content/public/app/content_main.h"
#include "content/shell/android/shell_manager.h"
#include "content/shell/shell.h"
#include "content/shell/shell_main_delegate.h"

static base::android::RegistrationMethod kRegistrationMethods[] = {
    { "Shell", content::Shell::Register },
    { "ShellManager", content::RegisterShellManager },
};

// This is called by the VM when the shared library is first loaded.
JNI_EXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
  base::android::InitVM(vm);
  JNIEnv* env = base::android::AttachCurrentThread();
  if (!content::RegisterLibraryLoaderEntryHook(env)) {
    return -1;
  }

  // To be called only from the UI thread.  If loading the library is done on
  // a separate thread, this should be moved elsewhere.
  if (!base::android::RegisterNativeMethods(env, kRegistrationMethods,
                                            arraysize(kRegistrationMethods)))
    return -1;

  content::SetContentMainDelegate(new content::ShellMainDelegate());

  return JNI_VERSION_1_4;
}
