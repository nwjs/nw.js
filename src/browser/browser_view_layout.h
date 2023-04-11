// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NW_BROWSER_VIEW_LAYOUT_H_
#define NW_BROWSER_VIEW_LAYOUT_H_

#include "base/compiler_specific.h"
#include "ui/views/layout/layout_manager.h"
#include "ui/views/view.h"

namespace nw {

///////////////////////////////////////////////////////////////////////////////
//
//  copied from ui/views/layout/FillLayout
//
///////////////////////////////////////////////////////////////////////////////
class BrowserViewLayout : public views::LayoutManager {
 public:
  BrowserViewLayout();
  ~BrowserViewLayout() override;

  // Overridden from LayoutManager:
   void Layout(views::View* host) override;
   gfx::Size GetPreferredSize(const views::View* host) const override;
   int GetPreferredHeightForWidth(const views::View* host,
                                         int width) const override;

  void set_menu_bar(views::View* menu_bar) { menu_bar_ = menu_bar; }
  views::View* menu_bar() { return menu_bar_; }

  void set_web_view(views::View* web_view) { web_view_ = web_view; }
  views::View* web_view() { return web_view_; }

  void set_tool_bar(views::View* tool_bar) { tool_bar_ = tool_bar; }
  views::View* tool_bar() { return tool_bar_; }

 private:
  raw_ptr<views::View> menu_bar_;
  raw_ptr<views::View> web_view_;
  raw_ptr<views::View> tool_bar_;

};

}  // namespace nw

#endif  // NW_BROWSER_VIEW_LAYOUT_H_
