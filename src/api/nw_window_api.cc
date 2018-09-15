#include "content/nw/src/api/nw_window_api.h"

#include "base/base64.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/task/post_task.h"
#include "content/public/browser/render_widget_host.h"
#include "chrome/browser/devtools/devtools_window.h"
#include "chrome/browser/extensions/devtools_util.h"
#include "chrome/browser/extensions/extension_tab_util.h"
#include "chrome/browser/extensions/window_controller.h"
#include "chrome/browser/extensions/api/tabs/windows_util.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_finder.h"
#include "chrome/browser/ui/browser_window.h"
#include "components/zoom/zoom_controller.h"
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/api/object_manager.h"
#include "content/public/common/content_features.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/web_contents.h"
#include "extensions/browser/app_window/app_window_registry.h"
#include "extensions/browser/extension_zoom_request_client.h"
#include "extensions/components/native_app_window/native_app_window_views.h"
#include "extensions/common/error_utils.h"
#include "extensions/common/constants.h"
#include "ui/gfx/codec/jpeg_codec.h"
#include "ui/gfx/codec/png_codec.h"
#include "ui/display/display.h"
#include "ui/gfx/geometry/size_conversions.h"
#include "ui/display/screen.h"
#include "content/nw/src/api/nw_current_window_internal.h"

#if defined(OS_WIN)
#include <shobjidl.h>
#include <dwmapi.h>

#include "base/win/windows_version.h"
#include "ui/base/win/hidden_window.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/icon_util.h"
#include "ui/gfx/font_list.h"
#include "ui/gfx/platform_font.h"
#include "ui/display/win/dpi.h"
#include "ui/views/win/hwnd_util.h"
#endif

#if defined(OS_LINUX)
#include "chrome/browser/ui/libgtkui/gtk_ui.h"
#endif

#if defined(OS_LINUX) || defined(OS_WIN)
#include "content/nw/src/browser/menubar_view.h"
#include "content/nw/src/browser/browser_view_layout.h"
using nw::BrowserViewLayout;
#endif

#if defined(OS_MACOSX)
#include "content/nw/src/nw_content_mac.h"
#endif

#include "chrome/browser/ui/webui/print_preview/print_preview_handler.h"

using content::RenderWidgetHost;
using content::RenderWidgetHostView;
using content::WebContents;
using zoom::ZoomController;

using nw::Menu;

#if defined(OS_LINUX) || defined(OS_WIN)
using nw::MenuBarView;
#endif

#if defined(OS_LINUX)
static void SetDeskopEnvironment() {
  static bool runOnce = false;
  if (runOnce) return;
  runOnce = true;

  std::unique_ptr<base::Environment> env(base::Environment::Create());
  std::string name;
  //if (env->GetVar("CHROME_DESKTOP", &name) && !name.empty())
  //  return;

  if (!env->GetVar("NW_DESKTOP", &name) || name.empty())
    name = "nw.desktop";

  env->SetVar("CHROME_DESKTOP", name);
}

#endif

namespace {

printing::PrinterList EnumeratePrintersAsync() {
  base::AssertBlockingAllowed();

  scoped_refptr<printing::PrintBackend> print_backend(
        printing::PrintBackend::CreateInstance(nullptr));

  printing::PrinterList printer_list;
  print_backend->EnumeratePrinters(&printer_list);

  return printer_list;
}
}

namespace extensions {

namespace windows = api::windows;
namespace tabs = api::tabs;

using api::extension_types::InjectDetails;

namespace {

const char kNoAssociatedAppWindow[] =
    "The context from which the function was called did not have an "
    "associated app window.";

template <typename T>
class ApiParameterExtractor {
 public:
  explicit ApiParameterExtractor(T* params) : params_(params) {}
  ~ApiParameterExtractor() {}

  bool populate_tabs() {
    if (params_->get_info.get() && params_->get_info->populate.get())
      return *params_->get_info->populate;
    return false;
  }

  WindowController::TypeFilter type_filters() {
    if (params_->get_info.get() && params_->get_info->window_types.get())
      return WindowController::GetFilterFromWindowTypes(
          *params_->get_info->window_types);
    return WindowController::kNoWindowFilter;
  }

