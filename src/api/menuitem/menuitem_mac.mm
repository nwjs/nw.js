// Copyright (c) 2012 Intel Corp
// Copyright (c) 2012 The Chromium Authors
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

#include "content/nw/src/api/menuitem/menuitem.h"

#include "base/values.h"
#import <Cocoa/Cocoa.h>
#include "content/nw/src/api/dispatcher_host.h"
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/api/menuitem/menuitem_delegate_mac.h"

namespace nwapi {

void MenuItem::Create(const base::DictionaryValue& option) {
  std::string type;
  option.GetString("type", &type);
  type_ = type;

  if (type == "separator") {
    menu_item_ = [NSMenuItem separatorItem];

    // Balance release
    delegate_ = [MenuItemDelegate alloc];
    [menu_item_ retain];
  } else {
    std::string label;
    option.GetString("label", &label);

    std::string selector;
    option.GetString("selector", &selector);

    if(!selector.empty()) {
      menu_item_ = [[NSMenuItem alloc]
                    initWithTitle:[NSString stringWithUTF8String:label.c_str()]
                    action: NSSelectorFromString([NSString stringWithUTF8String:selector.c_str()])
                    keyEquivalent: @""];
      delegate_ = [MenuItemDelegate alloc];
    } else {
      menu_item_ = [[NSMenuItem alloc]
                    initWithTitle:[NSString stringWithUTF8String:label.c_str()]
                    action: @selector(invoke:)
                    keyEquivalent: @""];
      delegate_ = [[MenuItemDelegate alloc] initWithMenuItem:this];
      [menu_item_ setTarget:delegate_];
    }

    if (type == "checkbox") {
      bool checked = false;
      option.GetBoolean("checked", &checked);
      SetChecked(checked);
    }

    bool enabled;
    if (option.GetBoolean("enabled", &enabled))
      SetEnabled(enabled);

    bool isTemplate;
    if (option.GetBoolean("iconIsTemplate", &isTemplate))
      SetIconIsTemplate(isTemplate);

    std::string icon;
    if (option.GetString("icon", &icon) && !icon.empty())
      SetIcon(icon);

    std::string tooltip;
    if (option.GetString("tooltip", &tooltip))
      SetTooltip(tooltip);

    std::string key;
    if (option.GetString("key", &key))
      SetKey(key);

    std::string modifiers;
    if (option.GetString("modifiers", &modifiers)) {
      SetModifiers(modifiers);
    }

    int menu_id;
    if (option.GetInteger("submenu", &menu_id))
      SetSubmenu(dispatcher_host()->GetApiObject<Menu>(menu_id));
  }
}

void MenuItem::OnClick() {
  if (type_ != "checkbox")
    return;

  if ([menu_item_ state] == NSOffState)
    [menu_item_ setState:NSOnState];
  else
    [menu_item_ setState:NSOffState];
}

void MenuItem::Destroy() {
  [menu_item_ release];
  [delegate_ release];
}

void MenuItem::SetLabel(const std::string& label) {
  [menu_item_ setTitle:[NSString stringWithUTF8String:label.c_str()]];
}

void MenuItem::SetKey(const std::string& key) {
  [menu_item_ setKeyEquivalent:[NSString stringWithUTF8String:key.c_str()]];
  VLOG(1) << "setkey: " << key;
}

void MenuItem::SetModifiers(const std::string& modifiers) {
  NSUInteger mask = 0;
  NSString* nsmodifiers = [NSString stringWithUTF8String:modifiers.c_str()];
  if([nsmodifiers rangeOfString:@"shift"].location != NSNotFound)
    mask = mask|NSShiftKeyMask;
  if([nsmodifiers rangeOfString:@"cmd"].location != NSNotFound)
    mask = mask|NSCommandKeyMask;
  if([nsmodifiers rangeOfString:@"alt"].location != NSNotFound)
    mask = mask|NSAlternateKeyMask;
  if([nsmodifiers rangeOfString:@"ctrl"].location != NSNotFound)
    mask = mask|NSControlKeyMask;
  [menu_item_ setKeyEquivalentModifierMask:mask];
}

void MenuItem::SetIcon(const std::string& icon) {
  if (!icon.empty()) {
    NSImage* image = [[NSImage alloc]
         initWithContentsOfFile:[NSString stringWithUTF8String:icon.c_str()]];
    [image setTemplate:iconIsTemplate];
    [menu_item_ setImage:image];
    [image release];
  } else {
    [menu_item_ setImage:nil];
  }
}

void MenuItem::SetIconIsTemplate(bool isTemplate) {
  iconIsTemplate = isTemplate;
  if ([menu_item_ image] != nil)
    [[menu_item_ image] setTemplate:isTemplate];
}

void MenuItem::SetTooltip(const std::string& tooltip) {
  [menu_item_ setToolTip:[NSString stringWithUTF8String:tooltip.c_str()]];
}

void MenuItem::SetEnabled(bool enabled) {
  [menu_item_ setEnabled:enabled];
}

void MenuItem::SetChecked(bool checked) {
  if (checked)
    [menu_item_ setState:NSOnState];
  else
    [menu_item_ setState:NSOffState];
}

void MenuItem::SetSubmenu(Menu* sub_menu) {
  [sub_menu->menu_ setTitle:[menu_item_ title]];
  [menu_item_ setSubmenu:sub_menu->menu_];
}

}  // namespace nwapi
