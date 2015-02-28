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

#include "content/nw/src/api/desktopcapture/desktopcapture.h"

#include "base/values.h"
#include "base/strings/utf_string_conversions.h"
#include "base/strings/string16.h"
#include "content/nw/src/api/dispatcher_host.h"

#include "third_party/webrtc/modules/desktop_capture/desktop_capture_options.h"
#include "third_party/webrtc/modules/desktop_capture/screen_capturer.h"
#include "third_party/webrtc/modules/desktop_capture/window_capturer.h"


namespace nwapi {

DesktopCapture::DesktopCapture(int id,
           const base::WeakPtr<DispatcherHost>& dispatcher_host,
           const base::DictionaryValue& option)
    : Base(id, dispatcher_host, option) {
}

DesktopCapture::~DesktopCapture() {
}


void DesktopCapture::CallSync(const std::string& method,
                         const base::ListValue& arguments,
                         base::ListValue* result) {
	
  if (method == "getDefault") {
    result->AppendString(GetDefault());
  } else {
    NOTREACHED() << "Invalid call to DesktopCapture method:" << method
                 << " arguments:" << arguments;
  }
}

void DesktopCapture::Call(const std::string& method, const base::ListValue& arguments) {

	if (method == "start") {
		bool windows, screens;
		if (arguments.GetBoolean(0, &windows) && arguments.GetBoolean(1, &screens))
			Start(windows, screens);
	}else if (method == "stop") {
		Stop();
	}
	else {
		NOTREACHED() << "Invalid call to DesktopCapture method:" << method
			<< " arguments:" << arguments;
	}
}

void DesktopCapture::Start(bool windows, bool screens) {
	
	webrtc::DesktopCaptureOptions options = webrtc::DesktopCaptureOptions::CreateDefault();
	options.set_disable_effects(false);
	scoped_ptr<webrtc::ScreenCapturer> screen_capturer(screens ? webrtc::ScreenCapturer::Create(options) : NULL);
	scoped_ptr<webrtc::WindowCapturer> window_capturer(windows ? webrtc::WindowCapturer::Create(options) : NULL);

	media_list.reset(new NativeDesktopMediaList(screen_capturer.Pass(), window_capturer.Pass()));

	media_list->StartUpdating(this);
}

void DesktopCapture::Stop() {
	media_list.reset();
}

std::string DesktopCapture::GetDefault() {
	//this->dispatcher_host()->SendEvent(this, "click", *result);
	//retstream << media_list->GetSourceCount();
	//return std::string(retstream.str());
	return std::string();
}

void DesktopCapture::OnSourceAdded(int index){
	const content::DesktopMediaID id = media_list->GetSource(index).id;
	std::stringstream stream;
	stream << id.id;
	std::string idStr = std::string(stream.str());
	base::ListValue source;
	source.AppendString(idStr);
	this->dispatcher_host()->SendEvent(this, "__nw_desktopcapture_listner", source);
}
void DesktopCapture::OnSourceRemoved(int index){

}
void DesktopCapture::OnSourceMoved(int old_index, int new_index){

}
void DesktopCapture::OnSourceNameChanged(int index){

}
void DesktopCapture::OnSourceThumbnailChanged(int index){

}

}  // namespace nwapi
