#include "content/nw/src/nw_content_mac.h"

#import <Cocoa/Cocoa.h>

#include "content/nw/src/api/menu/menu.h"
#include "extensions/browser/app_window/native_app_window.h"
#import  "ui/gfx/mac/nswindow_frame_controls.h"

void NWChangeAppMenu(nw::Menu* menu) {
  NSMenu *main_menu;

  if (menu == nil) {
    main_menu = [[NSMenu alloc] initWithTitle:@""];
  } else {
    main_menu = menu->menu_;
  }
  
  [NSApp setMainMenu:main_menu];
}

void NWSetNSWindowShowInTaskbar(extensions::NativeAppWindow* win, bool show) {
  NSWindow* nswin = win->GetNativeWindow();
  gfx::SetNSWindowShowInTaskbar(nswin, show);
}