 private:
  T* params_;
};

}

#if 0
static Browser* getBrowser(UIThreadExtensionFunction* func) {
  content::WebContents* web_contents = func->GetSenderWebContents();
  if (!web_contents) {
    return NULL;
  }
  Browser* browser = chrome::FindBrowserWithWebContents(web_contents);
  return browser;
}
#endif

static Browser* getBrowser(UIThreadExtensionFunction* func, int id) {
  Browser* browser = nullptr;
  std::string error;
  if (!windows_util::GetBrowserFromWindowID(
     func, id, WindowController::GetAllWindowFilter(),
          &browser, &error)) {
    return nullptr;
  }
  return browser;
}

static AppWindow* getAppWindow(UIThreadExtensionFunction* func) {
  AppWindowRegistry* registry = AppWindowRegistry::Get(func->browser_context());
  DCHECK(registry);
  content::WebContents* web_contents = func->GetSenderWebContents();
  if (!web_contents) {
    // No need to set an error, since we won't return to the caller anyway if
    // there's no RVH.
    return NULL;
  }
  return registry->GetAppWindowForWebContents(web_contents);
}

#ifdef OS_WIN
static HWND getHWND(AppWindow* window) {
  if (window == NULL) return NULL;
  native_app_window::NativeAppWindowViews* native_app_window_views =
    static_cast<native_app_window::NativeAppWindowViews*>(
    window->GetBaseWindow());
  return views::HWNDForWidget(native_app_window_views->widget()->GetTopLevelWidget());
}
#endif

void NwCurrentWindowInternalCloseFunction::DoClose(AppWindow* window) {
  window->GetBaseWindow()->ForceClose();
}

void NwCurrentWindowInternalCloseFunction::DoCloseBrowser(Browser* browser) {
  browser->window()->ForceClose();
}

ExtensionFunction::ResponseAction
NwCurrentWindowInternalCloseFunction::Run() {
  std::unique_ptr<extensions::nwapi::nw_current_window_internal::Close::Params> params(
                      extensions::nwapi::nw_current_window_internal::Close::Params::Create(*args_));
  EXTENSION_FUNCTION_VALIDATE(params.get());

  bool force = params->force.get() ? *params->force : false;
  if (base::FeatureList::IsEnabled(::features::kNWNewWin)) {
    int id = 0;
    args_->GetInteger(1, &id);
    Browser* browser = getBrowser(this, id);
    if (!browser)
      return RespondNow(Error("cannot find browser window"));
    if (force)
      base::MessageLoop::current()->task_runner()->PostTask(FROM_HERE,
         base::Bind(&NwCurrentWindowInternalCloseFunction::DoCloseBrowser, browser));
    else if (browser->NWCanClose())
      base::MessageLoop::current()->task_runner()->PostTask(FROM_HERE,
         base::Bind(&NwCurrentWindowInternalCloseFunction::DoCloseBrowser, browser));
  } else {
    AppWindow* window = getAppWindow(this);
    if (force)
      base::MessageLoop::current()->task_runner()->PostTask(FROM_HERE,
         base::Bind(&NwCurrentWindowInternalCloseFunction::DoClose, window));
    else if (window->NWCanClose())
      base::MessageLoop::current()->task_runner()->PostTask(FROM_HERE,
         base::Bind(&NwCurrentWindowInternalCloseFunction::DoClose, window));
  }

  return RespondNow(NoArguments());
}

void NwCurrentWindowInternalShowDevToolsInternalFunction::OnOpened() {
  Respond(NoArguments());
  Release();
}

ExtensionFunction::ResponseAction
NwCurrentWindowInternalShowDevToolsInternalFunction::Run() {
  content::RenderFrameHost* rfh = render_frame_host();
  content::WebContents* web_contents = content::WebContents::FromRenderFrameHost(rfh);
  scoped_refptr<content::DevToolsAgentHost> agent(
      content::DevToolsAgentHost::GetOrCreateFor(web_contents));
  DevToolsWindow::OpenDevToolsWindow(web_contents);
  DevToolsWindow* devtools_window =
      DevToolsWindow::FindDevToolsWindow(agent.get());
  if (devtools_window) {
    AddRef();
    devtools_window->SetLoadCompletedCallback(base::Bind(&NwCurrentWindowInternalShowDevToolsInternalFunction::OnOpened, base::Unretained(this)));
  } else
    return RespondNow(NoArguments());

  return RespondLater();
}

void NwCurrentWindowInternalShowDevTools2InternalFunction::OnOpened() {
  Respond(NoArguments());
  Release();
}

ExtensionFunction::ResponseAction
NwCurrentWindowInternalShowDevTools2InternalFunction::Run() {
  int id = 0;
  EXTENSION_FUNCTION_VALIDATE(args_);
  EXTENSION_FUNCTION_VALIDATE(args_->GetInteger(0, &id));

  Browser* browser = getBrowser(this, id);
  content::WebContents* web_contents = browser->tab_strip_model()->GetActiveWebContents();
  scoped_refptr<content::DevToolsAgentHost> agent(
      content::DevToolsAgentHost::GetOrCreateFor(web_contents));
  DevToolsWindow::OpenDevToolsWindow(web_contents);
  DevToolsWindow* devtools_window =
      DevToolsWindow::FindDevToolsWindow(agent.get());
  if (devtools_window) {
    AddRef();
    devtools_window->SetLoadCompletedCallback(base::Bind(&NwCurrentWindowInternalShowDevTools2InternalFunction::OnOpened, base::Unretained(this)));
  } else
    return RespondNow(NoArguments());

  return RespondLater();
}

ExtensionFunction::ResponseAction
NwCurrentWindowInternalCloseDevToolsFunction::Run() {
  content::RenderFrameHost* rfh = render_frame_host();
  content::WebContents* web_contents = content::WebContents::FromRenderFrameHost(rfh);
  scoped_refptr<content::DevToolsAgentHost> agent(
      content::DevToolsAgentHost::GetOrCreateFor(web_contents));
  DevToolsWindow* devtools_window =
      DevToolsWindow::FindDevToolsWindow(agent.get());
  if (devtools_window) {
    devtools_window->Close();
  }
  return RespondNow(NoArguments());
}

NwCurrentWindowInternalCapturePageInternalFunction::NwCurrentWindowInternalCapturePageInternalFunction() {
}

NwCurrentWindowInternalCapturePageInternalFunction::~NwCurrentWindowInternalCapturePageInternalFunction() {
}

ExtensionFunction::ResponseAction
NwCurrentWindowInternalCapturePageInternalFunction::Run() {
  EXTENSION_FUNCTION_VALIDATE(args_);

  std::unique_ptr<ImageDetails> image_details;
  if (args_->GetSize() > 0) {
    base::Value* spec = NULL;
    EXTENSION_FUNCTION_VALIDATE(args_->Get(0, &spec) && spec);
    image_details = ImageDetails::FromValue(*spec);
  }

  content::RenderFrameHost* rfh = render_frame_host();
  WebContents* contents = content::WebContents::FromRenderFrameHost(rfh);
  if (!contents)
    return RespondNow(Error("contents is null"));

  // The default format and quality setting used when encoding jpegs.
  const api::extension_types::ImageFormat kDefaultFormat =
      api::extension_types::IMAGE_FORMAT_JPEG;
  const int kDefaultQuality = 90;

  image_format_ = kDefaultFormat;
  image_quality_ = kDefaultQuality;

  if (image_details) {
    if (image_details->format !=
        api::extension_types::IMAGE_FORMAT_NONE)
      image_format_ = image_details->format;
    if (image_details->quality.get())
      image_quality_ = *image_details->quality;
  }

  // TODO(miu): Account for fullscreen render widget?  http://crbug.com/419878
  RenderWidgetHostView* const view = contents->GetRenderWidgetHostView();
  if (!view) {
    return RespondNow(Error("view is invisible"));
  }

  view->CopyFromSurface(gfx::Rect(),  // Copy entire surface area.
                        gfx::Size(),  // Result contains device-level detail.
      base::Bind(&NwCurrentWindowInternalCapturePageInternalFunction::CopyFromBackingStoreComplete,
                 this));
  return RespondLater();
}

void NwCurrentWindowInternalCapturePageInternalFunction::CopyFromBackingStoreComplete(
    const SkBitmap& bitmap) {
  if (!bitmap.drawsNothing()) {
    OnCaptureSuccess(bitmap);
    return;
  }
  OnCaptureFailure(FAILURE_REASON_UNKNOWN);
}

void NwCurrentWindowInternalCapturePageInternalFunction::OnCaptureSuccess(const SkBitmap& bitmap) {
  std::vector<unsigned char> data;
  bool encoded = false;
  std::string mime_type;
  switch (image_format_) {
    case api::extension_types::IMAGE_FORMAT_JPEG:
      encoded = gfx::JPEGCodec::Encode(bitmap, image_quality_, &data);
      mime_type = kMimeTypeJpeg;
      break;
    case api::extension_types::IMAGE_FORMAT_PNG:
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
                                     reinterpret_cast<const char*>(data.data()), data.size());

  base::Base64Encode(stream_as_string, &base64_result);
  base64_result.insert(
      0, base::StringPrintf("data:%s;base64,", mime_type.c_str()));
  Respond(OneArgument(std::make_unique<base::Value>(base64_result)));
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
  Respond(Error(error_));
}

NwCurrentWindowInternalClearMenuFunction::NwCurrentWindowInternalClearMenuFunction() {
}

NwCurrentWindowInternalClearMenuFunction::~NwCurrentWindowInternalClearMenuFunction() {
}

ExtensionFunction::ResponseAction
NwCurrentWindowInternalClearMenuFunction::Run() {
  AppWindow* window = getAppWindow(this);
  if (!window) {
    error_ = kNoAssociatedAppWindow;
    return RespondNow(Error(error_));
  }

#if defined(OS_MACOSX)
  NWChangeAppMenu(NULL);
#endif

#if defined(OS_LINUX) || defined(OS_WIN)
  native_app_window::NativeAppWindowViews* native_app_window_views =
      static_cast<native_app_window::NativeAppWindowViews*>(
          window->GetBaseWindow());

  BrowserViewLayout *browser_view_layout = static_cast<BrowserViewLayout*>(native_app_window_views->GetLayoutManager());
  views::View* menubar = browser_view_layout->menu_bar();
  if (menubar) {
    native_app_window_views->RemoveChildView(menubar);
  }
  browser_view_layout->set_menu_bar(NULL);
  native_app_window_views->layout_();
  native_app_window_views->SchedulePaint();
  if (window->menu_) {
    window->menu_->RemoveKeys();
    window->menu_ = NULL;
  }
#endif
  return RespondNow(NoArguments());
}

NwCurrentWindowInternalSetMenuFunction::NwCurrentWindowInternalSetMenuFunction() {
}

NwCurrentWindowInternalSetMenuFunction::~NwCurrentWindowInternalSetMenuFunction() {
}

bool NwCurrentWindowInternalSetMenuFunction::RunNWSync(base::ListValue* response, std::string* error) {
  int id = 0;
  EXTENSION_FUNCTION_VALIDATE(args_->GetInteger(0, &id));
  AppWindow* window = getAppWindow(this);
  if (!window) {
    error_ = kNoAssociatedAppWindow;
    return false;
  }
  nw::ObjectManager* obj_manager = nw::ObjectManager::Get(browser_context());
  Menu* menu = (Menu*)obj_manager->GetApiObject(id);

#if defined(OS_LINUX) || defined(OS_WIN)
  Menu* old_menu = window->menu_;
#endif

  window->menu_ = menu;
  
#if defined(OS_MACOSX)
  response->Append(NWChangeAppMenu(menu));
#endif

#if defined(OS_LINUX) || defined(OS_WIN)
  native_app_window::NativeAppWindowViews* native_app_window_views =
      static_cast<native_app_window::NativeAppWindowViews*>(
          window->GetBaseWindow());

  MenuBarView* menubar = new MenuBarView();
  static_cast<BrowserViewLayout*>(native_app_window_views->GetLayoutManager())->set_menu_bar(menubar);
  native_app_window_views->AddChildView(menubar);
  menubar->UpdateMenu(menu->model());
  native_app_window_views->layout_();
  native_app_window_views->SchedulePaint();
  if (old_menu) old_menu->RemoveKeys();
  menu->UpdateKeys( native_app_window_views->widget()->GetFocusManager() );
  response->Append(std::unique_ptr<base::ListValue>(new base::ListValue()));
#endif
  return true;
}
  
#if defined(OS_WIN)
static base::win::ScopedHICON createBadgeIcon(const HWND hWnd, const TCHAR *value, const int sizeX, const int sizeY) {
  // canvas for the overlay icon
  gfx::Canvas canvas(gfx::Size(sizeX, sizeY), 1, false);

  // drawing red circle
  cc::PaintFlags flags;
  flags.setColor(SK_ColorRED);
  canvas.DrawCircle(gfx::Point(sizeX / 2, sizeY / 2), sizeX / 2, flags);

  // drawing the text
  gfx::PlatformFont *platform_font = gfx::PlatformFont::CreateDefault();
  const int fontSize = sizeY * 0.65f;
  gfx::Font font(platform_font->GetFontName(), fontSize);
  platform_font->Release();
  platform_font = NULL;
  const int yMargin = (sizeY - fontSize) / 2;
  canvas.DrawStringRectWithFlags(value, gfx::FontList(font), SK_ColorWHITE, gfx::Rect(sizeX, fontSize + yMargin + 1), gfx::Canvas::TEXT_ALIGN_CENTER);

  // return the canvas as windows native icon handle
  return IconUtil::CreateHICONFromSkBitmap(canvas.GetBitmap());
}
#endif

#ifndef OS_MACOSX
ExtensionFunction::ResponseAction
NwCurrentWindowInternalSetBadgeLabelFunction::Run() {
  EXTENSION_FUNCTION_VALIDATE(args_);
  std::string badge;
  EXTENSION_FUNCTION_VALIDATE(args_->GetString(0, &badge));
#if defined(OS_WIN)
  Microsoft::WRL::ComPtr<ITaskbarList3> taskbar;
  HRESULT result = ::CoCreateInstance(CLSID_TaskbarList, nullptr,
    CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&taskbar));

