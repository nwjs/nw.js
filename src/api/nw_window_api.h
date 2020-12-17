#ifndef NW_API_WINDOW_API_H_
#define NW_API_WINDOW_API_H_

#include <vector>

#include "content/public/browser/readback_types.h"
#include "extensions/browser/extension_function.h"
#include "extensions/common/api/extension_types.h"

class SkBitmap;

namespace content {
class WebContents;
}

namespace extensions {
class AppWindow;

class NwCurrentWindowInternalCloseFunction : public AsyncExtensionFunction {
 public:
  NwCurrentWindowInternalCloseFunction() {};
  static void DoClose(AppWindow*);

 protected:
  ~NwCurrentWindowInternalCloseFunction() override {};

  // ExtensionFunction:
  bool RunAsync() override;
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.close", UNKNOWN)
};


class NwCurrentWindowInternalShowDevToolsInternalFunction : public AsyncExtensionFunction {
 public:
  NwCurrentWindowInternalShowDevToolsInternalFunction() {};

 protected:
  ~NwCurrentWindowInternalShowDevToolsInternalFunction() override {};

  // ExtensionFunction:
  bool RunAsync() override;
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.showDevToolsInternal", UNKNOWN)
 private:
  void OnOpened();
};

class NwCurrentWindowInternalCloseDevToolsFunction : public AsyncExtensionFunction {
 public:
  NwCurrentWindowInternalCloseDevToolsFunction() {};

 protected:
  ~NwCurrentWindowInternalCloseDevToolsFunction() override {};

  // ExtensionFunction:
  bool RunAsync() override;
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.closeDevTools", UNKNOWN)
};

class NwCurrentWindowInternalCapturePageInternalFunction : public AsyncExtensionFunction {
 public:
  NwCurrentWindowInternalCapturePageInternalFunction();

 protected:
  ~NwCurrentWindowInternalCapturePageInternalFunction() override;

  // ExtensionFunction:
  bool RunAsync() override;
  enum FailureReason {
    FAILURE_REASON_UNKNOWN,
    FAILURE_REASON_ENCODING_FAILED,
    FAILURE_REASON_VIEW_INVISIBLE
  };
  virtual void OnCaptureFailure(FailureReason reason);
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.capturePageInternal", UNKNOWN)

 private:
  typedef api::extension_types::ImageDetails ImageDetails;

  void CopyFromBackingStoreComplete(const SkBitmap& bitmap,
                                    content::ReadbackResponse response);
  void OnCaptureSuccess(const SkBitmap& bitmap);

  // The format (JPEG vs PNG) of the resulting image.  Set in RunAsync().
  api::extension_types::ImageFormat image_format_;

  // Quality setting to use when encoding jpegs.  Set in RunAsync().
  int image_quality_;

  DISALLOW_COPY_AND_ASSIGN(NwCurrentWindowInternalCapturePageInternalFunction);
};

class NwCurrentWindowInternalClearMenuFunction : public AsyncExtensionFunction {
 public:
  NwCurrentWindowInternalClearMenuFunction();

 protected:
  ~NwCurrentWindowInternalClearMenuFunction() override;

  // ExtensionFunction:
  bool RunAsync() override;
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.clearMenu", UNKNOWN)

 private:

  DISALLOW_COPY_AND_ASSIGN(NwCurrentWindowInternalClearMenuFunction);
};

class NwCurrentWindowInternalSetMenuFunction : public NWSyncExtensionFunction {
 public:
  NwCurrentWindowInternalSetMenuFunction();

 protected:
  ~NwCurrentWindowInternalSetMenuFunction() override;

  // ExtensionFunction:
  bool RunNWSync(base::ListValue* response, std::string* error) override;
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.setMenu", UNKNOWN)

 private:

  DISALLOW_COPY_AND_ASSIGN(NwCurrentWindowInternalSetMenuFunction);
};

class NwCurrentWindowInternalSetBadgeLabelFunction : public AsyncExtensionFunction {
 public:
  NwCurrentWindowInternalSetBadgeLabelFunction(){}

 protected:
  ~NwCurrentWindowInternalSetBadgeLabelFunction() override {}

  // ExtensionFunction:
  bool RunAsync() override;
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.setBadgeLabel", UNKNOWN)
};

class NwCurrentWindowInternalRequestAttentionInternalFunction : public AsyncExtensionFunction {
 public:
  NwCurrentWindowInternalRequestAttentionInternalFunction(){}

 protected:
  ~NwCurrentWindowInternalRequestAttentionInternalFunction() override {}

  // ExtensionFunction:
  bool RunAsync() override;
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.requestAttentionInternal", UNKNOWN)
 private:
  DISALLOW_COPY_AND_ASSIGN(NwCurrentWindowInternalRequestAttentionInternalFunction);
};
  
class NwCurrentWindowInternalSetProgressBarFunction : public AsyncExtensionFunction {
 public:
  NwCurrentWindowInternalSetProgressBarFunction(){}

 protected:
  ~NwCurrentWindowInternalSetProgressBarFunction() override {}

  // ExtensionFunction:
  bool RunAsync() override;
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.setProgressBar", UNKNOWN)
 private:
  void Callback();
};

class NwCurrentWindowInternalReloadIgnoringCacheFunction : public AsyncExtensionFunction {
 public:
   NwCurrentWindowInternalReloadIgnoringCacheFunction() {}

