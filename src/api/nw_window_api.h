#ifndef NW_API_WINDOW_API_H_
#define NW_API_WINDOW_API_H_

#include <vector>

#include "extensions/browser/extension_function.h"
#include "extensions/common/api/extension_types.h"
#include "printing/backend/print_backend.h"

class SkBitmap;
class Browser;

namespace content {
class WebContents;
}

namespace extensions {
class AppWindow;

class NwCurrentWindowInternalCloseFunction : public ExtensionFunction {
 public:
  NwCurrentWindowInternalCloseFunction() {}
  static void DoClose(AppWindow*);
  static void DoCloseBrowser(Browser*);

 protected:
  ~NwCurrentWindowInternalCloseFunction() override {}

  // ExtensionFunction:
  ResponseAction Run() override;
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.close", UNKNOWN)
};


class NwCurrentWindowInternalShowDevToolsInternalFunction : public ExtensionFunction {
 public:
  NwCurrentWindowInternalShowDevToolsInternalFunction() {}

 protected:
  ~NwCurrentWindowInternalShowDevToolsInternalFunction() override {}

  // ExtensionFunction:
  ResponseAction Run() override;
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.showDevToolsInternal", UNKNOWN)
 private:
  void OnOpened();
};

class NwCurrentWindowInternalShowDevTools2InternalFunction : public ExtensionFunction {
 public:
  NwCurrentWindowInternalShowDevTools2InternalFunction() {}

 protected:
  ~NwCurrentWindowInternalShowDevTools2InternalFunction() override {}

  // ExtensionFunction:
  ResponseAction Run() override;
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.showDevTools2Internal", UNKNOWN)
 private:
  void OnOpened();
};

class NwCurrentWindowInternalCloseDevToolsFunction : public ExtensionFunction {
 public:
  NwCurrentWindowInternalCloseDevToolsFunction() {}

 protected:
  ~NwCurrentWindowInternalCloseDevToolsFunction() override {}

  // ExtensionFunction:
  ResponseAction Run() override;
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.closeDevTools", UNKNOWN)
};

class NwCurrentWindowInternalCapturePageInternalFunction : public ExtensionFunction {
 public:
  NwCurrentWindowInternalCapturePageInternalFunction();

 protected:
  ~NwCurrentWindowInternalCapturePageInternalFunction() override;

  // ExtensionFunction:
  ResponseAction Run() override;
  enum FailureReason {
    FAILURE_REASON_UNKNOWN,
    FAILURE_REASON_ENCODING_FAILED,
    FAILURE_REASON_VIEW_INVISIBLE
  };
  virtual void OnCaptureFailure(FailureReason reason);
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.capturePageInternal", UNKNOWN)

 private:
  typedef api::extension_types::ImageDetails ImageDetails;

  void CopyFromBackingStoreComplete(const SkBitmap& bitmap);
  void OnCaptureSuccess(const SkBitmap& bitmap);

  // The format (JPEG vs PNG) of the resulting image.  Set in RunAsync().
  api::extension_types::ImageFormat image_format_;

  // Quality setting to use when encoding jpegs.  Set in RunAsync().
  int image_quality_;

  DISALLOW_COPY_AND_ASSIGN(NwCurrentWindowInternalCapturePageInternalFunction);
};

class NwCurrentWindowInternalClearMenuFunction : public ExtensionFunction {
 public:
  NwCurrentWindowInternalClearMenuFunction();

 protected:
  ~NwCurrentWindowInternalClearMenuFunction() override;

  // ExtensionFunction:
  ResponseAction Run() override;
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

class NwCurrentWindowInternalSetShadowInternalFunction : public ExtensionFunction {
 public:
  NwCurrentWindowInternalSetShadowInternalFunction(){}

 protected:
  ~NwCurrentWindowInternalSetShadowInternalFunction() override {}
#if defined(OS_MACOSX)
  void SetShadowOnWindow(NSWindow *window, bool shadow);
#endif