  if (FAILED(result)) {
    error_ = "Failed creating a TaskbarList3 object: ";
    LOG(ERROR) << error_ << result;
    return RespondNow(Error(error_));
  }

  result = taskbar->HrInit();
  if (FAILED(result)) {
    error_ = "Failed initializing an ITaskbarList3 interface.";
    LOG(ERROR) << error_;
    return RespondNow(Error(error_));
  }

  base::win::ScopedHICON icon;
  HWND hWnd = getHWND(getAppWindow(this));
  if (hWnd == NULL) {
    error_ = kNoAssociatedAppWindow;
    LOG(ERROR) << error_;
    return RespondNow(Error(error_));
  }
  const float scale = display::win::GetDPIScale();
  if (badge.size())
    icon = createBadgeIcon(hWnd, base::UTF8ToUTF16(badge).c_str(), 16 * scale, 16 * scale);

  taskbar->SetOverlayIcon(hWnd, icon.get(), L"Status");
#elif defined(OS_LINUX)
  views::LinuxUI* linuxUI = views::LinuxUI::instance();
  if (linuxUI == NULL) {
    error_ = "LinuxUI::instance() is NULL";
    return RespondNow(Error(error_));
  }
  SetDeskopEnvironment();
  linuxUI->SetDownloadCount(atoi(badge.c_str()));
#else
  error_ = "NwCurrentWindowInternalSetBadgeLabelFunction NOT Implemented"
  NOTIMPLEMENTED() << error_;
  return RespondNow(Error(error_));
#endif
  return RespondNow(NoArguments());
}
  
