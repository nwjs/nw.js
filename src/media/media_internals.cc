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

#include "content/nw/src/media/media_internals.h"

#include "base/memory/scoped_ptr.h"
#include "base/strings/string16.h"
#include "base/stringprintf.h"
#include "content/nw/src/media/media_capture_devices_dispatcher.h"
#include "content/public/browser/browser_thread.h"
#include "media/base/media_log.h"
#include "media/base/media_log_event.h"

using content::BrowserThread;

namespace media {

namespace {

const content::MediaStreamDevice* FindDefaultDeviceWithId(
    const content::MediaStreamDevices& devices,
    const std::string& device_id) {
  if (devices.empty())
    return NULL;

  content::MediaStreamDevices::const_iterator iter = devices.begin();
  for (; iter != devices.end(); ++iter) {
    if (iter->id == device_id) {
      return &(*iter);
    }
  }

  return &(*devices.begin());
};

}  // namespace

void GetDefaultDevicesForProfile(bool audio,
                                 bool video,
                                 content::MediaStreamDevices* devices) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(audio || video);

  MediaCaptureDevicesDispatcher* dispatcher =
      MediaInternals::GetInstance()->GetMediaCaptureDevicesDispatcher();
  if (audio) {
    std::string default_device;
    const content::MediaStreamDevices& audio_devices =
        dispatcher->GetAudioCaptureDevices();
    const content::MediaStreamDevice* const device =
        FindDefaultDeviceWithId(audio_devices, default_device);
    if (device)
      devices->push_back(*device);
  }

  if (video) {
    std::string default_device;
    const content::MediaStreamDevices& video_devices =
           dispatcher->GetVideoCaptureDevices();
    const content::MediaStreamDevice* const device =
        FindDefaultDeviceWithId(video_devices, default_device);
    if (device)
      devices->push_back(*device);
  }
}

} // namespace media

MediaInternals* MediaInternals::GetInstance() {
  return Singleton<MediaInternals>::get();
}

MediaInternals::~MediaInternals() {}

void MediaInternals::OnAudioCaptureDevicesChanged(
    const content::MediaStreamDevices& devices) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  media_devices_dispatcher_->AudioCaptureDevicesChanged(devices);
}

void MediaInternals::OnVideoCaptureDevicesChanged(
    const content::MediaStreamDevices& devices) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  media_devices_dispatcher_->VideoCaptureDevicesChanged(devices);
}

void MediaInternals::OnMediaRequestStateChanged(
    int render_process_id,
    int render_view_id,
    const content::MediaStreamDevice& device,
    content::MediaRequestState state) {
}

void MediaInternals::OnAudioStreamPlayingChanged(
    int render_process_id, int render_view_id, int stream_id, bool playing) {
}

scoped_refptr<MediaCaptureDevicesDispatcher>
MediaInternals::GetMediaCaptureDevicesDispatcher() {
  return media_devices_dispatcher_;
}

MediaInternals::MediaInternals()
    : media_devices_dispatcher_(new MediaCaptureDevicesDispatcher()) {
}
