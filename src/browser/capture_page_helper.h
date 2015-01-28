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

#ifndef CONTENT_NW_SRC_BROWSER_CAPTURE_PAGE_HELPER_H_
#define CONTENT_NW_SRC_BROWSER_CAPTURE_PAGE_HELPER_H_

#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/public/browser/readback_types.h"

namespace content {
class Shell;
}

namespace skia {
class PlatformBitmap;
}

class SkBitmap;

namespace nw {

namespace capture_page_helper_constants {

extern const char kFormatValueJpeg[];
extern const char kFormatValuePng[];
extern const char kMimeTypeJpeg[];
extern const char kMimeTypePng[];

// The default quality setting used when encoding jpegs.
extern const int kDefaultQuality;

}; // namespace capture_page_helper_constants

class CapturePageHelper : public base::RefCountedThreadSafe<CapturePageHelper>,
                      public content::WebContentsObserver {
 public:
  enum ImageFormat {
    FORMAT_JPEG,
    FORMAT_PNG
  };

  static scoped_refptr<CapturePageHelper> Create(const base::WeakPtr<content::Shell>& shell);

  // Capture a snapshot of the page.
  void StartCapturePage(const std::string& image_format_str);

 private:
  CapturePageHelper(const base::WeakPtr<content::Shell>& shell);
  ~CapturePageHelper() override;

  // Internal helpers ----------------------------------------------------------

  // Message handler.
  void OnSnapshot(const SkBitmap& bitmap);

  void CopyFromBackingStoreComplete(const SkBitmap& bitmap, content::ReadbackResponse response);
  void SendResultFromBitmap(const SkBitmap& screen_capture);

  // content::WebContentsObserver overrides:
  bool OnMessageReceived(const IPC::Message& message) override;

  base::WeakPtr<content::Shell> shell_;

  // The format (JPEG vs PNG) of the resulting image. Set in StartCapturePage().
  ImageFormat image_format_;
  friend class base::RefCountedThreadSafe<CapturePageHelper>;
};

}; // namespace nw

#endif
