#include "nw_screen_api.h"

#if defined(OS_WIN)
#include "windows.h"
#endif

#include "base/lazy_instance.h"
#include "base/values.h"
#include "content/nw/src/api/nw_screen.h"
#include "extensions/browser/extensions_browser_client.h"
#include "ui/display/display_observer.h"
#include "ui/display/display.h"
#include "ui/display/screen.h"
#include "ui/gfx/image/image_skia_rep_default.h"

// For desktop capture APIs
#include "base/base64.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/media/webrtc/desktop_media_list_observer.h"
#include "chrome/browser/media/webrtc/media_capture_devices_dispatcher.h"
#include "chrome/browser/media/webrtc/native_desktop_media_list.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/desktop_streams_registry.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/web_contents.h"
#include "third_party/webrtc/modules/desktop_capture/desktop_capture_options.h"
#include "third_party/webrtc/modules/desktop_capture/desktop_capturer.h"
#include "ui/gfx/codec/png_codec.h"
#include "ui/gfx/image/image.h"
#include "ui/gfx/image/image_skia.h"

using namespace extensions::nwapi::nw__screen;
using namespace content;

namespace extensions {
  
  class NwDesktopCaptureMonitor : public DesktopMediaListObserver {
  public:
    static NwDesktopCaptureMonitor* GetInstance();

    NwDesktopCaptureMonitor();
    void Start(bool screens, bool windows);
    void Stop();
    bool IsStarted();

  private:
    int GetPrimaryMonitorIndex();
    // DesktopMediaListObserver implementation.
    void OnSourceAdded(int index) override;
    void OnSourceRemoved(int index) override;
    void OnSourceMoved(int old_index, int new_index) override;
    void OnSourceNameChanged(int index) override;
    void OnSourceThumbnailChanged(int index) override;
    void OnSourcePreviewChanged(size_t index) override;

    bool started_;
    std::vector<std::unique_ptr<DesktopMediaList>> media_list_;

  };

  class NwScreenDisplayObserver: public display::DisplayObserver {
  public:
    static NwScreenDisplayObserver* GetInstance();
    NwScreenDisplayObserver();

  private:
    ~NwScreenDisplayObserver() override;
    // gfx::DisplayObserver implementation.
    void OnDisplayMetricsChanged(const display::Display& display, uint32_t changed_metrics) override;
    void OnDisplayAdded(const display::Display& new_display) override;
    void OnDisplayRemoved(const display::Display& old_display) override;

  };

  namespace {

    // Helper function to convert display::Display to nwapi::nw__screen::Display
    std::unique_ptr<nwapi::nw__screen::Display> ConvertGfxDisplay(const display::Display& gfx_display) {
      std::unique_ptr<nwapi::nw__screen::Display> displayResult(new nwapi::nw__screen::Display);

      displayResult->id = gfx_display.id();
      displayResult->scale_factor = gfx_display.device_scale_factor();
      displayResult->is_built_in = gfx_display.IsInternal();
      displayResult->rotation = gfx_display.RotationAsDegree();
      displayResult->touch_support = (int)gfx_display.touch_support();

      gfx::Rect rect = gfx_display.bounds();
      DisplayGeometry& bounds = displayResult->bounds;
      bounds.x = rect.x();
      bounds.y = rect.y();
      bounds.width = rect.width();
      bounds.height = rect.height();

      rect = gfx_display.work_area();
      DisplayGeometry& work_area = displayResult->work_area;
      work_area.x = rect.x();
      work_area.y = rect.y();
      work_area.width = rect.width();
      work_area.height = rect.height();

      return displayResult;
    }

    void DispatchEvent(
        events::HistogramValue histogram_value,
        const std::string& event_name,
        base::Value::List args) {
      DCHECK_CURRENTLY_ON(BrowserThread::UI);
      base::Value::List event_args;
      for (size_t i = 0; i < args.size(); i++) {
        event_args.Append(args[i].Clone());
      }
      ExtensionsBrowserClient::Get()->
        BroadcastEventToRenderers(
                                  histogram_value, event_name,
                                  std::move(event_args),
                                  false);
    }

    // Lazy initialize screen event listeners until first call
    base::LazyInstance<NwScreenDisplayObserver>::Leaky
      g_display_observer = LAZY_INSTANCE_INITIALIZER;

    base::LazyInstance<NwDesktopCaptureMonitor>::Leaky
      g_desktop_capture_monitor = LAZY_INSTANCE_INITIALIZER;

  }

  // static
  NwScreenDisplayObserver* NwScreenDisplayObserver::GetInstance() {
    return g_display_observer.Pointer();
  }

  NwScreenDisplayObserver::NwScreenDisplayObserver() {
    display::Screen* screen = display::Screen::GetScreen();
    if (screen) {
      screen->AddObserver(this);
    }
  }

  NwScreenDisplayObserver::~NwScreenDisplayObserver() {
    display::Screen* screen = display::Screen::GetScreen();
    if (screen) {
      screen->RemoveObserver(this);
    }
  }

