// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/media/media_stream_devices_controller.h"

#include "base/command_line.h"
#include "base/values.h"
#include "chrome/common/chrome_switches.h"
#include "content/nw/src/media/media_capture_devices_dispatcher.h"
#include "content/nw/src/media/media_internals.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/desktop_media_id.h"
#include "content/public/common/media_stream_request.h"

#include "third_party/webrtc/modules/desktop_capture/desktop_capture_types.h"

using content::BrowserThread;

namespace {

bool HasAnyAvailableDevice() {
  MediaCaptureDevicesDispatcher* dispatcher =
      MediaInternals::GetInstance()->GetMediaCaptureDevicesDispatcher();
  const content::MediaStreamDevices& audio_devices =
      dispatcher->GetAudioCaptureDevices();
  const content::MediaStreamDevices& video_devices =
      dispatcher->GetVideoCaptureDevices();

  return !audio_devices.empty() || !video_devices.empty();
};

const char kAudioKey[] = "audio";
const char kVideoKey[] = "video";

}  // namespace

MediaStreamDevicesController::MediaStreamDevicesController(
    const content::MediaStreamRequest& request,
    const content::MediaResponseCallback& callback)
    :
      request_(request),
      callback_(callback),
      has_audio_(content::IsAudioMediaType(request.audio_type) &&
                 !IsAudioDeviceBlockedByPolicy()),
      has_video_(content::IsVideoMediaType(request.video_type) &&
                 !IsVideoDeviceBlockedByPolicy()) {
}

MediaStreamDevicesController::~MediaStreamDevicesController() {}

#if 0
// static
void MediaStreamDevicesController::RegisterUserPrefs(
    PrefServiceSyncable* prefs) {
  prefs->RegisterBooleanPref(prefs::kVideoCaptureAllowed,
                             true,
                             PrefServiceSyncable::UNSYNCABLE_PREF);
  prefs->RegisterBooleanPref(prefs::kAudioCaptureAllowed,
                             true,
                             PrefServiceSyncable::UNSYNCABLE_PREF);
}
#endif

bool MediaStreamDevicesController::DismissInfoBarAndTakeActionOnSettings() {
  // If this is a no UI check for policies only go straight to accept - policy
  // check will be done automatically on the way.
  if (request_.request_type == content::MEDIA_OPEN_DEVICE) {
    Accept(false);
    return true;
  }

  if (request_.audio_type == content::MEDIA_TAB_AUDIO_CAPTURE ||
      request_.video_type == content::MEDIA_TAB_VIDEO_CAPTURE ||
      request_.video_type == content::MEDIA_DESKTOP_VIDEO_CAPTURE) {
    HandleTapMediaRequest();
    return true;
  }

#if 0
  // Deny the request if the security origin is empty, this happens with
  // file access without |--allow-file-access-from-files| flag.
  if (request_.security_origin.is_empty()) {
    Deny(false);
    return true;
  }
#endif
  // Deny the request if there is no device attached to the OS.
  if (!HasAnyAvailableDevice()) {
    Deny(false);
    return true;
  }

  // Check if any allow exception has been made for this request.
  if (IsRequestAllowedByDefault()) {
    Accept(false);
    return true;
  }
#if 0
  // Check if any block exception has been made for this request.
  if (IsRequestBlockedByDefault()) {
    Deny(false);
    return true;
  }

  // Check if the media default setting is set to block.
  if (IsDefaultMediaAccessBlocked()) {
    Deny(false);
    return true;
  }
#endif
  // Don't show the infobar.
  return true;
}

const std::string& MediaStreamDevicesController::GetSecurityOriginSpec() const {
  return request_.security_origin.spec();
}

