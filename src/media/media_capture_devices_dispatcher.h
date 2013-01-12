// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NW_MEDIA_MEDIA_CAPTURE_DEVICES_DISPATCHER_H_
#define NW_MEDIA_MEDIA_CAPTURE_DEVICES_DISPATCHER_H_

#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/observer_list.h"
#include "content/public/common/media_stream_request.h"

// This observer is owned by MediaInternals and deleted when MediaInternals
// is deleted.
class MediaCaptureDevicesDispatcher
    : public base::RefCountedThreadSafe<MediaCaptureDevicesDispatcher> {
 public:
  class Observer {
   public:
    // Handle an information update consisting of a up-to-date audio capture
    // device lists. This happens when a microphone is plugged in or unplugged.
    virtual void OnUpdateAudioDevices(
        const content::MediaStreamDevices& devices) {}

    // Handle an information update consisting of a up-to-date video capture
    // device lists. This happens when a camera is plugged in or unplugged.
    virtual void OnUpdateVideoDevices(
        const content::MediaStreamDevices& devices) {}

    virtual ~Observer() {}
  };

  MediaCaptureDevicesDispatcher();

  // Called on IO thread when one audio device is plugged in or unplugged.
  void AudioCaptureDevicesChanged(const content::MediaStreamDevices& devices);

  // Called on IO thread when one video device is plugged in or unplugged.
  void VideoCaptureDevicesChanged(const content::MediaStreamDevices& devices);

  // Methods for observers. Called on UI thread.
  // Observers should add themselves on construction and remove themselves
  // on destruction.
  void AddObserver(Observer* observer);
  void RemoveObserver(Observer* observer);
  const content::MediaStreamDevices& GetAudioCaptureDevices();
  const content::MediaStreamDevices& GetVideoCaptureDevices();

 private:
  friend class base::RefCountedThreadSafe<MediaCaptureDevicesDispatcher>;
  virtual ~MediaCaptureDevicesDispatcher();

  // Called by the public Audio/VideoCaptureDevicesChanged() functions,
  // executed on UI thread.
  void UpdateAudioDevicesOnUIThread(const content::MediaStreamDevices& devices);
  void UpdateVideoDevicesOnUIThread(const content::MediaStreamDevices& devices);

  // A list of cached audio capture devices.
  content::MediaStreamDevices audio_devices_;

  // A list of cached video capture devices.
  content::MediaStreamDevices video_devices_;

  // A list of observers for the device update notifications.
  ObserverList<Observer> observers_;

  // Flag to indicate if device enumeration has been done/doing.
  // Only accessed on UI thread.
  bool devices_enumerated_;
};

#endif  // CHROME_BROWSER_MEDIA_MEDIA_CAPTURE_DEVICES_DISPATCHER_H_