  // Called when the |display|'s bound has changed.
  void NwScreenDisplayObserver::OnDisplayMetricsChanged(const display::Display& display,
    uint32_t changed_metrics) {
    auto args(
      nwapi::nw__screen::OnDisplayBoundsChanged::Create(*ConvertGfxDisplay(display),
                                                        changed_metrics));
    DispatchEvent(
      events::HistogramValue::UNKNOWN,
      nwapi::nw__screen::OnDisplayBoundsChanged::kEventName,
      std::move(args));
  }

  // Called when |new_display| has been added.
  void NwScreenDisplayObserver::OnDisplayAdded(const display::Display& new_display) {
    auto args(nwapi::nw__screen::OnDisplayAdded::Create(*ConvertGfxDisplay(new_display)));
    DispatchEvent(
      events::HistogramValue::UNKNOWN,
      nwapi::nw__screen::OnDisplayAdded::kEventName,
      std::move(args));
  }

  // Called when |old_display| has been removed.
  void NwScreenDisplayObserver::OnDisplayRemoved(const display::Display& old_display) {
    auto args(nwapi::nw__screen::OnDisplayRemoved::Create(*ConvertGfxDisplay(old_display)));
    DispatchEvent(
      events::HistogramValue::UNKNOWN,
      nwapi::nw__screen::OnDisplayRemoved::kEventName,
      std::move(args));
  }

  NwScreenGetScreensFunction::NwScreenGetScreensFunction() {}

  bool NwScreenGetScreensFunction::RunNWSync(base::Value::List* response, std::string* error) {
    const std::vector<display::Display>& displays = display::Screen::GetScreen()->GetAllDisplays();

    for (size_t i=0; i<displays.size(); i++) {
      response->Append(base::Value::FromUniquePtrValue(ConvertGfxDisplay(displays[i])->ToValue()));
    }

    return true;
  }

  NwScreenInitEventListenersFunction::NwScreenInitEventListenersFunction() {}

  bool NwScreenInitEventListenersFunction::RunNWSync(base::Value::List* response, std::string* error) {
    NwScreenDisplayObserver::GetInstance();
    return true;
  }

  NwDesktopCaptureMonitor* NwDesktopCaptureMonitor::GetInstance() {
    return g_desktop_capture_monitor.Pointer();
  }

  NwDesktopCaptureMonitor::NwDesktopCaptureMonitor()
      : started_(false) {
  }

  void NwDesktopCaptureMonitor::Start(bool screens, bool windows) {
    if (started_) {
      return;
    }

    started_ = true;

    webrtc::DesktopCaptureOptions options = webrtc::DesktopCaptureOptions::CreateDefault();
    options.set_disable_effects(false);

    if (screens) {
      std::unique_ptr<DesktopMediaList> screen_media_list =
        std::make_unique<NativeDesktopMediaList>(
          DesktopMediaList::Type::kScreen,
          webrtc::DesktopCapturer::CreateScreenCapturer(options));
      media_list_.push_back(std::move(screen_media_list));
    }

    if (windows) {
      std::unique_ptr<DesktopMediaList> window_media_list =
        std::make_unique<NativeDesktopMediaList>(
          DesktopMediaList::Type::kWindow,
          webrtc::DesktopCapturer::CreateWindowCapturer(options));
      media_list_.push_back(std::move(window_media_list));
    }

    for (auto& media_list : media_list_) {
      media_list->StartUpdating(this);
    }
  }

  void NwDesktopCaptureMonitor::Stop() {
    started_ = false;
    media_list_.clear();
  }

  bool NwDesktopCaptureMonitor::IsStarted() {
    return started_;
  }

