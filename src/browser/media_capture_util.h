// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EXTENSIONS_SHELL_BROWSER_MEDIA_CAPTURE_UTIL_H_
#define EXTENSIONS_SHELL_BROWSER_MEDIA_CAPTURE_UTIL_H_

#include "base/macros.h"
#include "content/public/common/media_stream_request.h"

namespace content {
class WebContents;
}

namespace extensions {

class Extension;

namespace media_capture_util {

// Grants access to audio and video capture devices.
// * If the caller requests specific device ids, grants access to those.
// * If the caller does not request specific ids, grants access to the first
//   available device.
// Usually used as a helper for media capture ProcessMediaAccessRequest().
void GrantMediaStreamRequest(content::WebContents* web_contents,
                             const content::MediaStreamRequest& request,
                             const content::MediaResponseCallback& callback,
                             const Extension* extension);

// Verifies that the extension has permission for |type|. If not, crash.
void VerifyMediaAccessPermission(content::MediaStreamType type,
                                 const Extension* extension);

}  // namespace media_capture_util
}  // namespace extensions

#endif  // EXTENSIONS_SHELL_BROWSER_MEDIA_CAPTURE_UTIL_H_