ExtensionFunction::ResponseAction
NwCurrentWindowInternalRequestAttentionInternalFunction::Run() {
  EXTENSION_FUNCTION_VALIDATE(args_);
  int count;
  EXTENSION_FUNCTION_VALIDATE(args_->GetInteger(0, &count));
#if defined(OS_WIN)
  FLASHWINFO fwi;
  fwi.cbSize = sizeof(fwi);
  fwi.hwnd = getHWND(getAppWindow(this));
  if (fwi.hwnd == NULL) {
    error_ = kNoAssociatedAppWindow;
    LOG(ERROR) << error_;
    return RespondNow(Error(error_));
  }
  if (count != 0) {
    fwi.dwFlags = FLASHW_ALL;
    fwi.uCount = count < 0 ? 4 : count;
    fwi.dwTimeout = 0;
  }
  else {
    fwi.dwFlags = FLASHW_STOP;
  }
  FlashWindowEx(&fwi);
#elif defined(OS_LINUX) || defined(OS_MACOSX)
  AppWindow* window = getAppWindow(this);
  if (!window) {
    error_ = kNoAssociatedAppWindow;
    return RespondNow(Error(error_));
  }
  window->GetBaseWindow()->FlashFrame(count);
#endif
  return RespondNow(NoArguments());
}
  