 protected:
   ~NwCurrentWindowInternalReloadIgnoringCacheFunction() override {}

   // ExtensionFunction:
   bool RunAsync() override;
   DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.reloadIgnoringCache", UNKNOWN)
};

class NwCurrentWindowInternalGetZoomFunction : public NWSyncExtensionFunction {
 public:
  NwCurrentWindowInternalGetZoomFunction() {}
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwCurrentWindowInternalGetZoomFunction() override {}
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.getZoom", UNKNOWN)
};

class NwCurrentWindowInternalSetZoomFunction : public NWSyncExtensionFunction {
 public:
  NwCurrentWindowInternalSetZoomFunction() {}
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwCurrentWindowInternalSetZoomFunction() override {}
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.setZoom", UNKNOWN)
};

class NwCurrentWindowInternalEnterKioskModeFunction : public AsyncExtensionFunction {
 public:
   NwCurrentWindowInternalEnterKioskModeFunction() {}

 protected:
   ~NwCurrentWindowInternalEnterKioskModeFunction() override {}

   // ExtensionFunction:
   bool RunAsync() override;
   DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.enterKioskMode", UNKNOWN)
};

class NwCurrentWindowInternalLeaveKioskModeFunction : public AsyncExtensionFunction {
 public:
   NwCurrentWindowInternalLeaveKioskModeFunction() {}

 protected:
   ~NwCurrentWindowInternalLeaveKioskModeFunction() override {}

   // ExtensionFunction:
   bool RunAsync() override;
   DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.leaveKioskMode", UNKNOWN)
};

class NwCurrentWindowInternalToggleKioskModeFunction : public AsyncExtensionFunction {
 public:
   NwCurrentWindowInternalToggleKioskModeFunction() {}

 protected:
   ~NwCurrentWindowInternalToggleKioskModeFunction() override {}

   // ExtensionFunction:
   bool RunAsync() override;
   DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.toggleKioskMode", UNKNOWN)
};

class NwCurrentWindowInternalIsKioskInternalFunction : public NWSyncExtensionFunction {
 public:
  NwCurrentWindowInternalIsKioskInternalFunction() {}
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwCurrentWindowInternalIsKioskInternalFunction() override {}
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.isKioskInternal", UNKNOWN)
};

class NwCurrentWindowInternalSetShowInTaskbarFunction : public AsyncExtensionFunction {
 public:
  NwCurrentWindowInternalSetShowInTaskbarFunction() {}

 protected:
  ~NwCurrentWindowInternalSetShowInTaskbarFunction() override {}
  
   // ExtensionFunction:
   bool RunAsync() override;
   DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.setShowInTaskbar", UNKNOWN)
};

class NwCurrentWindowInternalSetTitleInternalFunction : public NWSyncExtensionFunction {
 public:
  NwCurrentWindowInternalSetTitleInternalFunction() {}
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwCurrentWindowInternalSetTitleInternalFunction() override {}
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.setTitleInternal", UNKNOWN)
};

class NwCurrentWindowInternalGetTitleInternalFunction : public NWSyncExtensionFunction {
 public:
  NwCurrentWindowInternalGetTitleInternalFunction() {}
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwCurrentWindowInternalGetTitleInternalFunction() override {}
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.getTitleInternal", UNKNOWN)
};

class NwCurrentWindowInternalGetWinParamInternalFunction : public NWSyncExtensionFunction {
 public:
  NwCurrentWindowInternalGetWinParamInternalFunction() {}
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwCurrentWindowInternalGetWinParamInternalFunction() override {}
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.getWinParamInternal", UNKNOWN)
};

class NwCurrentWindowInternalGetPrintersFunction : public AsyncExtensionFunction {
 public:
  NwCurrentWindowInternalGetPrintersFunction() {}
  bool RunAsync() override;
  void OnGetPrinterList(base::ListValue* results);
 protected:
  ~NwCurrentWindowInternalGetPrintersFunction() override {}
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.getPrinters", UNKNOWN)
};

class NwCurrentWindowInternalSetPrintSettingsInternalFunction : public NWSyncExtensionFunction {
 public:
   NwCurrentWindowInternalSetPrintSettingsInternalFunction() {}
   bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
   ~NwCurrentWindowInternalSetPrintSettingsInternalFunction() override {}
   DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.setPrintSettingsInternal", UNKNOWN)
};

class NwCurrentWindowInternalGetAppUserModelIDFunction : public NWSyncExtensionFunction {
 public:
   NwCurrentWindowInternalGetAppUserModelIDFunction() {}
   bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
   ~NwCurrentWindowInternalGetAppUserModelIDFunction() override {}
   DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.getAppUserModelID", UNKNOWN)
};

class NwCurrentWindowInternalSetAppUserModelIDFunction : public NWSyncExtensionFunction {
 public:
   NwCurrentWindowInternalSetAppUserModelIDFunction() {}
   bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
   ~NwCurrentWindowInternalSetAppUserModelIDFunction() override {}
   DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.setAppUserModelID", UNKNOWN)
};
} // namespace extensions
#endif
