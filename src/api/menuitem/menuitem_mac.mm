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

#include "base/strings/sys_string_conversions.h"
#include "base/values.h"
#import <Cocoa/Cocoa.h>
#include "content/nw/src/api/base/base_mac.h"
#include "content/nw/src/api/object_manager.h"
#include "content/nw/src/api/menu/menu.h"
#include "content/nw/src/api/menuitem/menuitem_delegate_mac.h"
#include "ui/events/keycodes/keyboard_code_conversion_mac.h"

namespace nw{

void MenuItem::Create(const base::Value::Dict& option) {
  const std::string* str = option.FindString("type");
  if (str)
    type_ = *str;
  native_ = option.FindBool("native").value_or(false);

  if (native_) return;

  if (type_ == "separator") {
    menu_item_ = [NSMenuItem separatorItem];

    // Balance release
    delegate_ = [MenuItemDelegate alloc];
    [menu_item_ retain];
  } else {
    std::string label;
    str = option.FindString("label");
    if (str)
      label = *str;

    std::string selector;
    str = option.FindString("selector");
    if (str)
      selector = *str;

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

    if (type_ == "checkbox") {
      bool checked = option.FindBool("checked").value_or(false);
      SetChecked(checked);
    }

    std::optional<bool> enabled = option.FindBool("enabled");
    if (enabled)
      SetEnabled(*enabled);

    std::optional<bool> isTemplate = option.FindBool("iconIsTemplate");
    if (isTemplate)
      SetIconIsTemplate(*isTemplate);

    std::string icon;
    str = option.FindString("icon");
    if (str)
      icon = *str;
    if (str && !icon.empty())
      SetIcon(icon);

    str = option.FindString("tooltip");
    if (str)
      SetTooltip(*str);

    str = option.FindString("key");
    if (str)
      SetKey(*str);

    str = option.FindString("modifiers");
    if (str) {
      SetModifiers(*str);
    }

    std::optional<int> menu_id = option.FindInt("submenu");
    if (menu_id)
      SetSubmenu(object_manager()->GetApiObject<Menu>(*menu_id));
  }

  [menu_item_ setAssociatedObject: this];
}

// static
base::Value::Dict MenuItem::CreateFromNative(NSMenuItem* menu_item, Menu* menu, int index) {
  base::Value::Dict options;

  options.Set("native", true);

  std::string type("normal");
  if ([menu_item isSeparatorItem]) {
    type = "separator";
  } if ([menu_item state] == NSControlStateValueOn) {
    type = "checkbox";
  }
  options.Set("type", type);

  options.Set("checked", [menu_item state] == NSControlStateValueOn);

  options.Set("label", base::SysNSStringToUTF8([menu_item title]));

  if ([menu_item image] != nil) {
    options.Set("icon", "<native>");
    options.Set("iconIsTemplate", [[menu_item image] isTemplate]);
  }

  options.Set("tooltip", base::SysNSStringToUTF8([menu_item toolTip]));

  options.Set("enabled", [menu_item isEnabled]);

  NSUInteger mask = [menu_item keyEquivalentModifierMask];
  if (mask != 0) {
    std::stringstream s;
    std::vector<std::string> modifiers;
    if (mask & NSEventModifierFlagCommand) modifiers.push_back("cmd");
    if (mask & NSEventModifierFlagControl) modifiers.push_back("ctrl");
    if (mask & NSEventModifierFlagOption) modifiers.push_back("alt");
    if (mask & NSEventModifierFlagShift) modifiers.push_back("shift");
    if (mask & NSEventModifierFlagFunction) modifiers.push_back("fn");
    std::copy(modifiers.begin(), modifiers.end(), std::ostream_iterator<std::string>(s, "+"));
    std::string str = s.str();
    if (str.length() > 0) {
      str.erase(str.length()-1);
    }
    options.Set("modifiers", str);
  }

  NSString* key = [menu_item keyEquivalent];
  if (key != nil) {
    options.Set("key", base::SysNSStringToUTF8(key));
  }

  int menuitem_id = ObjectManager::AllocateId();
  options.Set("id", menuitem_id);

  ObjectManager* manager = menu->object_manager();
  manager->OnAllocateObject(menuitem_id, "MenuItem", options, menu->extension_id_);
  MenuItem* item = reinterpret_cast<MenuItem*>(manager->GetApiObject(menuitem_id));
  item->menu_item_ = menu_item;
  [menu_item setAssociatedObject: item];

  return options;
}

// static
MenuItem* MenuItem::GetMenuItemFromNative(NSMenuItem* menu_item) {
  return (MenuItem*)[menu_item associatedObject];
}

void MenuItem::OnClick() {
  if (type_ != "checkbox")
    return;

  if ([menu_item_ state] == NSControlStateValueOff)
    [menu_item_ setState:NSControlStateValueOn];
  else
    [menu_item_ setState:NSControlStateValueOff];
}

void MenuItem::Destroy() {
  if (native_) return;

  [menu_item_ release];
  [delegate_ release];
}

void MenuItem::SetLabel(const std::string& label) {
  [menu_item_ setTitle:[NSString stringWithUTF8String:label.c_str()]];
}

void MenuItem::SetKey(const std::string& key) {
  ui::KeyboardCode key_code = GetKeycodeFromText(key);
  NSString* key_equivalent;
  if (ui::VKEY_UNKNOWN == key_code) { // legacy key code support
    key_equivalent = [NSString stringWithUTF8String:key.c_str()];
  } else {
    unichar shifted_character;
    int result = ui::MacKeyCodeForWindowsKeyCode(key_code, 0, &shifted_character, NULL);
    DCHECK(result != -1);
    key_equivalent = [NSString stringWithFormat:@"%C", shifted_character];
  }
  [menu_item_ setKeyEquivalent:key_equivalent];
  VLOG(1) << "setkey: " << key;
}

void MenuItem::SetModifiers(const std::string& modifiers) {
  NSUInteger mask = 0;
  NSString* nsmodifiers = [NSString stringWithUTF8String:modifiers.c_str()].lowercaseString;
  if([nsmodifiers rangeOfString:@"shift"].location != NSNotFound)
    mask = mask|NSEventModifierFlagShift;
  if([nsmodifiers rangeOfString:@"cmd"].location != NSNotFound
    || [nsmodifiers rangeOfString:@"command"].location != NSNotFound
    || [nsmodifiers rangeOfString:@"super"].location != NSNotFound)
    mask = mask|NSEventModifierFlagCommand;
  if([nsmodifiers rangeOfString:@"alt"].location != NSNotFound)
    mask = mask|NSEventModifierFlagOption;
  if([nsmodifiers rangeOfString:@"ctrl"].location != NSNotFound)
    mask = mask|NSEventModifierFlagControl;
  if([nsmodifiers rangeOfString:@"fn"].location != NSNotFound)
    mask = mask|NSEventModifierFlagFunction;
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
    [menu_item_ setState:NSControlStateValueOn];
  else
    [menu_item_ setState:NSControlStateValueOff];
}

void MenuItem::SetSubmenu(Menu* sub_menu) {
  [sub_menu->menu_ setTitle:[menu_item_ title]];
  [menu_item_ setSubmenu:sub_menu->menu_];
}

bool MenuItem::GetChecked() {
  return menu_item_.state == NSControlStateValueOn;
}

}  // namespace nw
