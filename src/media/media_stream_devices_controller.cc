// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/media/media_stream_devices_controller.h"

#include "base/values.h"
#include "content/nw/src/media/media_capture_devices_dispatcher.h"
#include "content/nw/src/media/media_internals.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/common/media_stream_request.h"

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
      request_.video_type == content::MEDIA_TAB_VIDEO_CAPTURE) {
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
  media::GetDefaultDevicesForProfile(
                                     has_audio_,
                                     has_video_,
                                     &devices);

  callback_.Run(devices, scoped_ptr<content::MediaStreamUI>());
}

void MediaStreamDevicesController::Deny(bool update_content_setting) {
  callback_.Run(content::MediaStreamDevices(), scoped_ptr<content::MediaStreamUI>());
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

  callback_.Run(devices, scoped_ptr<content::MediaStreamUI>());
}

bool MediaStreamDevicesController::IsSchemeSecure() const {
  return (request_.security_origin.SchemeIsSecure());
}

bool MediaStreamDevicesController::ShouldAlwaysAllowOrigin() const {
  return true;
}

