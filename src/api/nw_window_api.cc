#include "content/nw/src/api/nw_window_api.h"

#include "base/base64.h"
#include "base/strings/stringprintf.h"
#include "content/public/browser/render_widget_host.h"
#include "chrome/browser/devtools/devtools_window.h"
#include "chrome/browser/extensions/devtools_util.h"
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/api/object_manager.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/web_contents.h"
#include "extensions/browser/app_window/app_window_registry.h"
#include "extensions/components/native_app_window/native_app_window_views.h"
#include "extensions/common/error_utils.h"
#include "extensions/common/constants.h"
#include "ui/gfx/codec/jpeg_codec.h"
#include "ui/gfx/codec/png_codec.h"
#include "ui/gfx/display.h"
#include "ui/gfx/geometry/size_conversions.h"
#include "ui/gfx/screen.h"

#if defined(OS_WIN)
#include "ui/views/win/hwnd_util.h"
#endif

using content::RenderWidgetHost;
using content::RenderWidgetHostView;
using content::WebContents;

using nw::Menu;

namespace extensions {
namespace {

const char kNoAssociatedAppWindow[] =
    "The context from which the function was called did not have an "
    "associated app window.";
}

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

NwCurrentWindowInternalCapturePageInternalFunction::NwCurrentWindowInternalCapturePageInternalFunction() {
}

NwCurrentWindowInternalCapturePageInternalFunction::~NwCurrentWindowInternalCapturePageInternalFunction() {
}

bool NwCurrentWindowInternalCapturePageInternalFunction::RunAsync() {
  EXTENSION_FUNCTION_VALIDATE(args_);

  scoped_ptr<ImageDetails> image_details;
  if (args_->GetSize() > 1) {
    base::Value* spec = NULL;
    EXTENSION_FUNCTION_VALIDATE(args_->Get(1, &spec) && spec);
    image_details = ImageDetails::FromValue(*spec);
  }

  content::RenderViewHost* rvh = render_view_host();
  WebContents* contents = content::WebContents::FromRenderViewHost(rvh);
  if (!contents)
    return false;

  // The default format and quality setting used when encoding jpegs.
  const core_api::extension_types::ImageFormat kDefaultFormat =
      core_api::extension_types::IMAGE_FORMAT_JPEG;
  const int kDefaultQuality = 90;

  image_format_ = kDefaultFormat;
  image_quality_ = kDefaultQuality;

  if (image_details) {
    if (image_details->format !=
        core_api::extension_types::IMAGE_FORMAT_NONE)
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
      base::Bind(&NwCurrentWindowInternalCapturePageInternalFunction::CopyFromBackingStoreComplete,
                 this),
      kN32_SkColorType);
  return true;
}

void NwCurrentWindowInternalCapturePageInternalFunction::CopyFromBackingStoreComplete(
    const SkBitmap& bitmap,
    content::ReadbackResponse response) {
  if (response == content::READBACK_SUCCESS) {
    OnCaptureSuccess(bitmap);
    return;
  }
  OnCaptureFailure(FAILURE_REASON_UNKNOWN);
}

void NwCurrentWindowInternalCapturePageInternalFunction::OnCaptureSuccess(const SkBitmap& bitmap) {
  std::vector<unsigned char> data;
  SkAutoLockPixels screen_capture_lock(bitmap);
  bool encoded = false;
  std::string mime_type;
  switch (image_format_) {
    case core_api::extension_types::IMAGE_FORMAT_JPEG:
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
    case core_api::extension_types::IMAGE_FORMAT_PNG:
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

void NwCurrentWindowInternalCapturePageInternalFunction::OnCaptureFailure(FailureReason reason) {
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

NwCurrentWindowInternalClearMenuFunction::NwCurrentWindowInternalClearMenuFunction() {
}

NwCurrentWindowInternalClearMenuFunction::~NwCurrentWindowInternalClearMenuFunction() {
}

bool NwCurrentWindowInternalClearMenuFunction::RunAsync() {
  return true;
}

NwCurrentWindowInternalSetMenuFunction::NwCurrentWindowInternalSetMenuFunction() {
}

NwCurrentWindowInternalSetMenuFunction::~NwCurrentWindowInternalSetMenuFunction() {
}

bool NwCurrentWindowInternalSetMenuFunction::RunAsync() {
  int id = 0;
  EXTENSION_FUNCTION_VALIDATE(args_->GetInteger(0, &id));

  AppWindowRegistry* registry = AppWindowRegistry::Get(browser_context());
  DCHECK(registry);
  content::RenderViewHost* rvh = render_view_host();
  if (!rvh)
    // No need to set an error, since we won't return to the caller anyway if
    // there's no RVH.
    return false;
  AppWindow* window = registry->GetAppWindowForRenderViewHost(rvh);
  if (!window) {
    error_ = kNoAssociatedAppWindow;
    return false;
  }
  nw::ObjectManager* obj_manager = nw::ObjectManager::Get(browser_context());
  Menu* menu = (Menu*)obj_manager->GetApiObject(id);

  window->menu_ = menu;
#if defined(OS_LINUX)
  MenuBarView* menubar = new MenuBarView();
  GetBrowserViewLayout()->set_menu_bar(menubar);
  AddChildView(menubar);
  menubar->UpdateMenu(menu->model());
  Layout();
  SchedulePaint();
#endif
  // The menu is lazily built.
#if defined(OS_WIN) //FIXME
  menu->Rebuild();
  menu->SetWindow(window);

  native_app_window::NativeAppWindowViews* native_app_window_views =
      static_cast<native_app_window::NativeAppWindowViews*>(
          window->GetBaseWindow());

  // menu is nwapi::Menu, menu->menu_ is NativeMenuWin,
  BOOL ret = ::SetMenu(views::HWNDForWidget(native_app_window_views->widget()->GetTopLevelWidget()), menu->menu_->GetNativeMenu());
  if (!ret)
	  LOG(ERROR) << "error setting menu";

  ::DrawMenuBar(views::HWNDForWidget(native_app_window_views->widget()->GetTopLevelWidget()));
  native_app_window_views->SchedulePaint();
#endif
  //FIXME menu->UpdateKeys( native_app_window_views->widget()->GetFocusManager() );
  return true;
}

} // namespace extensions