  int NwDesktopCaptureMonitor::GetPrimaryMonitorIndex() {
  #ifdef _WIN32 
    int count=0;
    for (int i = 0;; ++i) {
      DISPLAY_DEVICE device;
      device.cb = sizeof(device);
      BOOL ret = EnumDisplayDevices(NULL, i, &device, 0);
      if(!ret)
        break;
      if (device.StateFlags & DISPLAY_DEVICE_ACTIVE){
        if (device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE){
          return count;
        }
        count++;
      }
    }
  #endif
    return -1;
  }

void NwDesktopCaptureMonitor::OnSourceAdded(int index) {
    DesktopMediaList::Source src = media_list_[0]->GetSource(index);

    std::string type;
    switch(src.id.type) {
    case content::DesktopMediaID::TYPE_WINDOW:
      type = "window";
      break;
    case content::DesktopMediaID::TYPE_SCREEN:
      type = "screen";
      break;
    case content::DesktopMediaID::TYPE_NONE:
      type = "none";
      break;
    default:
      type = "unknown";
      break;
    }

    auto args(nwapi::nw__screen::OnSourceAdded::Create(
      src.id.ToString(),
      base::UTF16ToUTF8(src.name),
      index,
      type,
      src.id.type == content::DesktopMediaID::TYPE_SCREEN && GetPrimaryMonitorIndex() == index));

    DispatchEvent(
      events::HistogramValue::UNKNOWN,
      nwapi::nw__screen::OnSourceAdded::kEventName,
      std::move(args));
  }

void NwDesktopCaptureMonitor::OnSourceRemoved(int index) {
  auto args(nwapi::nw__screen::OnSourceRemoved::Create(index));
  DispatchEvent(
      events::HistogramValue::UNKNOWN,
      nwapi::nw__screen::OnSourceRemoved::kEventName,
      std::move(args));
}

void NwDesktopCaptureMonitor::OnSourceMoved(int old_index, int new_index) {
    DesktopMediaList::Source src = media_list_[0]->GetSource(new_index);
    auto args(nwapi::nw__screen::OnSourceOrderChanged::Create(
      src.id.ToString(),
      new_index,
      old_index));
    DispatchEvent(
      events::HistogramValue::UNKNOWN,
      nwapi::nw__screen::OnSourceOrderChanged::kEventName,
      std::move(args));
  }

void NwDesktopCaptureMonitor::OnSourceNameChanged(int index) {
    DesktopMediaList::Source src = media_list_[0]->GetSource(index);
    auto args(nwapi::nw__screen::OnSourceNameChanged::Create(
      src.id.ToString(),
      base::UTF16ToUTF8(src.name)));
    DispatchEvent(
      events::HistogramValue::UNKNOWN,
      nwapi::nw__screen::OnSourceNameChanged::kEventName,
      std::move(args));
  }

void NwDesktopCaptureMonitor::OnSourcePreviewChanged(size_t index) {
}

void NwDesktopCaptureMonitor::OnSourceThumbnailChanged(int index) {
    std::string base64;

    DesktopMediaList::Source src = media_list_[0]->GetSource(index);
    SkBitmap bitmap = src.thumbnail.GetRepresentation(1).GetBitmap();
    std::vector<unsigned char> data;
    bool success = gfx::PNGCodec::EncodeBGRASkBitmap(bitmap, false, &data);
    if (success){
      base::StringPiece raw_str(reinterpret_cast<const char*>(&data[0]), data.size());
      base::Base64Encode(raw_str, &base64);
    }

    auto args(nwapi::nw__screen::OnSourceThumbnailChanged::Create(
      src.id.ToString(),
      base64));
    DispatchEvent(
      events::HistogramValue::UNKNOWN,
      nwapi::nw__screen::OnSourceThumbnailChanged::kEventName,
      std::move(args));
  }

  NwScreenStartMonitorFunction::NwScreenStartMonitorFunction() {}

  bool NwScreenStartMonitorFunction::RunNWSync(base::Value::List* response, std::string* error) {
    bool screens, windows;
    EXTENSION_FUNCTION_VALIDATE(args().size() >= 2);
    EXTENSION_FUNCTION_VALIDATE(args()[0].is_bool() && args()[1].is_bool());
    screens = args()[0].GetBool();
    windows = args()[1].GetBool();
    NwDesktopCaptureMonitor::GetInstance()->Start(screens, windows);
    return true;
  }

  NwScreenStopMonitorFunction::NwScreenStopMonitorFunction() {}

  bool NwScreenStopMonitorFunction::RunNWSync(base::Value::List* response, std::string* error) {
    NwDesktopCaptureMonitor::GetInstance()->Stop();
    return true;
  }

  NwScreenIsMonitorStartedFunction::NwScreenIsMonitorStartedFunction() {}

  bool NwScreenIsMonitorStartedFunction::RunNWSync(base::Value::List* response, std::string* error) {
    response->Append(NwDesktopCaptureMonitor::GetInstance()->IsStarted());
    return true;
  }

  NwScreenRegisterStreamFunction::NwScreenRegisterStreamFunction() {}

  bool NwScreenRegisterStreamFunction::RunNWSync(base::Value::List* response, std::string* error) {
    std::string id;
    EXTENSION_FUNCTION_VALIDATE(args().size() >= 1);
    EXTENSION_FUNCTION_VALIDATE(args()[0].is_string());
    id = args()[0].GetString();

    // following code is modified from `DesktopCaptureChooseDesktopMediaFunctionBase::OnPickerDialogResults`
    // in chrome/browser/extensions/api/desktop_capture/desktop_capture_base.cc

    content::DesktopMediaID source = content::DesktopMediaID::Parse(id);
    content::WebContents* web_contents = GetSenderWebContents();
    if (!source.is_null() && web_contents) {
      std::string result;
      source.audio_share = true;
      content::DesktopStreamsRegistry* registry = content::DesktopStreamsRegistry::GetInstance();
      // TODO(miu): Once render_frame_host() is being set, we should register the
      // exact RenderFrame requesting the stream, not the main RenderFrame.  With
      // that change, also update
      // MediaCaptureDevicesDispatcher::ProcessDesktopCaptureAccessRequest().
      // http://crbug.com/304341
      content::RenderFrameHost* const main_frame = web_contents->GetPrimaryMainFrame();
      result = registry->RegisterStream(main_frame->GetProcess()->GetID(),
                                        main_frame->GetRoutingID(),
                                        url::Origin::Create(web_contents->GetURL().DeprecatedGetOriginAsURL()),
                                        source,
                                        extension()->name(), content::kRegistryStreamTypeDesktop);
      response->Append(result);
    }
    return true;
  }

} // extensions