  // ExtensionFunction:
  ResponseAction Run() override;
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.setShadowInternal", UNKNOWN)
};

class NwCurrentWindowInternalSetBadgeLabelFunction : public ExtensionFunction {
 public:
  NwCurrentWindowInternalSetBadgeLabelFunction(){}

 protected:
  ~NwCurrentWindowInternalSetBadgeLabelFunction() override {}

  // ExtensionFunction:
  ResponseAction Run() override;
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.setBadgeLabel", UNKNOWN)
};

class NwCurrentWindowInternalRequestAttentionInternalFunction : public ExtensionFunction {
 public:
  NwCurrentWindowInternalRequestAttentionInternalFunction(){}

 protected:
  ~NwCurrentWindowInternalRequestAttentionInternalFunction() override {}

  // ExtensionFunction:
  ResponseAction Run() override;
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.requestAttentionInternal", UNKNOWN)
 private:
  DISALLOW_COPY_AND_ASSIGN(NwCurrentWindowInternalRequestAttentionInternalFunction);
};
  
class NwCurrentWindowInternalSetProgressBarFunction : public ExtensionFunction {
 public:
  NwCurrentWindowInternalSetProgressBarFunction(){}

 protected:
  ~NwCurrentWindowInternalSetProgressBarFunction() override {}

  // ExtensionFunction:
  ResponseAction Run() override;
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.setProgressBar", UNKNOWN)
 private:
  void Callback();
};

class NwCurrentWindowInternalReloadIgnoringCacheFunction : public ExtensionFunction {
 public:
   NwCurrentWindowInternalReloadIgnoringCacheFunction() {}

 protected:
   ~NwCurrentWindowInternalReloadIgnoringCacheFunction() override {}

   // ExtensionFunction:
   ResponseAction Run() override;
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

class NwCurrentWindowInternalEnterKioskModeFunction : public ExtensionFunction {
 public:
   NwCurrentWindowInternalEnterKioskModeFunction() {}

 protected:
   ~NwCurrentWindowInternalEnterKioskModeFunction() override {}

   // ExtensionFunction:
   ResponseAction Run() override;
   DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.enterKioskMode", UNKNOWN)
};

class NwCurrentWindowInternalLeaveKioskModeFunction : public ExtensionFunction {
 public:
   NwCurrentWindowInternalLeaveKioskModeFunction() {}

 protected:
   ~NwCurrentWindowInternalLeaveKioskModeFunction() override {}

   // ExtensionFunction:
   ResponseAction Run() override;
   DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.leaveKioskMode", UNKNOWN)
};

class NwCurrentWindowInternalToggleKioskModeFunction : public ExtensionFunction {
 public:
   NwCurrentWindowInternalToggleKioskModeFunction() {}

 protected:
   ~NwCurrentWindowInternalToggleKioskModeFunction() override {}

   // ExtensionFunction:
   ResponseAction Run() override;
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

class NwCurrentWindowInternalSetShowInTaskbarFunction : public ExtensionFunction {
 public:
  NwCurrentWindowInternalSetShowInTaskbarFunction() {}

 protected:
  ~NwCurrentWindowInternalSetShowInTaskbarFunction() override {}
  
   // ExtensionFunction:
  ResponseAction Run() override;
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

class NwCurrentWindowInternalGetCurrentFunction : public NWSyncExtensionFunction {
 public:
  NwCurrentWindowInternalGetCurrentFunction() {}
  bool RunNWSync(base::ListValue* response, std::string* error) override;

 protected:
  ~NwCurrentWindowInternalGetCurrentFunction() override {}
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.getCurrent", UNKNOWN)
};

class NwCurrentWindowInternalGetPrintersFunction : public ExtensionFunction {
 public:
  NwCurrentWindowInternalGetPrintersFunction() {}

  void OnGetPrinterList(const printing::PrinterList& printer_list);
 protected:
  ResponseAction Run() override;
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
} // namespace extensions
#endif
