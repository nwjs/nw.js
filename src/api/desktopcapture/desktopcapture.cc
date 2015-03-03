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
#include "chrome/browser/media/desktop_streams_registry.h"
#include "chrome/browser/media/media_capture_devices_dispatcher.h"

#include "third_party/webrtc/modules/desktop_capture/desktop_capture_options.h"
#include "third_party/webrtc/modules/desktop_capture/screen_capturer.h"
#include "third_party/webrtc/modules/desktop_capture/window_capturer.h"

#include "base/base64.h"
#include "ui/gfx/codec/png_codec.h"
#include "ui/gfx/image/image.h"
#include "ui/gfx/image/image_skia.h"


namespace nwapi {

DesktopCapture::DesktopCapture(int id,
	const base::WeakPtr<DispatcherHost>& dispatcher_host,
	const base::DictionaryValue& option)
	: Base(id, dispatcher_host, option) {
}

DesktopCapture::~DesktopCapture() {
	media_list.reset();
}


void DesktopCapture::CallSync(const std::string& method, const base::ListValue& arguments, base::ListValue* result){
	if (method == "start") {
		bool screens, windows;
		if (arguments.GetBoolean(0, &screens) && arguments.GetBoolean(1, &windows))
			Start(screens, windows);
	}else if (method == "stop") {
		Stop();
	}
	else {
		NOTREACHED() << "Invalid call to DesktopCapture method:" << method
			<< " arguments:" << arguments;
	}
}

void DesktopCapture::Start(bool screens, bool windows) {
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

//note: I dont see any reason for ToString() not to have const declared, but it is not, so we have to remove const here
std::string DesktopCapture::GetStringId(DesktopMediaList::Source * id){
	return const_cast<content::DesktopMediaID*>(&(id->id))->ToString();
}

void DesktopCapture::OnSourceAdded(int index){
	DesktopMediaList::Source src = media_list->GetSource(index);
	
	base::ListValue param;
	param.AppendString(GetStringId(&src));
	param.AppendString(src.name);
	param.AppendInteger(index);
	this->dispatcher_host()->SendEvent(this, "__nw_desktopcapture_listner_added", param);
}
void DesktopCapture::OnSourceRemoved(int index){
	base::ListValue param;
	param.AppendInteger(index); //Note: pass by index here, because the information about which ID was at that index is lost before the removed callback is called.  Its saved in the javascript though, so we can look it up there
	this->dispatcher_host()->SendEvent(this, "__nw_desktopcapture_listner_removed", param);
}
void DesktopCapture::OnSourceMoved(int old_index, int new_index){
	DesktopMediaList::Source src = media_list->GetSource(new_index);
	base::ListValue param;
	param.AppendString(GetStringId(&src));
	param.AppendInteger(new_index);
	param.AppendInteger(old_index);
	this->dispatcher_host()->SendEvent(this, "__nw_desktopcapture_listner_moved", param);
}
void DesktopCapture::OnSourceNameChanged(int index){
	DesktopMediaList::Source src = media_list->GetSource(index);

	base::ListValue param;
	param.AppendString(GetStringId(&src));
	param.AppendString(src.name);
	this->dispatcher_host()->SendEvent(this, "__nw_desktopcapture_listner_namechanged", param);
}

void DesktopCapture::OnSourceThumbnailChanged(int index){
	std::string base64;

	DesktopMediaList::Source src = media_list->GetSource(index);
	SkBitmap bitmap = src.thumbnail.GetRepresentation(1).sk_bitmap();
	SkAutoLockPixels lock_image(bitmap);
	std::vector<unsigned char> data;
	bool success = gfx::PNGCodec::EncodeBGRASkBitmap(bitmap, false, &data);
	if (success){
		base::StringPiece raw_str(reinterpret_cast<const char*>(&data[0]), data.size());
		base::Base64Encode(raw_str, &base64);
	}
	base::ListValue param;
	param.AppendString(GetStringId(&src));
	param.AppendString(base64);
	this->dispatcher_host()->SendEvent(this, "__nw_desktopcapture_listner_thumbnailchanged", param);
}

}  // namespace nwapi
