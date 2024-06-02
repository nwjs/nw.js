// Copyright (c) 2014 Jefry Tedjokusumo <jtg_gg@yahoo.com.sg>
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


#include "base/values.h"
#include "content/nw/src/api/screen/screen.h"
#include "content/nw/src/api/screen/desktop_capture_api.h"
#include "content/nw/src/api/dispatcher_host.h"
#include "content/nw/src/api/event/event.h"
#include "content/nw/src/nw_shell.h"
#include "ui/gfx/display_observer.h"
#include "ui/gfx/screen.h"

namespace nwapi {
std::string DisplayToJSON(const gfx::Display& display) {
  std::stringstream ret;
  gfx::Rect rect = display.bounds();

  ret << "{\"id\":" << display.id();

  ret << ",\"bounds\":{\"x\":" << rect.x()
  << ", \"y\":" << rect.y()
  << ", \"width\":" << rect.width()
  << ", \"height\":" << rect.height() << "}";

  rect = display.work_area();
  ret << ",\"work_area\":{\"x\":" << rect.x()
  << ", \"y\":" << rect.y()
  << ", \"width\":" << rect.width()
  << ", \"height\":" << rect.height() << "}";

  ret << ",\"scaleFactor\":" << display.device_scale_factor();
  ret << ",\"isBuiltIn\":" << (display.IsInternal() ? "true" : "false");
  ret << ",\"rotation\":" << display.RotationAsDegree();
  ret << ",\"touchSupport\":" << display.touch_support();
  ret << "}";

  return ret.str();
}

class JavaScriptDisplayObserver : BaseEvent, public gfx::DisplayObserver {
  friend class EventListener;
  EventListener* object_;
  gfx::Screen* screen_;

  // Called when the |display|'s bound has changed.
   void OnDisplayMetricsChanged(const gfx::Display& display, uint32_t changed_metrics) override {
    base::ListValue arguments;
    arguments.AppendString(DisplayToJSON(display));
    arguments.AppendInteger(changed_metrics);
    object_->dispatcher_host()->SendEvent(object_, "displayBoundsChanged", arguments);
  }

  // Called when |new_display| has been added.
   void OnDisplayAdded(const gfx::Display& new_display) override {
    base::ListValue arguments;
    arguments.AppendString(DisplayToJSON(new_display));
    object_->dispatcher_host()->SendEvent(object_, "displayAdded", arguments);

  }

  // Called when |old_display| has been removed.
   void OnDisplaysRemoved(const gfx::Displays& old_displays) override {
    base::ListValue arguments;
    for (const auto& display : old_displays) {
      arguments.AppendString(DisplayToJSON(display));
    }
    object_->dispatcher_host()->SendEvent(object_, "displayRemoved", arguments);
  }

  static const int id;

  JavaScriptDisplayObserver(EventListener* object) : object_(object), screen_(NULL){
  }

   ~JavaScriptDisplayObserver() override {
    if(screen_)
      screen_->RemoveObserver(this);
  }

public:
  void setScreen(gfx::Screen* screen) {
    if(screen_) screen_->RemoveObserver(this);
    screen_ = screen;
    if(screen_) screen_->AddObserver(this);
  }
};

const int JavaScriptDisplayObserver::id = EventListener::getUID();

scoped_refptr<DesktopCaptureChooseDesktopMediaFunction> gpDCCDMF;
  
void ChooseDesktopMediaCallback(EventListener* event_listener,
  ExtensionFunction::ResponseType type,
  const base::ListValue& results,
  const std::string& error) {
  
  event_listener->dispatcher_host()->SendEvent(event_listener, "chooseDesktopMedia", results);
  gpDCCDMF = NULL;
}
  // static
void Screen::Call(DispatcherHost* dispatcher_host,
               const std::string& method,
               const base::ListValue& arguments,
               base::ListValue* result) {

  if (method == "GetScreens") {
    std::stringstream ret;
    const std::vector<gfx::Display>& displays = gfx::Screen::GetNativeScreen()->GetAllDisplays();

    if (displays.size() == 0) {
      result->AppendString("{}");
      return;
    }

    for (size_t i=0; i<displays.size(); i++) {
      if(i!=0) ret << ",";
      ret << DisplayToJSON(displays[i]);
    }
    result->AppendString("["+ret.str()+"]");
    return;
  } else if (method == "AddScreenChangeCallback") {
    int object_id = 0;
    arguments.GetInteger(0, &object_id);
    EventListener* event_listener = dispatcher_host->GetApiObject<EventListener>(object_id);
    JavaScriptDisplayObserver* listener = event_listener->AddListener<JavaScriptDisplayObserver>();
    if (listener) listener->setScreen(gfx::Screen::GetNativeScreen());
    result->AppendBoolean(listener != NULL);
    return;
  } else if (method == "RemoveScreenChangeCallback") {
    int object_id = 0;
    arguments.GetInteger(0, &object_id);
    EventListener* event_listener = dispatcher_host->GetApiObject<EventListener>(object_id);
    bool res = event_listener->RemoveListener<JavaScriptDisplayObserver>();
    result->AppendBoolean(res);
    return;
  } else if (method == "ChooseDesktopMedia") {
    if (gpDCCDMF == NULL) {
      gpDCCDMF = new DesktopCaptureChooseDesktopMediaFunction();
      gpDCCDMF->SetArgs(&arguments);
      gpDCCDMF->SetRenderViewHost(dispatcher_host->render_view_host());
    
      int object_id = 0;
      arguments.GetInteger(0, &object_id);
      EventListener* event_listener = dispatcher_host->GetApiObject<EventListener>(object_id);
      ExtensionFunction::ResponseCallback callback = base::Bind(&ChooseDesktopMediaCallback, event_listener);
      gpDCCDMF->set_response_callback(callback);
      result->AppendBoolean(gpDCCDMF->RunSync());
    } else {
      // Screen Picker GUI is still active, return false;
      result->AppendBoolean(false);
    }
  }

}

} // namespace nwapi
