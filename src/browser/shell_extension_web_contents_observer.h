// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EXTENSIONS_SHELL_BROWSER_SHELL_EXTENSION_WEB_CONTENTS_OBSERVER_H_
#define EXTENSIONS_SHELL_BROWSER_SHELL_EXTENSION_WEB_CONTENTS_OBSERVER_H_

#include "content/public/browser/web_contents_user_data.h"
#include "extensions/browser/extension_web_contents_observer.h"

namespace extensions {

// The app_shell version of ExtensionWebContentsObserver.
class ShellExtensionWebContentsObserver
    : public ExtensionWebContentsObserver,
      public content::WebContentsUserData<ShellExtensionWebContentsObserver> {
 private:
  friend class content::WebContentsUserData<ShellExtensionWebContentsObserver>;

  explicit ShellExtensionWebContentsObserver(
      content::WebContents* web_contents);
  ~ShellExtensionWebContentsObserver() override;

  DISALLOW_COPY_AND_ASSIGN(ShellExtensionWebContentsObserver);
};

}  // namespace extensions

#endif  // EXTENSIONS_SHELL_BROWSER_SHELL_EXTENSION_WEB_CONTENTS_OBSERVER_H_
