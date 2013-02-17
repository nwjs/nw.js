// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NW_MEDIA_MEDIA_INTERNALS_H_
#define NW_MEDIA_MEDIA_INTERNALS_H_

#include <string>

#include "base/memory/ref_counted.h"
#include "base/memory/singleton.h"
#include "base/observer_list.h"
#include "base/values.h"
#include "content/public/browser/media_observer.h"
#include "content/public/common/media_stream_request.h"

class MediaCaptureDevicesDispatcher;
class MediaInternalsObserver;
class MediaStreamCaptureIndicator;

namespace media {

struct MediaLogEvent;

// Helper to get the default devices which can be used by the media request,
// if the return list is empty, it means there is no available device on the OS.
// Called on the UI thread.
void GetDefaultDevicesForProfile(
                                 bool audio,
                                 bool video,
                                 content::MediaStreamDevices* devices);

}

// This class stores information about currently active media.
// It's constructed on the UI thread but all of its methods are called on the IO
// thread.
class MediaInternals : public content::MediaObserver {
 public:
  virtual ~MediaInternals();

  static MediaInternals* GetInstance();

  // Overridden from content::MediaObserver:
  virtual void OnDeleteAudioStream(void* host, int stream_id) OVERRIDE;
  virtual void OnSetAudioStreamPlaying(void* host,
                                       int stream_id,
                                       bool playing) OVERRIDE;
  virtual void OnSetAudioStreamStatus(void* host,
                                      int stream_id,
                                      const std::string& status) OVERRIDE;
  virtual void OnSetAudioStreamVolume(void* host,
                                      int stream_id,
                                      double volume) OVERRIDE;
  virtual void OnMediaEvent(int render_process_id,
                            const media::MediaLogEvent& event) OVERRIDE;
  virtual void OnCaptureDevicesOpened(
      int render_process_id,
      int render_view_id,
      const content::MediaStreamDevices& devices) OVERRIDE;
  virtual void OnCaptureDevicesClosed(
      int render_process_id,
      int render_view_id,
      const content::MediaStreamDevices& devices) OVERRIDE;
  virtual void OnAudioCaptureDevicesChanged(
      const content::MediaStreamDevices& devices) OVERRIDE;
  virtual void OnVideoCaptureDevicesChanged(
      const content::MediaStreamDevices& devices) OVERRIDE;
  virtual void OnMediaRequestStateChanged(
      int render_process_id,
      int render_view_id,
      const content::MediaStreamDevice& device,
      content::MediaRequestState state) OVERRIDE;

  // Methods for observers.
  // Observers should add themselves on construction and remove themselves
  // on destruction.
  void AddObserver(MediaInternalsObserver* observer);
  void RemoveObserver(MediaInternalsObserver* observer);
  void SendEverything();

  scoped_refptr<MediaStreamCaptureIndicator> GetMediaStreamCaptureIndicator();
  scoped_refptr<MediaCaptureDevicesDispatcher>
      GetMediaCaptureDevicesDispatcher();

 private:
  friend class MediaInternalsTest;
  friend struct DefaultSingletonTraits<MediaInternals>;

  MediaInternals();

  // Sets |property| of an audio stream to |value| and notifies observers.
  // (host, stream_id) is a unique id for the audio stream.
  // |host| will never be dereferenced.
  void UpdateAudioStream(void* host, int stream_id,
                         const std::string& property, Value* value);

  // Removes |item| from |data_|.
  void DeleteItem(const std::string& item);

  // Sets data_.id.property = value and notifies attached UIs using update_fn.
  // id may be any depth, e.g. "video.decoders.1.2.3"
  void UpdateItem(const std::string& update_fn, const std::string& id,
                  const std::string& property, Value* value);

  // Calls javascript |function|(|value|) on each attached UI.
  void SendUpdate(const std::string& function, Value* value);

  DictionaryValue data_;
  ObserverList<MediaInternalsObserver> observers_;
  scoped_refptr<MediaCaptureDevicesDispatcher> media_devices_dispatcher_;

  DISALLOW_COPY_AND_ASSIGN(MediaInternals);
};

#endif  // CHROME_BROWSER_MEDIA_MEDIA_INTERNALS_H_
