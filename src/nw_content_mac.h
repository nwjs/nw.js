#ifndef _CONTENT_NW_CONTENT_MAC_H
#define _CONTENT_NW_CONTENT_MAC_H

#include "base/values.h"
#include "ui/gfx/native_ui_types.h"

namespace nw {
class Menu;
}

namespace extensions {
class NativeAppWindow;
}

base::Value::List NWChangeAppMenu(nw::Menu* menu);
void NWSetNSWindowShowInTaskbar(extensions::NativeAppWindow* win, bool show);
void NWSetNSWindowShowInTaskbar(gfx::NativeWindow win, bool show);
void NWSetNSAppKioskOptions(void);
void NWRestoreNSAppKioskOptions(void);
#endif