void MediaStreamDevicesController::Accept(bool update_content_setting) {
  // Get the default devices for the request.
  content::MediaStreamDevices devices;
  MediaCaptureDevicesDispatcher* dispatcher =
      MediaInternals::GetInstance()->GetMediaCaptureDevicesDispatcher();
  switch (request_.request_type) {
  case content::MEDIA_OPEN_DEVICE: {
    const content::MediaStreamDevice* device = NULL;
    // For open device request pick the desired device or fall back to the
    // first available of the given type.
    if (request_.audio_type == content::MEDIA_DEVICE_AUDIO_CAPTURE) {
      device = dispatcher->
        GetRequestedAudioDevice(request_.requested_audio_device_id);
      // TODO(wjia): Confirm this is the intended behavior.
      if (!device) {
        device = dispatcher->GetFirstAvailableAudioDevice();
      }
    } else if (request_.video_type == content::MEDIA_DEVICE_VIDEO_CAPTURE) {
      // Pepper API opens only one device at a time.
      device = dispatcher->GetRequestedVideoDevice(request_.requested_video_device_id);
      // TODO(wjia): Confirm this is the intended behavior.
      if (!device) {
        device = dispatcher->GetFirstAvailableVideoDevice();
      }
    }
    if (device)
      devices.push_back(*device);
    break;
  } case content::MEDIA_GENERATE_STREAM: {
      bool needs_audio_device = has_audio_;
      bool needs_video_device = has_video_;

      // Get the exact audio or video device if an id is specified.
      if (!request_.requested_audio_device_id.empty()) {
        const content::MediaStreamDevice* audio_device =
          dispatcher->GetRequestedAudioDevice(request_.requested_audio_device_id);
        if (audio_device) {
          devices.push_back(*audio_device);
          needs_audio_device = false;
        }
      }
      if (!request_.requested_video_device_id.empty()) {
        const content::MediaStreamDevice* video_device =
          dispatcher->GetRequestedVideoDevice(request_.requested_video_device_id);
        if (video_device) {
          devices.push_back(*video_device);
          needs_video_device = false;
        }
      }

      // If either or both audio and video devices were requested but not
      // specified by id, get the default devices.
      if (needs_audio_device || needs_video_device) {
        media::GetDefaultDevicesForProfile(
                                      needs_audio_device,
                                      needs_video_device,
                                      &devices);
      }
      break;
    } case content::MEDIA_DEVICE_ACCESS:
    // Get the default devices for the request.
    media::GetDefaultDevicesForProfile(
                                  has_audio_,
                                  has_video_,
                                  &devices);
    break;
  case content::MEDIA_ENUMERATE_DEVICES:
    // Do nothing.
    NOTREACHED();
    break;
  }

  callback_.Run(devices,
                devices.empty() ?
                content::MEDIA_DEVICE_NO_HARDWARE : content::MEDIA_DEVICE_OK,
                scoped_ptr<content::MediaStreamUI>());
}

void MediaStreamDevicesController::Deny(bool update_content_setting) {
  callback_.Run(content::MediaStreamDevices(), content::MEDIA_DEVICE_NO_HARDWARE, scoped_ptr<content::MediaStreamUI>());
}

bool MediaStreamDevicesController::IsAudioDeviceBlockedByPolicy() const {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  return false;
}

bool MediaStreamDevicesController::IsVideoDeviceBlockedByPolicy() const {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  return false;
}

bool MediaStreamDevicesController::IsRequestAllowedByDefault() const {
  return true;
}

bool MediaStreamDevicesController::IsRequestBlockedByDefault() const {
  return false;
}

bool MediaStreamDevicesController::IsDefaultMediaAccessBlocked() const {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  return false;
}

void MediaStreamDevicesController::HandleTapMediaRequest() {
  content::MediaStreamDevices devices;

  if (request_.audio_type == content::MEDIA_TAB_AUDIO_CAPTURE) {
    devices.push_back(content::MediaStreamDevice(
          content::MEDIA_TAB_VIDEO_CAPTURE, "", ""));
  }
  if (request_.video_type == content::MEDIA_TAB_VIDEO_CAPTURE) {
    devices.push_back(content::MediaStreamDevice(
          content::MEDIA_TAB_AUDIO_CAPTURE, "", ""));
  }
  if (request_.video_type == content::MEDIA_DESKTOP_VIDEO_CAPTURE) {
    const bool screen_capture_enabled =
      CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableUserMediaScreenCapturing);

    if (screen_capture_enabled) {
      content::DesktopMediaID media_id;
      // If the device id wasn't specified then this is a screen capture request
      // (i.e. chooseDesktopMedia() API wasn't used to generate device id).
      if (request_.requested_video_device_id.empty()) {
        media_id =
            content::DesktopMediaID(content::DesktopMediaID::TYPE_SCREEN,
                                    webrtc::kFullDesktopScreenId);
      } else {
        media_id =
            content::DesktopMediaID::Parse(request_.requested_video_device_id);
      }

      devices.push_back(content::MediaStreamDevice(
            content::MEDIA_DESKTOP_VIDEO_CAPTURE, media_id.ToString(), "Screen"));
    }
  }

  callback_.Run(devices,
                devices.empty() ?
                     content::MEDIA_DEVICE_NO_HARDWARE : content::MEDIA_DEVICE_OK,
                scoped_ptr<content::MediaStreamUI>());
}

bool MediaStreamDevicesController::IsSchemeSecure() const {
  return (request_.security_origin.SchemeIsSecure());
}

bool MediaStreamDevicesController::ShouldAlwaysAllowOrigin() const {
  return true;
}

