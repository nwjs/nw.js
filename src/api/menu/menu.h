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

#include "base/compiler_specific.h"
#include "content/nw/src/api/base/base.h"

#include <string>
#include <vector>

#if defined(OS_MACOSX)
#if __OBJC__
@class NSMenu;
#else
class NSMenu;
#endif  // __OBJC__
#elif defined(TOOLKIT_GTK)
#include <gtk/gtk.h>
#elif defined(OS_WIN)
#include <windows.h>
#endif  // defined(OS_MACOSX)

namespace content {
class Shell;
}

namespace api {

class MenuItem;

class Menu : public Base {
 public:
  Menu(int id,
       DispatcherHost* dispatcher_host,
       const base::DictionaryValue& option);
  virtual ~Menu();

  virtual void Call(const std::string& method,
                    const base::ListValue& arguments) OVERRIDE;

 private:
  friend class MenuItem;
  friend class Tray;

  // Platform-independent implementations
  void Create(const base::DictionaryValue& option);
  void Destroy();
  void SetTitle(const std::string& title);
  void Append(MenuItem* menu_item);
  void Insert(MenuItem* menu_item, int pos);
  void Remove(MenuItem* menu_item, int pos_hint);
  void RemoveAt(int pos);
  void Popup(int x, int y, content::Shell*);

#if defined(OS_MACOSX)
  __block NSMenu* menu_;
#elif defined(TOOLKIT_GTK)
  GtkMenu* menu_;
#elif defined(OS_WIN)
  HMENU menu_;
#endif

  DISALLOW_COPY_AND_ASSIGN(Menu);
};

}  // namespace api

#endif  // CONTENT_NW_SRC_API_MENU_MENU_H_
