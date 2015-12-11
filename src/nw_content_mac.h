#ifndef _CONTENT_NW_CONTENT_MAC_H
#define _CONTENT_NW_CONTENT_MAC_H

namespace nw {
class Menu;
}

namespace extensions {
class NativeAppWindow;
}

void NWChangeAppMenu(nw::Menu* menu);
void NWSetNSWindowShowInTaskbar(extensions::NativeAppWindow* win, bool show);
#endif
