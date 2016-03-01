#ifndef NW_SRC_API_NW_SCREEN_API_H_
#define NW_SRC_API_NW_SCREEN_API_H_

#include "extensions/browser/extension_function.h"

namespace extensions {

  // implement nw.Screen.getScreens()
  class NwScreenGetScreensFunction: public NWSyncExtensionFunction {
    public:
      NwScreenGetScreensFunction();
      bool RunNWSync(base::ListValue* response, std::string* error) override;

    protected:
      ~NwScreenGetScreensFunction() override {}
      DECLARE_EXTENSION_FUNCTION("nw.Screen.getScreens", UNKNOWN)

    private:
      DISALLOW_COPY_AND_ASSIGN(NwScreenGetScreensFunction);      
  };

  // implement nw.Screen.initEventListeners()
  class NwScreenInitEventListenersFunction: public NWSyncExtensionFunction {
    public:
      NwScreenInitEventListenersFunction();
      bool RunNWSync(base::ListValue* response, std::string* error) override;

    protected:
      ~NwScreenInitEventListenersFunction() override {}
      DECLARE_EXTENSION_FUNCTION("nw.Screen.initEventListeners", UNKNOWN)

    private:
      DISALLOW_COPY_AND_ASSIGN(NwScreenInitEventListenersFunction);      
  };

  // implement nw.Screen.startMonitor()
  class NwScreenStartMonitorFunction : public NWSyncExtensionFunction {
  public:
    NwScreenStartMonitorFunction();
    bool RunNWSync(base::ListValue* response, std::string* error) override;

  protected:
    ~NwScreenStartMonitorFunction() override {}
    DECLARE_EXTENSION_FUNCTION("nw.Screen.startMonitor", UNKNOWN)

  private:
    DISALLOW_COPY_AND_ASSIGN(NwScreenStartMonitorFunction);
  };

  // implement nw.Screen.stopMonitor()
  class NwScreenStopMonitorFunction : public NWSyncExtensionFunction {
  public:
    NwScreenStopMonitorFunction();
    bool RunNWSync(base::ListValue* response, std::string* error) override;

  protected:
    ~NwScreenStopMonitorFunction() override {}
    DECLARE_EXTENSION_FUNCTION("nw.Screen.stopMonitor", UNKNOWN)

  private:
    DISALLOW_COPY_AND_ASSIGN(NwScreenStopMonitorFunction);
  };

  // implement nw.Screen.isMonitorStarted()
  class NwScreenIsMonitorStartedFunction : public NWSyncExtensionFunction {
  public:
    NwScreenIsMonitorStartedFunction();
    bool RunNWSync(base::ListValue* response, std::string* error) override;

  protected:
    ~NwScreenIsMonitorStartedFunction() override {}
    DECLARE_EXTENSION_FUNCTION("nw.Screen.isMonitorStarted", UNKNOWN)

  private:
    DISALLOW_COPY_AND_ASSIGN(NwScreenIsMonitorStartedFunction);
  };

  // implement nw.Screen.registerStream()
  class NwScreenRegisterStreamFunction : public NWSyncExtensionFunction {
  public:
    NwScreenRegisterStreamFunction();
    bool RunNWSync(base::ListValue* response, std::string* error) override;

  protected:
    ~NwScreenRegisterStreamFunction() override {}
    DECLARE_EXTENSION_FUNCTION("nw.Screen.registerStream", UNKNOWN)

  private:
    DISALLOW_COPY_AND_ASSIGN(NwScreenRegisterStreamFunction);
  };
} // extensions

#endif //NW_SRC_API_NW_SCREEN_API_H_
