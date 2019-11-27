#include "content/nw/src/nw_content_mac.h"

#import <Cocoa/Cocoa.h>

#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/api/menuitem/menuitem.h"
#include "extensions/browser/app_window/native_app_window.h"
#import  "ui/gfx/mac/nswindow_frame_controls.h"
#include "chrome/grit/generated_resources.h"
#include "ui/base/l10n/l10n_util_mac.h"

std::unique_ptr<base::ListValue> NWChangeAppMenu(nw::Menu* menu) {
  NSMenu *main_menu;

  if (menu == nil) {
    main_menu = [[NSMenu alloc] initWithTitle:@""];
  } else {
    main_menu = menu->menu_;
  }
  
  [NSApp setMainMenu:main_menu];

  std::unique_ptr<base::ListValue> items(new base::ListValue());

  if (menu != nil) {
    NSString* editMenuTitle = l10n_util::GetNSStringWithFixup(IDS_EDIT_MENU_MAC);
    NSInteger editMenuIndex = [main_menu indexOfItemWithTitle:editMenuTitle];
    if (editMenuIndex != -1) {
      NSMenuItem* editMenuItem = [main_menu itemAtIndex:editMenuIndex];
      if (editMenuItem != nil && [editMenuItem hasSubmenu]) {
        NSMenu* editMenu = [editMenuItem submenu];
        nw::Menu* nwEditMenu = nw::Menu::GetMenuFromNative(editMenu);
        NSArray* itemList = [editMenu itemArray];
        for(int i = 0; i < [editMenu numberOfItems]; i++) {
          NSMenuItem* nativeItem = [itemList objectAtIndex:i];
          if (!nw::MenuItem::GetMenuItemFromNative(nativeItem)) {
            std::unique_ptr<base::DictionaryValue> options = nw::MenuItem::CreateFromNative(nativeItem, nwEditMenu, i);
            std::unique_ptr<base::DictionaryValue> menuPatch(new base::DictionaryValue);
            menuPatch->SetInteger("menu", editMenuIndex);
            menuPatch->SetInteger("index", i);
            menuPatch->Set("option", std::move(options));
            items->Append(std::move(menuPatch));
          }
        }
      }
    }
  }

  return items;
}

void NWSetNSWindowShowInTaskbar(extensions::NativeAppWindow* win, bool show) {
  NSWindow* nswin = win->GetNativeWindow().GetNativeNSWindow();
  gfx::SetNSWindowShowInTaskbar(nswin, show);
}


static NSApplicationPresentationOptions previousOptions = 0;
static bool kiosked = false;

void NWSetNSAppKioskOptions(void) {
  if(!kiosked) {
    kiosked = true;
    previousOptions = [NSApp currentSystemPresentationOptions];
  }
  NSApplicationPresentationOptions options = NSApplicationPresentationHideDock | NSApplicationPresentationHideMenuBar | NSApplicationPresentationDisableProcessSwitching | NSApplicationPresentationDisableForceQuit | NSApplicationPresentationDisableSessionTermination | NSApplicationPresentationDisableHideApplication | NSFullScreenWindowMask | NSApplicationPresentationDisableAppleMenu;
  [NSApp setPresentationOptions:options];
  NSWindow *window = [NSApp keyWindow];
  [window setLevel:NSStatusWindowLevel+1];
  [NSApp activateIgnoringOtherApps:YES];
}

void NWRestoreNSAppKioskOptions(void) {
    if (kiosked) {
        kiosked = false;
        // Or should we just use NSApplicationPresentationDefault?
        [NSApp setPresentationOptions:previousOptions]; 
        NSWindow *window = [NSApp keyWindow];
        [window setLevel:NSNormalWindowLevel];
        [NSApp activateIgnoringOtherApps:NO];
    }
}
