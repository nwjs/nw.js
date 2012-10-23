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

#ifndef CONTENT_NW_SRC_MEDIA_MEDIA_INTERNALS_H_
#define CONTENT_NW_SRC_MEDIA_MEDIA_INTERNALS_H_

#include "base/memory/ref_counted.h"
#include "base/memory/singleton.h"
#include "base/observer_list.h"
#include "base/values.h"
#include "content/public/browser/media_observer.h"

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
  virtual void OnMediaRequestStateChanged(
      int render_process_id,
      int render_view_id,
      const content::MediaStreamDevice& device,
      content::MediaRequestState state) OVERRIDE;

 private:
  friend struct DefaultSingletonTraits<MediaInternals>;

  MediaInternals();

  DISALLOW_COPY_AND_ASSIGN(MediaInternals);
};

#endif  // CONTENT_NW_SRC_MEDIA_MEDIA_INTERNALS_H_