ExtensionFunction::ResponseAction
NwCurrentWindowInternalSetProgressBarFunction::Run() {
  EXTENSION_FUNCTION_VALIDATE(args_);
  double progress;
  EXTENSION_FUNCTION_VALIDATE(args_->GetDouble(0, &progress));
#if defined(OS_WIN)
  Microsoft::WRL::ComPtr<ITaskbarList3> taskbar;
  HRESULT result = ::CoCreateInstance(CLSID_TaskbarList, nullptr,
    CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&taskbar));

  if (FAILED(result)) {
    error_ = "Failed creating a TaskbarList3 object: ";
    LOG(ERROR) <<  error_ << result;
    return RespondNow(Error(error_));
  }

  result = taskbar->HrInit();
  if (FAILED(result)) {
    error_ = "Failed initializing an ITaskbarList3 interface.";
    LOG(ERROR) << error_;
    return RespondNow(Error(error_));
  }

  HWND hWnd = getHWND(getAppWindow(this));
  if (hWnd == NULL) {
    error_ = kNoAssociatedAppWindow;
    LOG(ERROR) << error_;
    return RespondNow(Error(error_));
  }
  TBPFLAG tbpFlag = TBPF_NOPROGRESS;

  if (progress > 1) {
    tbpFlag = TBPF_INDETERMINATE;
  }
  else if (progress >= 0) {
    tbpFlag = TBPF_NORMAL;
    taskbar->SetProgressValue(hWnd, progress * 100, 100);
  }

  taskbar->SetProgressState(hWnd, tbpFlag);
