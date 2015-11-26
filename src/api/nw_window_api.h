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

class NwCurrentWindowInternalShowDevToolsFunction : public AsyncExtensionFunction {
 public:
  NwCurrentWindowInternalShowDevToolsFunction();

 protected:
  ~NwCurrentWindowInternalShowDevToolsFunction() override;

  // ExtensionFunction:
  bool RunAsync() override;
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.showDevTools", UNKNOWN)
 private:
  void Callback();
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

class NwCurrentWindowInternalSetMenuFunction : public AsyncExtensionFunction {
 public:
  NwCurrentWindowInternalSetMenuFunction();

 protected:
  ~NwCurrentWindowInternalSetMenuFunction() override;

  // ExtensionFunction:
  bool RunAsync() override;
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

class NwCurrentWindowInternalRequestAttentionFunction : public AsyncExtensionFunction {
 public:
  NwCurrentWindowInternalRequestAttentionFunction(){}

 protected:
  ~NwCurrentWindowInternalRequestAttentionFunction() override {}

  // ExtensionFunction:
  bool RunAsync() override;
  DECLARE_EXTENSION_FUNCTION("nw.currentWindowInternal.requestAttention", UNKNOWN)
 private:
  DISALLOW_COPY_AND_ASSIGN(NwCurrentWindowInternalRequestAttentionFunction);
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
} // namespace extensions
#endif
