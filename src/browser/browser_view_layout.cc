// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/browser_view_layout.h"
#include "content/nw/src/common/shell_switches.h"

#include "base/logging.h"

using views::View;

namespace nw {

BrowserViewLayout::BrowserViewLayout()
  : menu_bar_(NULL), web_view_(NULL), tool_bar_(NULL)
{
}

BrowserViewLayout::~BrowserViewLayout() {}

void BrowserViewLayout::Layout(View* host) {
  if (host->children().empty())
    return;

  int y = 0;
  gfx::Size host_size = host->GetContentsBounds().size();

  if (menu_bar_) {
    menu_bar_->SetBounds(0, y, host_size.width(), kMenuHeight);
    y += kMenuHeight;
  }

  if (tool_bar_) {
    int height = tool_bar_->GetPreferredSize().height();
    tool_bar_->SetBounds(0, y, host_size.width(), height);
    y += height;
  }

  web_view_->SetBounds(0, y, host_size.width(), host_size.height() - y);
}

gfx::Size BrowserViewLayout::GetPreferredSize(const View* host) const {
  if (host->children().empty())
    return gfx::Size();

  gfx::Rect rect(web_view_->GetPreferredSize());
  rect.Inset(-host->GetInsets());
  if (menu_bar_)
    rect.Inset(gfx::Insets::TLBR(0, 0, 0, -kMenuHeight));

  if (tool_bar_)
    rect.Inset(gfx::Insets::TLBR(0, 0, 0, -tool_bar_->GetPreferredSize().height()));

  return rect.size();
}

int BrowserViewLayout::GetPreferredHeightForWidth(const View* host, int width) const {
  if (host->children().empty())
    return 0;

  const gfx::Insets insets = host->GetInsets();
  int ret = web_view_->GetHeightForWidth(width - insets.width()) +
      insets.height();

  if (menu_bar_)
    ret += kMenuHeight;

  if (tool_bar_)
    ret += tool_bar_->GetHeightForWidth(width - insets.width());

  return ret;
}

}  // namespace views
