// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "extensions/shell/browser/media_capture_util.h"

#include <string>

#include "base/callback.h"
#include "base/logging.h"
#include "content/public/browser/media_capture_devices.h"
#include "extensions/common/permissions/permissions_data.h"

using content::MediaCaptureDevices;
using content::MediaStreamDevice;
using content::MediaStreamDevices;
using content::MediaStreamUI;

namespace extensions {

const MediaStreamDevice* GetRequestedDeviceOrDefault(
    const MediaStreamDevices& devices,
    const std::string& requested_device_id) {
  if (!requested_device_id.empty())
    return devices.FindById(requested_device_id);

  if (!devices.empty())
    return &devices[0];

  return NULL;
}

namespace media_capture_util {

// See also Chrome's MediaCaptureDevicesDispatcher.
void GrantMediaStreamRequest(content::WebContents* web_contents,
                             const content::MediaStreamRequest& request,
                             const content::MediaResponseCallback& callback,
                             const Extension* extension) {
  // app_shell only supports audio and video capture, not tab or screen capture.
  DCHECK(request.audio_type == content::MEDIA_DEVICE_AUDIO_CAPTURE ||
         request.video_type == content::MEDIA_DEVICE_VIDEO_CAPTURE);

  MediaStreamDevices devices;

  if (request.audio_type == content::MEDIA_DEVICE_AUDIO_CAPTURE) {
    VerifyMediaAccessPermission(request.audio_type, extension);
    const MediaStreamDevice* device = GetRequestedDeviceOrDefault(
        MediaCaptureDevices::GetInstance()->GetAudioCaptureDevices(),
        request.requested_audio_device_id);
    if (device)
      devices.push_back(*device);
  }

  if (request.video_type == content::MEDIA_DEVICE_VIDEO_CAPTURE) {
    VerifyMediaAccessPermission(request.video_type, extension);
    const MediaStreamDevice* device = GetRequestedDeviceOrDefault(
        MediaCaptureDevices::GetInstance()->GetVideoCaptureDevices(),
        request.requested_video_device_id);
    if (device)
      devices.push_back(*device);
  }

  // TODO(jamescook): Should we show a recording icon somewhere? If so, where?
  scoped_ptr<MediaStreamUI> ui;
  callback.Run(devices,
               devices.empty() ? content::MEDIA_DEVICE_INVALID_STATE
                               : content::MEDIA_DEVICE_OK,
               ui.Pass());
}

void VerifyMediaAccessPermission(content::MediaStreamType type,
                                 const Extension* extension) {
  const PermissionsData* permissions_data = extension->permissions_data();
  if (type == content::MEDIA_DEVICE_AUDIO_CAPTURE) {
    // app_shell has no UI surface to show an error, and on an embedded device
    // it's better to crash than to have a feature not work.
    CHECK(permissions_data->HasAPIPermission(APIPermission::kAudioCapture))
        << "Audio capture request but no audioCapture permission in manifest.";
  } else {
    DCHECK(type == content::MEDIA_DEVICE_VIDEO_CAPTURE);
    CHECK(permissions_data->HasAPIPermission(APIPermission::kVideoCapture))
        << "Video capture request but no videoCapture permission in manifest.";
  }
}

}  // namespace media_capture_util
}  // namespace extensions
