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

#include "base/file_util.h"
#include "base/values.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_shell.h"
#include "ui/gfx/rect.h"
#include "ui/gfx/image/image_skia_rep.h"
#include "webkit/glue/image_decoder.h"

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

  // Setup window from manifest.
  int x, y;
  std::string position;
  if (manifest->GetInteger(switches::kmX, &x) &&
      manifest->GetInteger(switches::kmY, &y)) {
    int width, height;
    manifest->GetInteger(switches::kmWidth, &width);
    manifest->GetInteger(switches::kmHeight, &height);
    window->Move(gfx::Rect(x, y, width, height));
  } else if (manifest->GetString(switches::kmPosition, &position)) {
    window->SetPosition(position);
  }
  int min_height, min_width;
  if (manifest->GetInteger(switches::kmMinHeight, &min_height) &&
      manifest->GetInteger(switches::kmMinWidth, &min_width)) {
    window->SetMinimumSize(min_width, min_height);
  }
  int max_height, max_width;
  if (manifest->GetInteger(switches::kmMaxHeight, &max_height) &&
      manifest->GetInteger(switches::kmMaxWidth, &max_width)) {
    window->SetMaximumSize(max_width, max_height);
  }
  bool resizable;
  if (manifest->GetBoolean(switches::kmResizable, &resizable)) {
    window->SetResizable(resizable);
  }
  bool fullscreen;
  if (manifest->GetBoolean(switches::kmFullscreen, &fullscreen) && fullscreen) {
    window->SetFullscreen(true);
  }
  bool toolbar = true;
  manifest->GetBoolean(switches::kmToolbar, &toolbar);
  if (toolbar) {
    window->AddToolbar();
  }
  std::string title("node-webkit");
  manifest->GetString(switches::kmTitle, &title);
  window->SetTitle(title);

  // Then show it.
  bool show = true;
  manifest->GetBoolean(switches::kmShow, &show);
  if (show)
    window->Show();

  return window;
}

NativeWindow::NativeWindow(content::Shell* shell,
                           base::DictionaryValue* manifest)
    : shell_(shell),
      has_frame_(true) {
  manifest->GetBoolean(switches::kmFrame, &has_frame_);

  LoadAppIconFromPackage();
}

NativeWindow::~NativeWindow() {
}

content::WebContents* NativeWindow::web_contents() const {
  return shell_->web_contents();
}

void NativeWindow::LoadAppIconFromPackage() {
  base::DictionaryValue* window = shell_->GetPackage()->window();
  std::string path_string;
  if (window->GetString(switches::kmIcon, &path_string)) {
    // Get icon path.
    FilePath path(shell_->GetPackage()->ConvertToAbsoutePath(
        FilePath::FromUTF8Unsafe(path_string)));

    // Read the file from disk.
    std::string file_contents;
    if (path.empty() || !file_util::ReadFileToString(path, &file_contents))
      return;

    // Decode the bitmap using WebKit's image decoder.
    const unsigned char* data =
        reinterpret_cast<const unsigned char*>(file_contents.data());
    webkit_glue::ImageDecoder decoder;
    scoped_ptr<SkBitmap> decoded(new SkBitmap());
    // Note: This class only decodes bitmaps from extension resources. Chrome
    // doesn't (for security reasons) directly load extension resources provided
    // by the extension author, but instead decodes them in a separate
    // locked-down utility process. Only if the decoding succeeds is the image
    // saved from memory to disk and subsequently used in the Chrome UI.
    // Chrome is therefore decoding bitmaps here that were generated by Chrome.
    *decoded = decoder.Decode(data, file_contents.length());
    if (decoded->empty())
      return;  // Unable to decode.

    app_icon_ = gfx::Image(*decoded.release());
  }
}

}  // namespace nw
