#include "content/nw/src/nw_content_mac.h"

#import <Cocoa/Cocoa.h>
#import <AutomaticAssessmentConfiguration/AutomaticAssessmentConfiguration.h>

#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/api/menuitem/menuitem.h"
#include "extensions/browser/app_window/native_app_window.h"
#import  "ui/gfx/mac/nswindow_frame_controls.h"
#include "chrome/grit/generated_resources.h"
#include "ui/base/l10n/l10n_util_mac.h"

API_AVAILABLE(macos(10.15.4))
@interface AssessmentSession : NSObject <AEAssessmentSessionDelegate> {
    AEAssessmentSession* session;
}

- (id)init;

- (void)assessmentSessionDidBegin:(AEAssessmentSession *)session;

- (void)assessmentSessionDidEnd:(AEAssessmentSession *)session;

- (void)assessmentSession:(AEAssessmentSession *)session
   failedToBeginWithError:(NSError *)error;

- (void)assessmentSession:(AEAssessmentSession *)session
  wasInterruptedWithError:(NSError *)error;

- (void)begin;

- (void)end;

- (BOOL)isActive;

@end

@implementation AssessmentSession

- (id)init {
    self = [super init];

    // configuration doesn't have any properties on macOS
    AEAssessmentConfiguration *configuration = [AEAssessmentConfiguration new];

    session = [[AEAssessmentSession alloc] initWithConfiguration:configuration];
    session.delegate = self;

    return self;
}

- (void)begin {
    [session begin];
}


- (void)end {
    [session end];
}


- (BOOL)isActive {
    return session.active;
}

- (void)assessmentSessionDidBegin:(AEAssessmentSession *)assessmentSession {
    NSLog(@"ASSESSMENT MODE ENABLED");
}

- (void)assessmentSessionDidEnd:(AEAssessmentSession *)assessmentSession {
    NSLog(@"ASSESSMENT MODE DISABLED");
}

- (void)assessmentSession:(AEAssessmentSession *)assessmentSession
   failedToBeginWithError:(NSError *)error {
    NSLog(@"FAILED TO BEGIN");
    NSLog(@"%@", [error localizedFailureReason]);

}

- (void)assessmentSession:(AEAssessmentSession *)assessmentSession
  wasInterruptedWithError:(NSError *)error {
    NSLog(@"INTERRUPTED");
    NSLog(@"%@", [error localizedDescription]);
    NSLog(@"%@", [error localizedFailureReason]);
}

@end

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

void NWSetNSWindowShowInTaskbar(gfx::NativeWindow win, bool show) {
    NSWindow* nswin = win.GetNativeNSWindow();
    gfx::SetNSWindowShowInTaskbar(nswin, show);
}

static NSApplicationPresentationOptions previousOptions = 0;
static bool kiosked = false;

API_AVAILABLE(macos(10.15.4))
static AssessmentSession *session;

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

    if (@available(macOS 10.15.4, *)) {
        session = [[AssessmentSession alloc] init];
        [session begin];
    }

}

void NWRestoreNSAppKioskOptions(void) {
    if (kiosked) {
        kiosked = false;
        // Or should we just use NSApplicationPresentationDefault?
        [NSApp setPresentationOptions:previousOptions];
        NSWindow *window = [NSApp keyWindow];
        [window setLevel:NSNormalWindowLevel];
        [NSApp activateIgnoringOtherApps:NO];

        if (@available(macOS 10.15.4, *)) {
            [session end];
        }
    }
}
