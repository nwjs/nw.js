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

#include "content/nw/src/api/menu/menu.h"

#import <Cocoa/Cocoa.h>
#include "content/nw/src/api/menuitem/menuitem.h"

namespace api {

using namespace v8;

Menu::Menu(CreationOption option) {
  menu_ = [[NSMenu alloc]
      initWithTitle:[NSString stringWithUTF8String:option.title.c_str()]];
  [menu_ setAutoenablesItems:NO];
}

Menu::~Menu() {
  [menu_ release];
}

void Menu::SetTitle(const std::string& title) {
  [menu_ setTitle:[NSString stringWithUTF8String:title.c_str()]];
}

std::string Menu::GetTitle() {
  return [[menu_ title] UTF8String];
}

void Menu::Append(MenuItem* menu_item) {
  [menu_ addItem:menu_item->menu_item_];
}

void Menu::Insert(MenuItem* menu_item, int pos) {
  [menu_ insertItem:menu_item->menu_item_ atIndex:pos];
}

void Menu::Remove(MenuItem* menu_item) {
  [menu_ removeItem:menu_item->menu_item_];
}

void Menu::Remove(int pos) {
  [menu_ removeItemAtIndex:pos];
}

}  // namespace api
