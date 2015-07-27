#include "content/nw/src/nw_content_mac.h"

#import <Cocoa/Cocoa.h>

#include "content/nw/src/api/menu/menu.h"

void NWChangeAppMenu(nw::Menu* menu) {
  [NSApp setMainMenu:menu->menu_];
}
