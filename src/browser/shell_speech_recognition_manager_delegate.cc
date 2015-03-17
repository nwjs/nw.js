// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/shell_speech_recognition_manager_delegate.h"

#include "base/bind.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/speech_recognition_manager.h"
#include "content/public/browser/speech_recognition_session_context.h"
#include "content/public/browser/web_contents.h"

using content::BrowserThread;
using content::SpeechRecognitionManager;
using content::WebContents;

namespace content {
namespace speech {

ShellSpeechRecognitionManagerDelegate::ShellSpeechRecognitionManagerDelegate() {
}

ShellSpeechRecognitionManagerDelegate::
~ShellSpeechRecognitionManagerDelegate() {
}

void ShellSpeechRecognitionManagerDelegate::OnRecognitionStart(int session_id) {
}

void ShellSpeechRecognitionManagerDelegate::OnAudioStart(int session_id) {
}

void ShellSpeechRecognitionManagerDelegate::OnEnvironmentEstimationComplete(
    int session_id) {
}

void ShellSpeechRecognitionManagerDelegate::OnSoundStart(int session_id) {
}

void ShellSpeechRecognitionManagerDelegate::OnSoundEnd(int session_id) {
}

void ShellSpeechRecognitionManagerDelegate::OnAudioEnd(int session_id) {
}

void ShellSpeechRecognitionManagerDelegate::OnRecognitionEnd(int session_id) {
}

void ShellSpeechRecognitionManagerDelegate::OnRecognitionResults(
    int session_id,
    const content::SpeechRecognitionResults& result) {
}

void ShellSpeechRecognitionManagerDelegate::OnRecognitionError(
    int session_id,
    const content::SpeechRecognitionError& error) {
}

void ShellSpeechRecognitionManagerDelegate::OnAudioLevelsChange(
    int session_id,
    float volume,
    float noise_volume) {
}

void ShellSpeechRecognitionManagerDelegate::GetDiagnosticInformation(
    bool* can_report_metrics,
    std::string* hardware_info) {
}

void ShellSpeechRecognitionManagerDelegate::CheckRecognitionIsAllowed(
    int session_id,
    base::Callback<void(bool ask_user, bool is_allowed)> callback) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));

  const content::SpeechRecognitionSessionContext& context =
      SpeechRecognitionManager::GetInstance()->GetSessionContext(session_id);

  // Make sure that initiators (extensions/web pages) properly set the
  // |render_process_id| field, which is needed later to retrieve the profile.
  DCHECK_NE(context.render_process_id, 0);

  BrowserThread::PostTask(BrowserThread::UI, FROM_HERE,
                          base::Bind(&CheckRenderViewType,
                                     callback,
                                     context.render_process_id,
                                     context.render_view_id));
}

content::SpeechRecognitionEventListener*
ShellSpeechRecognitionManagerDelegate::GetEventListener() {
  return this;
}

bool ShellSpeechRecognitionManagerDelegate::FilterProfanities(
    int render_process_id) {
  // TODO(zork): Determine where this preference should come from.
  return true;
}

// static
void ShellSpeechRecognitionManagerDelegate::CheckRenderViewType(
    base::Callback<void(bool ask_user, bool is_allowed)> callback,
    int render_process_id,
    int render_view_id) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  bool allowed = true;
  bool check_permission = false;

  BrowserThread::PostTask(BrowserThread::IO, FROM_HERE,
                          base::Bind(callback, check_permission, allowed));
}

}  // namespace speech
}  // namespace extensions
