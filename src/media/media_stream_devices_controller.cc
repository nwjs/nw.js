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

#include "content/nw/src/media/media_stream_devices_controller.h"

#include "base/logging.h"
#include "base/values.h"

namespace {

// A predicate that checks if a StreamDeviceInfo object has the same ID as the
// device ID specified at construction.
class DeviceIdEquals {
 public:
  explicit DeviceIdEquals(const std::string& device_id)
      : device_id_(device_id) {
  }

  bool operator() (const content::MediaStreamDevice& device) {
    return device.device_id == device_id_;
  }

 private:
  std::string device_id_;
};

// A predicate that checks if a StreamDeviceInfo object has the same device
// name as the device name specified at construction.
class DeviceNameEquals {
 public:
  explicit DeviceNameEquals(const std::string& device_name)
      : device_name_(device_name) {
  }

  bool operator() (const content::MediaStreamDevice& device) {
    return device.name == device_name_;
  }

 private:
  std::string device_name_;
};

// Whether |request| contains any device of given |type|.
bool HasDevice(const content::MediaStreamRequest& request,
               content::MediaStreamDeviceType type) {
  content::MediaStreamDeviceMap::const_iterator device_it =
      request.devices.find(type);
  return device_it != request.devices.end() && !device_it->second.empty();
}

const char kAudioKey[] = "audio";
const char kVideoKey[] = "video";

}  // namespace

MediaStreamDevicesController::MediaStreamDevicesController(
    const content::MediaStreamRequest* request,
    const content::MediaResponseCallback& callback)
    : request_(*request),
      callback_(callback) {
  has_audio_ =
      HasDevice(request_, content::MEDIA_STREAM_DEVICE_TYPE_AUDIO_CAPTURE);
  has_video_ =
      HasDevice(request_, content::MEDIA_STREAM_DEVICE_TYPE_VIDEO_CAPTURE);
}

MediaStreamDevicesController::~MediaStreamDevicesController() {}

bool MediaStreamDevicesController::DismissInfoBarAndTakeActionOnSettings() {
  // Deny the request and don't show the infobar if there is no devices.
  if (!has_audio_ && !has_video_) {
    // TODO(xians): We should detect this in a early state, and post a callback
    // to tell the users that no device is available. Remove the code and add
    // a DCHECK when this is done.
    Deny();
    return true;
  }

  std::string audio, video;
  GetAlwaysAllowedDevices(&audio, &video);
  Accept(audio, video, true);
  return true;
}

content::MediaStreamDevices
MediaStreamDevicesController::GetAudioDevices() const {
  if (!has_audio_)
    return content::MediaStreamDevices();

  content::MediaStreamDeviceMap::const_iterator it =
      request_.devices.find(content::MEDIA_STREAM_DEVICE_TYPE_AUDIO_CAPTURE);
  DCHECK(it != request_.devices.end());
  return it->second;
}

content::MediaStreamDevices
MediaStreamDevicesController::GetVideoDevices() const {
  if (!has_video_)
    return content::MediaStreamDevices();

  content::MediaStreamDeviceMap::const_iterator it =
      request_.devices.find(content::MEDIA_STREAM_DEVICE_TYPE_VIDEO_CAPTURE);
  DCHECK(it != request_.devices.end());
  return it->second;
}

void MediaStreamDevicesController::Accept(const std::string& audio_id,
                                          const std::string& video_id,
                                          bool always_allow) {
  content::MediaStreamDevices devices;
  std::string audio_device, video_device;
  if (has_audio_) {
    AddDeviceWithId(content::MEDIA_STREAM_DEVICE_TYPE_AUDIO_CAPTURE,
                    audio_id, &devices, &audio_device);
  }
  if (has_video_) {
    AddDeviceWithId(content::MEDIA_STREAM_DEVICE_TYPE_VIDEO_CAPTURE,
                    video_id, &devices, &video_device);
  }
  DCHECK(!devices.empty());

  callback_.Run(devices);
}

void MediaStreamDevicesController::Deny() {
  callback_.Run(content::MediaStreamDevices());
}

void MediaStreamDevicesController::AddDeviceWithId(
    content::MediaStreamDeviceType type,
    const std::string& id,
    content::MediaStreamDevices* devices,
    std::string* device_name) {
  DCHECK(devices);
  content::MediaStreamDeviceMap::const_iterator device_it =
      request_.devices.find(type);
  if (device_it == request_.devices.end())
    return;

  content::MediaStreamDevices::const_iterator it = std::find_if(
      device_it->second.begin(), device_it->second.end(), DeviceIdEquals(id));
  if (it == device_it->second.end())
    return;

  devices->push_back(*it);
  *device_name = it->name;
}

void MediaStreamDevicesController::GetAlwaysAllowedDevices(
    std::string* audio_id, std::string* video_id) {
  DCHECK(audio_id->empty());
  DCHECK(video_id->empty());
  if (has_audio_) {
    *audio_id =
        GetFirstDeviceId(content::MEDIA_STREAM_DEVICE_TYPE_AUDIO_CAPTURE);
  }
  if (has_video_) {
    *video_id =
        GetFirstDeviceId(content::MEDIA_STREAM_DEVICE_TYPE_VIDEO_CAPTURE);
  }
}

std::string MediaStreamDevicesController::GetDeviceIdByName(
    content::MediaStreamDeviceType type,
    const std::string& name) {
  content::MediaStreamDeviceMap::const_iterator device_it =
      request_.devices.find(type);
  if (device_it != request_.devices.end()) {
    content::MediaStreamDevices::const_iterator it = std::find_if(
        device_it->second.begin(), device_it->second.end(),
        DeviceNameEquals(name));
    if (it != device_it->second.end())
      return it->device_id;
  }

  // Device is not available, return an empty string.
  return std::string();
}

std::string MediaStreamDevicesController::GetFirstDeviceId(
    content::MediaStreamDeviceType type) {
  content::MediaStreamDeviceMap::const_iterator device_it =
      request_.devices.find(type);
  if (device_it != request_.devices.end())
    return device_it->second.begin()->device_id;

  return std::string();
}
