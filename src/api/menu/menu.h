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

#ifndef CONTENT_NW_SRC_API_MENU_MENU_H_
#define CONTENT_NW_SRC_API_MENU_MENU_H_ 

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "third_party/node/src/node.h"

#include <string>
#include <vector>

#if defined(OS_MACOSX)
#if __OBJC__
@class NSMenu;
#else
class NSMenu;
#endif  // __OBJC__
#elif defined(TOOLKIT_GTK)
struct GtkMenu;
#elif defined(OS_WIN)
#include <windows.h>
#endif  // defined(OS_MACOSX)

namespace api {

class MenuItem;

class Menu : node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> target);

 private:
  friend class MenuItem;
  friend class Tray;

  // The menu create properties
  struct CreationOption {
    std::string title;
  };

  Menu(CreationOption options);
  virtual ~Menu();

  void SetTitle(const std::string& title);
  std::string GetTitle();
  void Append(MenuItem* menu_item);
  void Insert(MenuItem* menu_item, int pos);
  void Remove(MenuItem* menu_item);
  void Remove(int pos);

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> Append(const v8::Arguments& args);
  static v8::Handle<v8::Value> Insert(const v8::Arguments& args);
  static v8::Handle<v8::Value> Remove(const v8::Arguments& args);
  static v8::Handle<v8::Value> RemoveAt(const v8::Arguments& args);
  static v8::Handle<v8::Value> PropertyGetter(v8::Local<v8::String> property,
                                              const v8::AccessorInfo& info);
  static void PropertySetter(v8::Local<v8::String> property,
                             v8::Local<v8::Value> value,
                             const v8::AccessorInfo& info);
  static v8::Handle<v8::Value> MenuItemGetter(
      uint32_t index,
      const v8::AccessorInfo& info);

  static v8::Persistent<v8::Function> constructor_;

#if defined(OS_MACOSX)
  NSMenu* menu_;
#elif defined(TOOLKIT_GTK)
  GtkMenu* menu_;
#elif defined(OS_WIN)
  HMENU menu_;
#endif

  DISALLOW_COPY_AND_ASSIGN(Menu);
};

}  // namespace api

#endif  // CONTENT_NW_SRC_API_MENU_MENU_H_