#elif defined(OS_LINUX)
  views::LinuxUI* linuxUI = views::LinuxUI::instance();
  if (linuxUI == NULL) {
    error_ = "LinuxUI::instance() is NULL";
    return RespondNow(Error(error_));
  }
  SetDeskopEnvironment();
  linuxUI->SetProgressFraction(progress);
#else
  error_ = "NwCurrentWindowInternalSetProgressBarFunction NOT Implemented"
  NOTIMPLEMENTED() << error_;
  return RespondNow(Error(error_));
#endif
  return RespondNow(NoArguments());
}
#endif

ExtensionFunction::ResponseAction
NwCurrentWindowInternalReloadIgnoringCacheFunction::Run() {
  content::WebContents* web_contents = GetSenderWebContents();
  web_contents->GetController().Reload(content::ReloadType::BYPASSING_CACHE, false);
  return RespondNow(NoArguments());
}

bool NwCurrentWindowInternalGetZoomFunction::RunNWSync(base::ListValue* response, std::string* error) {
  content::WebContents* web_contents = GetSenderWebContents();
  if (!web_contents)
    return false;
  double zoom_level =
      ZoomController::FromWebContents(web_contents)->GetZoomLevel();
  response->AppendDouble(zoom_level);
  return true;
}

bool NwCurrentWindowInternalSetZoomFunction::RunNWSync(base::ListValue* response, std::string* error) {
  double zoom_level;

  EXTENSION_FUNCTION_VALIDATE(args_->GetDouble(0, &zoom_level));
  content::WebContents* web_contents = GetSenderWebContents();
  if (!web_contents)
    return false;
  ZoomController* zoom_controller =
      ZoomController::FromWebContents(web_contents);
  scoped_refptr<ExtensionZoomRequestClient> client(
      new ExtensionZoomRequestClient(extension()));
  zoom_controller->SetZoomMode(zoom::ZoomController::ZOOM_MODE_ISOLATED);
  if (!zoom_controller->SetZoomLevelByClient(zoom_level, client)) {
    return false;
  }
  return true;
}

ExtensionFunction::ResponseAction
NwCurrentWindowInternalEnterKioskModeFunction::Run() {
  AppWindow* window = getAppWindow(this);
  window->ForcedFullscreen();
  return RespondNow(NoArguments());
}

