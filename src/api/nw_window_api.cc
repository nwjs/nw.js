#include "content/nw/src/api/nw_window_api.h"

#include "base/base64.h"
#include "base/strings/stringprintf.h"
#include "content/public/browser/render_widget_host.h"
#include "chrome/browser/devtools/devtools_window.h"
#include "chrome/browser/extensions/devtools_util.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/web_contents.h"
#include "extensions/common/error_utils.h"
#include "extensions/common/constants.h"
#include "ui/gfx/codec/jpeg_codec.h"
#include "ui/gfx/codec/png_codec.h"
#include "ui/gfx/display.h"
#include "ui/gfx/geometry/size_conversions.h"
#include "ui/gfx/screen.h"

using content::RenderWidgetHost;
using content::RenderWidgetHostView;
using content::WebContents;

namespace extensions {
NwCurrentWindowInternalShowDevToolsFunction::NwCurrentWindowInternalShowDevToolsFunction() {

}

NwCurrentWindowInternalShowDevToolsFunction::~NwCurrentWindowInternalShowDevToolsFunction() {
}

bool NwCurrentWindowInternalShowDevToolsFunction::RunAsync() {
  content::RenderViewHost* rvh = render_view_host();
  DevToolsWindow::OpenDevToolsWindow(
      content::WebContents::FromRenderViewHost(rvh));
  SendResponse(true);
  return true;
}

NwCurrentWindowInternalCapturePageFunction::NwCurrentWindowInternalCapturePageFunction() {
}

NwCurrentWindowInternalCapturePageFunction::~NwCurrentWindowInternalCapturePageFunction() {
}

bool NwCurrentWindowInternalCapturePageFunction::RunAsync() {
  EXTENSION_FUNCTION_VALIDATE(args_);

  scoped_ptr<ImageDetails> image_details;
  if (args_->GetSize() > 0) {
    base::Value* spec = NULL;
    EXTENSION_FUNCTION_VALIDATE(args_->Get(0, &spec) && spec);
    image_details = ImageDetails::FromValue(*spec);
  }

  content::RenderViewHost* rvh = render_view_host();
  WebContents* contents = content::WebContents::FromRenderViewHost(rvh);
  if (!contents)
    return false;

  // The default format and quality setting used when encoding jpegs.
  const ImageDetails::Format kDefaultFormat = ImageDetails::FORMAT_JPEG;
  const int kDefaultQuality = 90;

  image_format_ = kDefaultFormat;
  image_quality_ = kDefaultQuality;

  if (image_details) {
    if (image_details->format != ImageDetails::FORMAT_NONE)
      image_format_ = image_details->format;
    if (image_details->quality.get())
      image_quality_ = *image_details->quality;
  }

  // TODO(miu): Account for fullscreen render widget?  http://crbug.com/419878
  RenderWidgetHostView* const view = contents->GetRenderWidgetHostView();
  RenderWidgetHost* const host = view ? view->GetRenderWidgetHost() : nullptr;
  if (!view || !host) {
    OnCaptureFailure(FAILURE_REASON_VIEW_INVISIBLE);
    return false;
  }

  // By default, the requested bitmap size is the view size in screen
  // coordinates.  However, if there's more pixel detail available on the
  // current system, increase the requested bitmap size to capture it all.
  const gfx::Size view_size = view->GetViewBounds().size();
  gfx::Size bitmap_size = view_size;
  const gfx::NativeView native_view = view->GetNativeView();
  gfx::Screen* const screen = gfx::Screen::GetScreenFor(native_view);
  const float scale =
      screen->GetDisplayNearestWindow(native_view).device_scale_factor();
  if (scale > 1.0f)
    bitmap_size = gfx::ToCeiledSize(gfx::ScaleSize(view_size, scale));

  host->CopyFromBackingStore(
      gfx::Rect(view_size),
      bitmap_size,
      base::Bind(&NwCurrentWindowInternalCapturePageFunction::CopyFromBackingStoreComplete,
                 this),
      kN32_SkColorType);
  return true;
}

void NwCurrentWindowInternalCapturePageFunction::CopyFromBackingStoreComplete(
    const SkBitmap& bitmap,
    content::ReadbackResponse response) {
  if (response == content::READBACK_SUCCESS) {
    OnCaptureSuccess(bitmap);
    return;
  }
  OnCaptureFailure(FAILURE_REASON_UNKNOWN);
}

void NwCurrentWindowInternalCapturePageFunction::OnCaptureSuccess(const SkBitmap& bitmap) {
  std::vector<unsigned char> data;
  SkAutoLockPixels screen_capture_lock(bitmap);
  bool encoded = false;
  std::string mime_type;
  switch (image_format_) {
    case ImageDetails::FORMAT_JPEG:
      encoded = gfx::JPEGCodec::Encode(
          reinterpret_cast<unsigned char*>(bitmap.getAddr32(0, 0)),
          gfx::JPEGCodec::FORMAT_SkBitmap,
          bitmap.width(),
          bitmap.height(),
          static_cast<int>(bitmap.rowBytes()),
          image_quality_,
          &data);
      mime_type = kMimeTypeJpeg;
      break;
    case ImageDetails::FORMAT_PNG:
      encoded =
          gfx::PNGCodec::EncodeBGRASkBitmap(bitmap,
                                            true,  // Discard transparency.
                                            &data);
      mime_type = kMimeTypePng;
      break;
    default:
      NOTREACHED() << "Invalid image format.";
  }

  if (!encoded) {
    OnCaptureFailure(FAILURE_REASON_ENCODING_FAILED);
    return;
  }

  std::string base64_result;
  base::StringPiece stream_as_string(
      reinterpret_cast<const char*>(vector_as_array(&data)), data.size());

  base::Base64Encode(stream_as_string, &base64_result);
  base64_result.insert(
      0, base::StringPrintf("data:%s;base64,", mime_type.c_str()));
  SetResult(new base::StringValue(base64_result));
  SendResponse(true);
}

void NwCurrentWindowInternalCapturePageFunction::OnCaptureFailure(FailureReason reason) {
  const char* reason_description = "internal error";
  switch (reason) {
    case FAILURE_REASON_UNKNOWN:
      reason_description = "unknown error";
      break;
    case FAILURE_REASON_ENCODING_FAILED:
      reason_description = "encoding failed";
      break;
    case FAILURE_REASON_VIEW_INVISIBLE:
      reason_description = "view is invisible";
      break;
  }
  error_ = ErrorUtils::FormatErrorMessage("Failed to capture tab: *",
                                          reason_description);
  SendResponse(false);
}

} // namespace extensions
