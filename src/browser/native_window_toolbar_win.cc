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

#include "content/nw/src/browser/native_window_toolbar_win.h"

#include "base/string16.h"
#include "content/nw/src/nw_shell.h"
#include "grit/ui_resources.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/views/controls/button/image_button.h"
#include "ui/views/controls/textfield/textfield.h"
#include "ui/views/layout/box_layout.h"

namespace nw {

const int kButtonMargin = 5;

NativeWindowToolbarWin::NativeWindowToolbarWin(content::Shell* shell)
    : shell_(shell) {
}

NativeWindowToolbarWin::~NativeWindowToolbarWin() {
}

views::View* NativeWindowToolbarWin::GetContentsView() {
  return this;
}

void NativeWindowToolbarWin::Layout() {
  int panel_width = width();
  int x = kButtonMargin;

  // Place three left buttons.
  gfx::Size sz = back_button_->GetPreferredSize();
  back_button_->SetBounds(x, (height() - sz.height()) / 2,
                          sz.width(), sz.height());
  x += sz.width() + kButtonMargin;

  sz = forward_button_->GetPreferredSize();
  forward_button_->SetBounds(x, back_button_->y(),
                             sz.width(), sz.height());
  x += sz.width() + kButtonMargin;

  sz = stop_or_refresh_button_->GetPreferredSize();
  stop_or_refresh_button_->SetBounds(x, back_button_->y(),
                                     sz.width(), sz.height());
  x += sz.width() + kButtonMargin;

  // And place devtools button as far as possible.
  sz = devtools_button_->GetPreferredSize();
  devtools_button_->SetBounds(panel_width - sz.width() - kButtonMargin,
                              back_button_->y(),
                              sz.width(),
                              sz.height());

  // Stretch url entry.
  sz = url_entry_->GetPreferredSize();
  url_entry_->SetBounds(x,
                        (height() - sz.height()) / 2,
                        devtools_button_->x() - kButtonMargin - x,
                        sz.height());
}

void NativeWindowToolbarWin::ViewHierarchyChanged(bool is_add,
                                                  views::View* parent,
                                                  views::View* child) {
  if (is_add && child == this)
    InitToolbar();
}

void NativeWindowToolbarWin::ContentsChanged(
    views::Textfield* sender,
    const string16& new_contents) {
}

bool NativeWindowToolbarWin::HandleKeyEvent(views::Textfield* sender,
                                            const ui::KeyEvent& key_event) {
  return false;
}

void NativeWindowToolbarWin::ButtonPressed(views::Button* sender,
                                           const ui::Event& event) {
}

void NativeWindowToolbarWin::InitToolbar() {
  views::BoxLayout* layout = new views::BoxLayout(
      views::BoxLayout::kHorizontal, 5, 5, 10);
  SetLayoutManager(layout);

  ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
  back_button_ = new views::ImageButton(this);
  back_button_->SetImage(views::CustomButton::BS_NORMAL,
      rb.GetNativeImageNamed(IDR_CLOSE_BAR).ToImageSkia());
  back_button_->SetImage(views::CustomButton::BS_HOT,
      rb.GetNativeImageNamed(IDR_CLOSE_BAR_H).ToImageSkia());
  back_button_->SetImage(views::CustomButton::BS_PUSHED,
      rb.GetNativeImageNamed(IDR_CLOSE_BAR_P).ToImageSkia());
  back_button_->SetAccessibleName(L"Back");
  AddChildView(back_button_);

  forward_button_ = new views::ImageButton(this);
  forward_button_->SetImage(views::CustomButton::BS_NORMAL,
      rb.GetNativeImageNamed(IDR_CLOSE_BAR).ToImageSkia());
  forward_button_->SetImage(views::CustomButton::BS_HOT,
      rb.GetNativeImageNamed(IDR_CLOSE_BAR_H).ToImageSkia());
  forward_button_->SetImage(views::CustomButton::BS_PUSHED,
      rb.GetNativeImageNamed(IDR_CLOSE_BAR_P).ToImageSkia());
  forward_button_->SetAccessibleName(L"Forward");
  AddChildView(forward_button_);

  stop_or_refresh_button_ = new views::ImageButton(this);
  stop_or_refresh_button_->SetImage(views::CustomButton::BS_NORMAL,
      rb.GetNativeImageNamed(IDR_CLOSE_BAR).ToImageSkia());
  stop_or_refresh_button_->SetImage(views::CustomButton::BS_HOT,
      rb.GetNativeImageNamed(IDR_CLOSE_BAR_H).ToImageSkia());
  stop_or_refresh_button_->SetImage(views::CustomButton::BS_PUSHED,
      rb.GetNativeImageNamed(IDR_CLOSE_BAR_P).ToImageSkia());
  stop_or_refresh_button_->SetAccessibleName(L"Stop");
  AddChildView(stop_or_refresh_button_);

  url_entry_ = new views::Textfield(views::Textfield::STYLE_DEFAULT);
  url_entry_->SetController(this);
  AddChildView(url_entry_);

  devtools_button_ = new views::ImageButton(this);
  devtools_button_->SetImage(views::CustomButton::BS_NORMAL,
      rb.GetNativeImageNamed(IDR_CLOSE_BAR).ToImageSkia());
  devtools_button_->SetImage(views::CustomButton::BS_HOT,
      rb.GetNativeImageNamed(IDR_CLOSE_BAR_H).ToImageSkia());
  devtools_button_->SetImage(views::CustomButton::BS_PUSHED,
      rb.GetNativeImageNamed(IDR_CLOSE_BAR_P).ToImageSkia());
  devtools_button_->SetAccessibleName(L"Devtools");
  AddChildView(devtools_button_);
}
  
}  // namespace nw
