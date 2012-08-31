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

#ifndef CONTENT_NW_SRC_MEDIA_MEDIA_STREAM_DEVICES_CONTROLLER_H_
#define CONTENT_NW_SRC_MEDIA_MEDIA_STREAM_DEVICES_CONTROLLER_H_

#include <string>

#include "content/public/browser/web_contents_delegate.h"

class GURL;

class MediaStreamDevicesController {
 public:
  // TODO(xians): Use const content::MediaStreamRequest& instead of *.
  MediaStreamDevicesController(const content::MediaStreamRequest* request,
                               const content::MediaResponseCallback& callback);

  virtual ~MediaStreamDevicesController();

  // Public method to be called before creating the MediaStreamInfoBarDelegate.
  // This function will check the content settings exceptions and take the
  // corresponding action on exception which matches the request.
  bool DismissInfoBarAndTakeActionOnSettings();

  // Public methods to be called by MediaStreamInfoBarDelegate;
  bool has_audio() const { return has_audio_; }
  bool has_video() const { return has_video_; }
  content::MediaStreamDevices GetAudioDevices() const;
  content::MediaStreamDevices GetVideoDevices() const;
  const GURL& GetSecurityOrigin() const;
  void Accept(const std::string& audio_id,
              const std::string& video_id,
              bool always_allow);
  void Deny();

 private:
  // Finds a device in the current request with the specified |id| and |type|,
  // adds it to the |devices| array and also return the name of the device.
  void AddDeviceWithId(content::MediaStreamDeviceType type,
                       const std::string& id,
                       content::MediaStreamDevices* devices,
                       std::string* device_name);

  // Gets the respective "always allowed" devices for the origin in |request_|.
  // |audio_id| and |video_id| will be empty if there is no "always allowed"
  // device for the origin, or any of the devices is not listed on the devices
  // list in |request_|.
  void GetAlwaysAllowedDevices(std::string* audio_id,
                               std::string* video_id);

  std::string GetDeviceIdByName(content::MediaStreamDeviceType type,
                                const std::string& name);

  std::string GetFirstDeviceId(content::MediaStreamDeviceType type);

  bool has_audio_;
  bool has_video_;

  // The original request for access to devices.
  const content::MediaStreamRequest request_;

  // The callback that needs to be Run to notify WebRTC of whether access to
  // audio/video devices was granted or not.
  content::MediaResponseCallback callback_;
  DISALLOW_COPY_AND_ASSIGN(MediaStreamDevicesController);
};

#endif  // CONTENT_NW_SRC_MEDIA_MEDIA_STREAM_DEVICES_CONTROLLER_H_
