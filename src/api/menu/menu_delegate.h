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

#ifndef CONTENT_NW_SRC_API_MENU_MENU_DELEGATE_H_
#define CONTENT_NW_SRC_API_MENU_MENU_DELEGATE_H_

#include "ui/menus/simple_menu_model.h"

namespace nw {

class ObjectManager;

class MenuDelegate : public ui::SimpleMenuModel::Delegate {
 public:
  MenuDelegate(ObjectManager* object_manager);
  ~MenuDelegate() override;

   bool IsCommandIdChecked(int command_id) const override;
   bool IsCommandIdEnabled(int command_id) const override;

   bool GetAcceleratorForCommandId(
      int command_id,
      ui::Accelerator* accelerator) const override;

   bool IsItemForCommandIdDynamic(int command_id) const override;
   std::u16string GetLabelForCommandId(int command_id) const override;
   ui::ImageModel GetIconForCommandId(int command_id) const override;

   void ExecuteCommand(int command_id, int event_flags) override;

   bool HasIcon(int command_id) override;

 private:
  raw_ptr<ObjectManager> object_manager_;

};

}  // namespace nw

#endif  // CONTENT_NW_SRC_API_MENU_MENU_DELEGATE_H_