ExtensionFunction::ResponseAction
NwCurrentWindowInternalLeaveKioskModeFunction::Run() {
  AppWindow* window = getAppWindow(this);
  window->Restore();
  return RespondNow(NoArguments());
}

ExtensionFunction::ResponseAction
NwCurrentWindowInternalToggleKioskModeFunction::Run() {
  AppWindow* window = getAppWindow(this);
  if (window->IsFullscreen() || window->IsForcedFullscreen())
    window->Restore();
  else
    window->ForcedFullscreen();
  return RespondNow(NoArguments());
}

bool NwCurrentWindowInternalIsKioskInternalFunction::RunNWSync(base::ListValue* response, std::string* error) {
  AppWindow* window = getAppWindow(this);
  if (window->IsFullscreen() || window->IsForcedFullscreen())
    response->AppendBoolean(true);
  else
    response->AppendBoolean(false);
  return true;
}

bool NwCurrentWindowInternalGetTitleInternalFunction::RunNWSync(base::ListValue* response, std::string* ret_error) {
  AppWindow* window = getAppWindow(this);
  if (window) {
    response->AppendString(window->title_override());
    return true;
  }
  Browser* browser = nullptr;
  std::string error;
  if (!windows_util::GetBrowserFromWindowID(
     this, extension_misc::kCurrentWindowId, WindowController::GetAllWindowFilter(),
          &browser, &error)) {
    *ret_error = error;
    return false;
  }
  if (!browser) {
    *ret_error = "no window found";
    return false;
  }
  response->AppendString(browser->GetWindowTitleForCurrentTab(false));
  return true;
}

ExtensionFunction::ResponseAction
NwCurrentWindowInternalSetShadowFunction::Run() {
#if defined(OS_MACOSX)
  EXTENSION_FUNCTION_VALIDATE(args_);
  bool shadow;
  EXTENSION_FUNCTION_VALIDATE(args_->GetBoolean(0, &shadow));
  AppWindow* window = getAppWindow(this);
  SetShadowOnWindow(window->GetNativeWindow(), shadow);
#endif
  return RespondNow(NoArguments());
}

bool NwCurrentWindowInternalSetTitleInternalFunction::RunNWSync(base::ListValue* response, std::string* error) {
  EXTENSION_FUNCTION_VALIDATE(args_);
  std::string title;
  EXTENSION_FUNCTION_VALIDATE(args_->GetString(0, &title));
  AppWindow* window = getAppWindow(this);
  window->set_title_override(title);
  window->GetBaseWindow()->UpdateWindowTitle();
  return true;
}

ExtensionFunction::ResponseAction
NwCurrentWindowInternalGetPrintersFunction::Run() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  base::PostTaskWithTraitsAndReplyWithResult(
                                             FROM_HERE, {base::MayBlock(), base::TaskShutdownBehavior::CONTINUE_ON_SHUTDOWN},
        base::Bind(&EnumeratePrintersAsync),
        base::Bind(&NwCurrentWindowInternalGetPrintersFunction::OnGetPrinterList,
                   this));
  return RespondLater();
}

void NwCurrentWindowInternalGetPrintersFunction::OnGetPrinterList(const printing::PrinterList& printer_list) {
  base::ListValue* printers = new base::ListValue();
  chrome::PrintersToValues(printer_list, printers);
  Respond(OneArgument(base::WrapUnique(printers)));
}

bool NwCurrentWindowInternalSetPrintSettingsInternalFunction::RunNWSync(base::ListValue* response, std::string* error) {
  EXTENSION_FUNCTION_VALIDATE(args_);

  if (!args_->GetSize())
    return false;
  base::Value* spec = NULL;
  EXTENSION_FUNCTION_VALIDATE(args_->Get(0, &spec) && spec);
  if (!spec->is_dict())
    return false;
  const base::DictionaryValue* dict = static_cast<const base::DictionaryValue*>(spec);
  bool auto_print;
  std::string printer_name, pdf_path;
  if (dict->GetBoolean("autoprint", &auto_print))
    chrome::NWPrintSetCustomPrinting(auto_print);
  if (dict->GetString("printer", &printer_name))
    chrome::NWPrintSetDefaultPrinter(printer_name);
  if (dict->GetString("pdf_path", &pdf_path))
    chrome::NWPrintSetPDFPath(base::FilePath::FromUTF8Unsafe(pdf_path));
  chrome::NWPrintSetOptions(dict);
  return true;
}

