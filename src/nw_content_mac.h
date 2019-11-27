#ifndef _CONTENT_NW_CONTENT_MAC_H
#define _CONTENT_NW_CONTENT_MAC_H

#include "base/values.h"

namespace nw {
class Menu;
}

namespace extensions {
class NativeAppWindow;
}

std::unique_ptr<base::ListValue> NWChangeAppMenu(nw::Menu* menu);
void NWSetNSWindowShowInTaskbar(extensions::NativeAppWindow* win, bool show);
void NWSetNSAppKioskOptions(void);
void NWRestoreNSAppKioskOptions(void);
#endif
