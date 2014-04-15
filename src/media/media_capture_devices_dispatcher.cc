// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/memory/singleton.h"

#include "content/nw/src/media/media_capture_devices_dispatcher.h"

#include "content/public/browser/browser_thread.h"
#include "content/public/browser/media_devices_monitor.h"
#include "content/public/common/media_stream_request.h"

using content::BrowserThread;
using content::MediaStreamDevices;

namespace {

// Finds a device in |devices| that has |device_id|, or NULL if not found.
const content::MediaStreamDevice* FindDeviceWithId(
    const content::MediaStreamDevices& devices,
    const std::string& device_id) {
  content::MediaStreamDevices::const_iterator iter = devices.begin();
  for (; iter != devices.end(); ++iter) {
    if (iter->id == device_id) {
      return &(*iter);
    }
  }
  return NULL;
};

}

MediaCaptureDevicesDispatcher::MediaCaptureDevicesDispatcher()
    : devices_enumerated_(false) {}

MediaCaptureDevicesDispatcher::~MediaCaptureDevicesDispatcher() {}

void MediaCaptureDevicesDispatcher::AudioCaptureDevicesChanged(
    const MediaStreamDevices& devices) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));

  BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&MediaCaptureDevicesDispatcher::UpdateAudioDevicesOnUIThread,
                 this, devices));
}

void MediaCaptureDevicesDispatcher::VideoCaptureDevicesChanged(
    const MediaStreamDevices& devices) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));

  BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&MediaCaptureDevicesDispatcher::UpdateVideoDevicesOnUIThread,
                 this, devices));
}

void MediaCaptureDevicesDispatcher::AddObserver(Observer* observer) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  if (!observers_.HasObserver(observer))
    observers_.AddObserver(observer);
}

void MediaCaptureDevicesDispatcher::RemoveObserver(Observer* observer) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  observers_.RemoveObserver(observer);
}

const MediaStreamDevices&
MediaCaptureDevicesDispatcher::GetAudioCaptureDevices() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  if (!devices_enumerated_) {
    BrowserThread::PostTask(
        BrowserThread::IO, FROM_HERE,
        base::Bind(&content::EnsureMonitorCaptureDevices));
    devices_enumerated_ = true;
  }
  return audio_devices_;
}

const MediaStreamDevices&
MediaCaptureDevicesDispatcher::GetVideoCaptureDevices() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  if (!devices_enumerated_) {
    BrowserThread::PostTask(
        BrowserThread::IO, FROM_HERE,
        base::Bind(&content::EnsureMonitorCaptureDevices));
    devices_enumerated_ = true;
  }
  return video_devices_;
}

void MediaCaptureDevicesDispatcher::OnCreatingAudioStream(
    int render_process_id,
    int render_view_id) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(
          &MediaCaptureDevicesDispatcher::OnCreatingAudioStreamOnUIThread,
          base::Unretained(this), render_process_id, render_view_id));
}

void MediaCaptureDevicesDispatcher::UpdateAudioDevicesOnUIThread(
    const content::MediaStreamDevices& devices) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  devices_enumerated_ = true;
  audio_devices_ = devices;
  FOR_EACH_OBSERVER(Observer, observers_,
                    OnUpdateAudioDevices(audio_devices_));
}

void MediaCaptureDevicesDispatcher::UpdateVideoDevicesOnUIThread(
    const content::MediaStreamDevices& devices){
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  devices_enumerated_ = true;
  video_devices_ = devices;
  FOR_EACH_OBSERVER(Observer, observers_,
                    OnUpdateVideoDevices(video_devices_));
}

void MediaCaptureDevicesDispatcher::OnCreatingAudioStreamOnUIThread(
    int render_process_id,
    int render_view_id) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  FOR_EACH_OBSERVER(Observer, observers_,
                    OnCreatingAudioStream(render_process_id, render_view_id));
}

const content::MediaStreamDevice*
MediaCaptureDevicesDispatcher::GetRequestedAudioDevice(
    const std::string& requested_audio_device_id) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  const content::MediaStreamDevices& audio_devices = GetAudioCaptureDevices();
  const content::MediaStreamDevice* const device =
      FindDeviceWithId(audio_devices, requested_audio_device_id);
  return device;
}

const content::MediaStreamDevice*
MediaCaptureDevicesDispatcher::GetFirstAvailableAudioDevice() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  const content::MediaStreamDevices& audio_devices = GetAudioCaptureDevices();
  if (audio_devices.empty())
    return NULL;
  return &(*audio_devices.begin());
}

const content::MediaStreamDevice*
MediaCaptureDevicesDispatcher::GetRequestedVideoDevice(
    const std::string& requested_video_device_id) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  const content::MediaStreamDevices& video_devices = GetVideoCaptureDevices();
  const content::MediaStreamDevice* const device =
      FindDeviceWithId(video_devices, requested_video_device_id);
  return device;
}

const content::MediaStreamDevice*
MediaCaptureDevicesDispatcher::GetFirstAvailableVideoDevice() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  const content::MediaStreamDevices& video_devices = GetVideoCaptureDevices();
  if (video_devices.empty())
    return NULL;
  return &(*video_devices.begin());
}