bool NwCurrentWindowInternalGetCurrentFunction::RunNWSync(base::ListValue* response, std::string* ret_error) {
  std::unique_ptr<windows::GetCurrent::Params> params(
      windows::GetCurrent::Params::Create(*args_));
  EXTENSION_FUNCTION_VALIDATE(params.get());

  ApiParameterExtractor<windows::GetCurrent::Params> extractor(params.get());
  Browser* browser = nullptr;
  std::string error;
  if (!windows_util::GetBrowserFromWindowID(
          this, extension_misc::kCurrentWindowId, extractor.type_filters(),
          &browser, &error)) {
    *ret_error = error;
    return false;
  }

  ExtensionTabUtil::PopulateTabBehavior populate_tab_behavior =
      extractor.populate_tabs() ? ExtensionTabUtil::kPopulateTabs
                                : ExtensionTabUtil::kDontPopulateTabs;
  std::unique_ptr<base::DictionaryValue> windows =
      ExtensionTabUtil::CreateWindowValueForExtension(*browser, extension(),
                                                      populate_tab_behavior);
  response->Append(std::move(windows));
  return true;
}

bool NwCurrentWindowInternalGetWinParamInternalFunction::RunNWSync(base::ListValue* response, std::string* error) {
  AppWindow* app_window = getAppWindow(this);

  if (!app_window) {
    *error = "cannot get current window; are you in background page/node context?";
    return false;
  }
  //from app_window_api.cc
  content::RenderFrameHost* created_frame =
      app_window->web_contents()->GetMainFrame();
  int frame_id = created_frame->GetRoutingID();

  base::DictionaryValue* result = new base::DictionaryValue;
  result->Set("frameId", std::make_unique<base::Value>(frame_id));
  result->Set("id", std::make_unique<base::Value>(app_window->window_key()));
  app_window->GetSerializedState(result);

  response->Append(base::WrapUnique(result));

  return true;
}

ExtensionFunction::ResponseAction
NwCurrentWindowInternalSetShowInTaskbarFunction::Run() {
  EXTENSION_FUNCTION_VALIDATE(args_);
  bool show;
  EXTENSION_FUNCTION_VALIDATE(args_->GetBoolean(0, &show));
#if defined(OS_WIN)
  AppWindow* window = getAppWindow(this);

  native_app_window::NativeAppWindowViews* native_app_window_views =
      static_cast<native_app_window::NativeAppWindowViews*>(
          window->GetBaseWindow());

  views::Widget* widget = native_app_window_views->widget()->GetTopLevelWidget();

  if (show == false && base::win::GetVersion() < base::win::VERSION_VISTA) {
    // Change the owner of native window. Only needed on Windows XP.
    ::SetParent(views::HWNDForWidget(widget),
                ui::GetHiddenWindow());
  }

  Microsoft::WRL::ComPtr<ITaskbarList3> taskbar;
  HRESULT result = ::CoCreateInstance(CLSID_TaskbarList, nullptr,
                                          CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&taskbar));
  if (FAILED(result)) {
    VLOG(1) << "Failed creating a TaskbarList object: " << result;
    return RespondNow(NoArguments());
  }

  result = taskbar->HrInit();
  if (FAILED(result)) {
    LOG(ERROR) << "Failed initializing an ITaskbarList interface.";
    return RespondNow(NoArguments());
  }

  if (show)
    result = taskbar->AddTab(views::HWNDForWidget(widget));
  else
    result = taskbar->DeleteTab(views::HWNDForWidget(widget));

  if (FAILED(result)) {
    LOG(ERROR) << "Failed to change the show in taskbar attribute";
    return RespondNow(NoArguments());
  }
#elif defined(OS_MACOSX)
  AppWindow* app_window = getAppWindow(this);
  extensions::NativeAppWindow* native_window = app_window->GetBaseWindow();
  NWSetNSWindowShowInTaskbar(native_window, show);
#endif
  return RespondNow(NoArguments());
}

} // namespace extensions

