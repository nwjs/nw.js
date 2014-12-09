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

#ifndef CONTENT_NW_SRC_BROWSER_NATIVE_WINDOW_TOOLBAR_AURA_H_
#define CONTENT_NW_SRC_BROWSER_NATIVE_WINDOW_TOOLBAR_AURA_H_

#include "base/basictypes.h"
#include "content/nw/src/browser/native_window.h"
#include "ui/views/controls/button/button.h"
#include "ui/views/controls/textfield/textfield_controller.h"
#include "ui/views/widget/widget_delegate.h"

namespace content {
class Shell;
}

namespace views {
class ImageButton;
class Textfield;
}

namespace nw {

class NativeWindowToolbarAura : public views::WidgetDelegateView,
                               public views::TextfieldController,
                               public views::ButtonListener {
 public:
  explicit NativeWindowToolbarAura(content::Shell* shell);
  virtual ~NativeWindowToolbarAura();

  void SetButtonEnabled(NativeWindow::TOOLBAR_BUTTON button,
                        bool enabled);
  void SetUrlEntry(const std::string& url);
  void SetIsLoading(bool loading);

 protected:
  // Overridden from WidgetDelegateView:
  virtual views::View* GetContentsView() OVERRIDE;

  // Overridden from View:
  virtual void Layout() OVERRIDE;
  virtual void ViewHierarchyChanged(
      const ViewHierarchyChangedDetails& details) OVERRIDE;

  // Overridden from TextfieldController:
  virtual void ContentsChanged(views::Textfield* sender,
                               const base::string16& new_contents) OVERRIDE;
  virtual bool HandleKeyEvent(views::Textfield* sender,
                              const ui::KeyEvent& key_event) OVERRIDE;

  // Overridden from ButtonListener:
  virtual void ButtonPressed(views::Button* sender,
                             const ui::Event& event) OVERRIDE;

 private:
  void InitToolbar();

  content::Shell* shell_;

  views::ImageButton* back_button_;
  views::ImageButton* forward_button_;
  views::ImageButton* stop_or_refresh_button_;
  views::Textfield* url_entry_;
  views::ImageButton* devtools_button_;
  views::ImageButton* dev_reload_button_;

  DISALLOW_COPY_AND_ASSIGN(NativeWindowToolbarAura);
};

}  // namespace nw

#endif  // CONTENT_NW_SRC_BROWSER_NATIVE_WINDOW_TOOLBAR_AURA_H_
