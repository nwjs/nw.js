// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NWAPI_DESKTOP_CAPTURE_DESKTOP_CAPTURE_API_H_
#define NWAPI_DESKTOP_CAPTURE_DESKTOP_CAPTURE_API_H_

#include <map>

#include "base/memory/singleton.h"
#include "chrome/browser/media/desktop_media_list.h"
#include "chrome/browser/media/desktop_media_picker.h"
#include "chrome/browser/media/native_desktop_media_list.h"
#include "extensions/browser/extension_function.h"
#include "content/public/browser/web_contents_observer.h"
#include "url/gurl.h"

namespace nwapi {

class DesktopCaptureChooseDesktopMediaFunction
    : public SyncExtensionFunction,
      public content::WebContentsObserver {
 public:

  // Factory creating DesktopMediaList and DesktopMediaPicker instances.
  // Used for tests to supply fake picker.
  class PickerFactory {
   public:
    virtual scoped_ptr<DesktopMediaList> CreateModel(bool show_screens,
                                                     bool show_windows) = 0;
    virtual scoped_ptr<DesktopMediaPicker> CreatePicker() = 0;
   protected:
    virtual ~PickerFactory() {}
  };

  // Used to set PickerFactory used to create mock DesktopMediaPicker instances
  // for tests. Calling tests keep ownership of the factory. Can be called with
  // |factory| set to NULL at the end of the test.
  static void SetPickerFactoryForTests(PickerFactory* factory);

  DesktopCaptureChooseDesktopMediaFunction();

  void Cancel();

  // ExtensionFunction overrides.
  bool RunSync() override;

 private:
  ~DesktopCaptureChooseDesktopMediaFunction() override;

  // content::WebContentsObserver overrides.
  void WebContentsDestroyed() override;

  void OnPickerDialogResults(content::DesktopMediaID source);

  int request_id_;

  // URL of page that desktop capture was requested for.
  GURL origin_;

  scoped_ptr<DesktopMediaPicker> picker_;
};

// this api is not exposed in nwjs yet
class DesktopCaptureCancelChooseDesktopMediaFunction
    : public SyncExtensionFunction {
 public:
  DesktopCaptureCancelChooseDesktopMediaFunction();

 private:
  ~DesktopCaptureCancelChooseDesktopMediaFunction() override;

  // ExtensionFunction overrides.
  bool RunSync() override;
};

// this class is only needed if we want the ability to cancel "choose desktop media"
// currently not used
class DesktopCaptureRequestsRegistry {
 public:
  DesktopCaptureRequestsRegistry();
  ~DesktopCaptureRequestsRegistry();

  static DesktopCaptureRequestsRegistry* GetInstance();

  void AddRequest(int process_id,
                  int request_id,
                  DesktopCaptureChooseDesktopMediaFunction* handler);
  void RemoveRequest(int process_id, int request_id);
  void CancelRequest(int process_id, int request_id);

 private:
  friend struct DefaultSingletonTraits<DesktopCaptureRequestsRegistry>;

  struct RequestId {
    RequestId(int process_id, int request_id);

    // Need to use RequestId as a key in std::map<>.
    bool operator<(const RequestId& other) const;

    int process_id;
    int request_id;
  };

  typedef std::map<RequestId,
                   DesktopCaptureChooseDesktopMediaFunction*> RequestsMap;

  RequestsMap requests_;

  DISALLOW_COPY_AND_ASSIGN(DesktopCaptureRequestsRegistry);
};

}  // namespace extensions

#endif  // NWAPI_DESKTOP_CAPTURE_DESKTOP_CAPTURE_API_H_
