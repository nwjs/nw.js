// Copyright (c) 2012 Intel Corp
// Copyright (c) 2012 The Chromium Authors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell co
// pies of the Software, and to permit persons to whom the Software is furnished
// to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in al
// l copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IM
// PLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNES
// S FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
// OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WH
// ETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#ifndef CONTENT_NW_SRC_API_DESKTOPCAPTURE_H_
#define CONTENT_NW_SRC_API_DESKTOPCAPTURE_H_
#include "base/compiler_specific.h"
#include "content/nw/src/api/base/base.h"
#include "chrome/browser/media/desktop_media_list_observer.h"
#include "chrome/browser/media/native_desktop_media_list.h"

namespace nwapi {
	class DesktopCapture : public Base, public DesktopMediaListObserver {
		public:
			DesktopCapture(int id,
				const base::WeakPtr<DispatcherHost>& dispatcher_host,
				const base::DictionaryValue& option);
			virtual ~DesktopCapture();
			virtual void CallSync(const std::string& method, const base::ListValue& arguments, base::ListValue* result) override;
			virtual void OnSourceAdded(int index);
			virtual void OnSourceRemoved(int index);
			virtual void OnSourceMoved(int old_index, int new_index);
			virtual void OnSourceNameChanged(int index);
			virtual void OnSourceThumbnailChanged(int index);
		private:
			DesktopCapture();
			DISALLOW_COPY_AND_ASSIGN(DesktopCapture);
			void Start(bool screens, bool windows);
			void Stop();
			
			std::string GetStringId(DesktopMediaList::Source * id);
			scoped_ptr<DesktopMediaList> media_list;
	};
} // namespace api
#endif // CONTENT_NW_SRC_API_DESKTOPCAPTURE_H_