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

#include "content/nw/src/browser/native_window.h"

#include "base/values.h"
#include "content/nw/src/browser/capture_page_helper.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_shell.h"
#include "grit/nw_resources.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/gfx/rect.h"

#if defined(OS_MACOSX)
#include "content/nw/src/browser/native_window_helper_mac.h"
#elif defined(TOOLKIT_GTK)
#include "content/nw/src/browser/native_window_gtk.h"
#elif defined(OS_WIN)
#include "content/nw/src/browser/native_window_win.h"
#endif

namespace nw {

// static
NativeWindow* NativeWindow::Create(content::Shell* shell,
                                   base::DictionaryValue* manifest) {
  // Set default width/height.
  if (!manifest->HasKey(switches::kmWidth))
    manifest->SetInteger(switches::kmWidth, 700);
  if (!manifest->HasKey(switches::kmHeight))
    manifest->SetInteger(switches::kmHeight, 450);

  // Create window.
  NativeWindow* window = 
#if defined(TOOLKIT_GTK)
      new NativeWindowGtk(shell, manifest);
#elif defined(OS_MACOSX)
      CreateNativeWindowCocoa(shell, manifest);
#elif defined(OS_WIN)
      new NativeWindowWin(shell, manifest);
#else
      NULL;
  NOTREACHED() << "Cannot create native window on unsupported platform.";
#endif

  return window;
}

NativeWindow::NativeWindow(content::Shell* shell,
                           base::DictionaryValue* manifest)
    : shell_(shell),
      has_frame_(true),
      capture_page_helper_(NULL) {
  bool transparent;
  if(manifest->GetBoolean(switches::kmTransparent, &transparent) && transparent)
    has_frame_ = false;
  else
    manifest->GetBoolean(switches::kmFrame, &has_frame_);

  LoadAppIconFromPackage(manifest);
}

NativeWindow::~NativeWindow() {
}

content::WebContents* NativeWindow::web_contents() const {
  return shell_->web_contents();
}

void NativeWindow::InitFromManifest(base::DictionaryValue* manifest) {
  // Setup window from manifest.
  int x, y;
  std::string position;
  if (manifest->GetInteger(switches::kmX, &x) &&
      manifest->GetInteger(switches::kmY, &y)) {
    SetPosition(gfx::Point(x, y));
  } else if (manifest->GetString(switches::kmPosition, &position)) {
    SetPosition(position);
  }
  int min_height = 0, min_width = 0;
  if (manifest->GetInteger(switches::kmMinHeight, &min_height) |
      manifest->GetInteger(switches::kmMinWidth, &min_width)) {
    SetMinimumSize(min_width, min_height);
  }
  int max_height = INT_MAX, max_width = INT_MAX;
#if defined(OS_WIN)
  // On Windows max_height and max_width will be filled with max length when
  // it's zero. 
  max_height = 0;
  max_width = 0;
#endif
  if (manifest->GetInteger(switches::kmMaxHeight, &max_height) |
      manifest->GetInteger(switches::kmMaxWidth, &max_width)) {
    SetMaximumSize(max_width, max_height);
  }
  bool resizable;
  if (manifest->GetBoolean(switches::kmResizable, &resizable)) {
    SetResizable(resizable);
  }
  bool top;
  if (manifest->GetBoolean(switches::kmAlwaysOnTop, &top) && top) {
    SetAlwaysOnTop(true);
  }
  bool fullscreen;
  if (manifest->GetBoolean(switches::kmFullscreen, &fullscreen) && fullscreen) {
    SetFullscreen(true);
  }
  bool kiosk;
  if (manifest->GetBoolean(switches::kmKiosk, &kiosk) && kiosk) {
    SetKiosk(kiosk);
  }
  bool transparent;
  if (manifest->GetBoolean(switches::kmTransparent, &transparent) && transparent) {
    SetTransparent();

    /* Transparent windows cannot have toolbars or other window controls */
    manifest->SetBoolean(switches::kmToolbar, false);
  }
  bool toolbar = true;
  manifest->GetBoolean(switches::kmToolbar, &toolbar);
  if (toolbar) {
    AddToolbar();
  }
  std::string title("node-webkit");
  manifest->GetString(switches::kmTitle, &title);
  SetTitle(title);

  // Then show it.
  bool show = true;
  manifest->GetBoolean(switches::kmShow, &show);
  if (show)
    Show();
}

void NativeWindow::CapturePage(const std::string& image_format) {
  // Lazily instance CapturePageHelper.
  if (capture_page_helper_ == NULL)
    capture_page_helper_ = CapturePageHelper::Create(shell_);

  capture_page_helper_->StartCapturePage(image_format);
}

void NativeWindow::LoadAppIconFromPackage(base::DictionaryValue* manifest) {
  std::string path_string;
  if (manifest->GetString(switches::kmIcon, &path_string)) {
    // Read icon from "icon" field.
    shell_->GetPackage()->GetImage(FilePath::FromUTF8Unsafe(path_string),
         &app_icon_);
  } else {
    // Set default icon.
    app_icon_ = ui::ResourceBundle::GetSharedInstance().
        GetNativeImageNamed(IDR_NW_DEFAULT_ICON);
  }
}

}  // namespace nw
