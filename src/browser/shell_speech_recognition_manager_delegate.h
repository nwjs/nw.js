// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NW_SHELL_BROWSER_SHELL_SPEECH_RECOGNITION_MANAGER_DELEGATE_H_
#define NW_SHELL_BROWSER_SHELL_SPEECH_RECOGNITION_MANAGER_DELEGATE_H_

#include "content/public/browser/speech_recognition_event_listener.h"
#include "content/public/browser/speech_recognition_manager_delegate.h"

namespace content {
namespace speech {

class ShellSpeechRecognitionManagerDelegate
    : public content::SpeechRecognitionManagerDelegate,
      public content::SpeechRecognitionEventListener {
 public:
  ShellSpeechRecognitionManagerDelegate();
  ~ShellSpeechRecognitionManagerDelegate() override;

 private:
  // SpeechRecognitionEventListener methods.
  void OnRecognitionStart(int session_id) override;
  void OnAudioStart(int session_id) override;
  void OnEnvironmentEstimationComplete(int session_id) override;
  void OnSoundStart(int session_id) override;
  void OnSoundEnd(int session_id) override;
  void OnAudioEnd(int session_id) override;
  void OnRecognitionEnd(int session_id) override;
  void OnRecognitionResults(
      int session_id,
      const content::SpeechRecognitionResults& result) override;
  void OnRecognitionError(
      int session_id,
      const content::SpeechRecognitionError& error) override;
  void OnAudioLevelsChange(int session_id,
                           float volume,
                           float noise_volume) override;

  // SpeechRecognitionManagerDelegate methods.
  void GetDiagnosticInformation(bool* can_report_metrics,
                                std::string* hardware_info) override;
  void CheckRecognitionIsAllowed(
      int session_id,
      base::Callback<void(bool ask_user, bool is_allowed)> callback) override;
  content::SpeechRecognitionEventListener* GetEventListener() override;
  bool FilterProfanities(int render_process_id) override;

  static void CheckRenderViewType(
      base::Callback<void(bool ask_user, bool is_allowed)> callback,
      int render_process_id,
      int render_view_id);

  DISALLOW_COPY_AND_ASSIGN(ShellSpeechRecognitionManagerDelegate);
};

}  // namespace speech
}  // namespace extensions

#endif  // NW_SHELL_BROWSER_SHELL_CONTENT_BROWSER_CLIENT_H_
