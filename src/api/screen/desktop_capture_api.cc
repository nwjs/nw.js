// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/api/screen/desktop_capture_api.h"

#include "base/command_line.h"
#include "base/compiler_specific.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/media/native_desktop_media_list.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "net/base/net_util.h"
#include "third_party/webrtc/modules/desktop_capture/desktop_capture_options.h"
#include "third_party/webrtc/modules/desktop_capture/screen_capturer.h"
#include "third_party/webrtc/modules/desktop_capture/window_capturer.h"

namespace nwapi {

namespace {

const char kEmptySourcesListError[] =
    "At least one source type must be specified.";

DesktopCaptureChooseDesktopMediaFunction::PickerFactory* g_picker_factory =
    NULL;

}  // namespace

// static
void DesktopCaptureChooseDesktopMediaFunction::SetPickerFactoryForTests(
    PickerFactory* factory) {
  g_picker_factory = factory;
}

DesktopCaptureChooseDesktopMediaFunction::
    DesktopCaptureChooseDesktopMediaFunction() {
}

DesktopCaptureChooseDesktopMediaFunction::
    ~DesktopCaptureChooseDesktopMediaFunction() {
  // RenderViewHost may be already destroyed.
  if (render_view_host()) {
    DesktopCaptureRequestsRegistry::GetInstance()->RemoveRequest(
        render_view_host()->GetProcess()->GetID(), request_id_);
  }
}

void DesktopCaptureChooseDesktopMediaFunction::Cancel() {
  // Keep reference to |this| to ensure the object doesn't get destroyed before
  // we return.
  scoped_refptr<DesktopCaptureChooseDesktopMediaFunction> self(this);
  if (picker_) {
    picker_.reset();
    SetResult(new base::StringValue(std::string()));
    SendResponse(true);
  }
}

bool DesktopCaptureChooseDesktopMediaFunction::RunSync() {

  DesktopCaptureRequestsRegistry::GetInstance()->AddRequest(
      render_view_host()->GetProcess()->GetID(), request_id_, this);

  // |web_contents| is the WebContents for which the stream is created, and will
  // also be used to determine where to show the picker's UI.
  content::WebContents* web_contents = content::WebContents::FromRenderViewHost(render_view_host());
  DCHECK(web_contents);
  base::string16 target_name;

  // Register to be notified when the tab is closed.
  Observe(web_contents);

  bool show_screens = false;
  bool show_windows = false;
  
  const base::ListValue* capture_param = NULL;
  if(args_->GetList(1,&capture_param)) {
    for(base::ListValue::const_iterator i = capture_param->begin(); i != capture_param->end(); i++) {
      const base::Value* val = static_cast<const base::Value*>(*i);
      std::string str;
      if(val->GetAsString(&str)) {
        if(!str.compare("window")) {
          show_windows = true;
          continue;
        }
        
        if(!str.compare("screen")) {
          show_screens = true;
          continue;
        }
      }
    }
  }

  if (!show_screens && !show_windows) {
    error_ = kEmptySourcesListError;
    return false;
  }

  const gfx::NativeWindow parent_window =
      web_contents->GetTopLevelNativeWindow();
  scoped_ptr<DesktopMediaList> media_list;
  if (g_picker_factory) {
    media_list = g_picker_factory->CreateModel(
        show_screens, show_windows);
    picker_ = g_picker_factory->CreatePicker();
  } else {
    {
      webrtc::DesktopCaptureOptions options =
          webrtc::DesktopCaptureOptions::CreateDefault();
      options.set_disable_effects(false);
      scoped_ptr<webrtc::ScreenCapturer> screen_capturer(
          show_screens ? webrtc::ScreenCapturer::Create(options) : NULL);
      scoped_ptr<webrtc::WindowCapturer> window_capturer(
          show_windows ? webrtc::WindowCapturer::Create(options) : NULL);

      media_list.reset(new NativeDesktopMediaList(
          screen_capturer.Pass(), window_capturer.Pass()));
    }

    // DesktopMediaPicker is implemented only for Windows, OSX and
    // Aura Linux builds.
#if defined(TOOLKIT_VIEWS) || defined(OS_MAC)
    picker_ = DesktopMediaPicker::Create();
#else
    error_ = "Desktop Capture API is not yet implemented for this platform.";
    return false;
#endif
  }
  DesktopMediaPicker::DoneCallback callback = base::Bind(
      &DesktopCaptureChooseDesktopMediaFunction::OnPickerDialogResults, this);

  picker_->Show(web_contents,
                parent_window,
                parent_window,
                target_name,
                target_name,
                media_list.Pass(),
                callback);
  return true;
}

void DesktopCaptureChooseDesktopMediaFunction::WebContentsDestroyed() {
  Cancel();
}

void DesktopCaptureChooseDesktopMediaFunction::OnPickerDialogResults(
    content::DesktopMediaID source) {
  std::string result;
  if (source.type != content::DesktopMediaID::TYPE_NONE &&
      web_contents()) {
    result = source.ToString();
  }

  SetResult(new base::StringValue(result));
  SendResponse(true);
}

DesktopCaptureRequestsRegistry::RequestId::RequestId(int process_id,
                                                     int request_id)
    : process_id(process_id),
      request_id(request_id) {
}

bool DesktopCaptureRequestsRegistry::RequestId::operator<(
    const RequestId& other) const {
  if (process_id != other.process_id) {
    return process_id < other.process_id;
  } else {
    return request_id < other.request_id;
  }
}

DesktopCaptureCancelChooseDesktopMediaFunction::
    DesktopCaptureCancelChooseDesktopMediaFunction() {}

DesktopCaptureCancelChooseDesktopMediaFunction::
    ~DesktopCaptureCancelChooseDesktopMediaFunction() {}

bool DesktopCaptureCancelChooseDesktopMediaFunction::RunSync() {
  int request_id;
  EXTENSION_FUNCTION_VALIDATE(args_->GetInteger(0, &request_id));

  DesktopCaptureRequestsRegistry::GetInstance()->CancelRequest(
      render_view_host()->GetProcess()->GetID(), request_id);
  return true;
}

DesktopCaptureRequestsRegistry::DesktopCaptureRequestsRegistry() {}
DesktopCaptureRequestsRegistry::~DesktopCaptureRequestsRegistry() {}

// static
DesktopCaptureRequestsRegistry* DesktopCaptureRequestsRegistry::GetInstance() {
  return Singleton<DesktopCaptureRequestsRegistry>::get();
}

void DesktopCaptureRequestsRegistry::AddRequest(
    int process_id,
    int request_id,
    DesktopCaptureChooseDesktopMediaFunction* handler) {
  requests_.insert(
      RequestsMap::value_type(RequestId(process_id, request_id), handler));
}

void DesktopCaptureRequestsRegistry::RemoveRequest(int process_id,
                                                   int request_id) {
  requests_.erase(RequestId(process_id, request_id));
}

void DesktopCaptureRequestsRegistry::CancelRequest(int process_id,
                                                   int request_id) {
  RequestsMap::iterator it = requests_.find(RequestId(process_id, request_id));
  if (it != requests_.end())
    it->second->Cancel();
}


}  // namespace extensions
