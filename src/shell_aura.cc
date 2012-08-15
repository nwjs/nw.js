// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/shell.h"

#include "base/utf_string_conversions.h"
#include "ui/aura/desktop/desktop_screen.h"
#include "ui/aura/desktop/desktop_stacking_client.h"
#include "ui/aura/display_manager.h"
#include "ui/aura/env.h"
#include "ui/aura/root_window.h"
#include "ui/aura/window.h"
#include "ui/aura/single_display_manager.h"
#include "ui/base/accessibility/accessibility_types.h"
#include "ui/base/clipboard/clipboard.h"
#include "ui/base/event.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/gfx/screen.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/controls/textfield/textfield.h"
#include "ui/views/controls/textfield/textfield_controller.h"
#include "ui/views/controls/webview/webview.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/views/view.h"
#include "ui/views/views_delegate.h"
#include "ui/views/widget/desktop_native_widget_helper_aura.h"
#include "ui/views/widget/widget.h"
#include "ui/views/widget/widget_delegate.h"

#if defined(OS_CHROMEOS)
#include "chromeos/dbus/dbus_thread_manager.h"
#endif

namespace views {
// ViewDelegate implementation for aura content shell
class ShellViewsDelegateAura : public ViewsDelegate {
 public:
  ShellViewsDelegateAura()
    : use_transparent_windows_(false) {
    DCHECK(!ViewsDelegate::views_delegate);
    ViewsDelegate::views_delegate = this;
  }

  virtual ~ShellViewsDelegateAura() {
    ViewsDelegate::views_delegate = NULL;
  }

  void SetUseTransparentWindows(bool transparent) {
    use_transparent_windows_ = transparent;
  }

  // Overridden from ViewsDelegate:
  virtual ui::Clipboard* GetClipboard() const OVERRIDE {
    if (!clipboard_.get()) {
      clipboard_.reset(new ui::Clipboard);
    }
    return clipboard_.get();
  }

  virtual void SaveWindowPlacement(const Widget* window,
                                   const std::string& window_name,
                                   const gfx::Rect& bounds,
                                   ui::WindowShowState show_state) OVERRIDE {
  }

  virtual bool GetSavedWindowPlacement(
      const std::string& window_name,
      gfx::Rect* bounds,
      ui::WindowShowState* show_state) const OVERRIDE {
    return false;
  }

  virtual void NotifyAccessibilityEvent(
      View* view, ui::AccessibilityTypes::Event event_type) OVERRIDE {}

  virtual void NotifyMenuItemFocused(const string16& menu_name,
                                     const string16& menu_item_name,
                                     int item_index,
                                     int item_count,
                                     bool has_submenu) OVERRIDE {}
#if defined(OS_WIN)
  virtual HICON GetDefaultWindowIcon() const OVERRIDE {
    return NULL;
  }
#endif
  virtual NonClientFrameView* CreateDefaultNonClientFrameView(
      Widget* widget) OVERRIDE {
    return NULL;
  }
  virtual bool UseTransparentWindows() const OVERRIDE {
    return use_transparent_windows_;
  }
  virtual void AddRef() OVERRIDE {}
  virtual void ReleaseRef() OVERRIDE {}

  virtual int GetDispositionForEvent(int event_flags) OVERRIDE {
    return 0;
  }

  virtual views::NativeWidgetHelperAura* CreateNativeWidgetHelper(
      views::NativeWidgetAura* native_widget) OVERRIDE {
    return new views::DesktopNativeWidgetHelperAura(native_widget);
  }

  virtual content::WebContents* CreateWebContents(
      content::BrowserContext* browser_context,
      content::SiteInstance* site_instance) OVERRIDE {
    return NULL;
  }

 private:
  mutable scoped_ptr<ui::Clipboard> clipboard_;
  bool use_transparent_windows_;

  DISALLOW_COPY_AND_ASSIGN(ShellViewsDelegateAura);
};

// Maintain the UI controls and web view for content shell
class ShellWindowDelegateView : public WidgetDelegateView,
                                public TextfieldController,
                                public ButtonListener {
 public:
  enum UIControl {
    BACK_BUTTON,
    FORWARD_BUTTON,
    STOP_BUTTON
  };

  ShellWindowDelegateView(content::Shell* shell)
    : shell_(shell),
      toolbar_view_(new View),
      contents_view_(new View) {
  }
  virtual ~ShellWindowDelegateView() {}

  // Update the state of UI controls
  void SetAddressBarURL(const GURL& url) {
    url_entry_->SetText(ASCIIToUTF16(url.spec()));
  }
  void SetWebContents(content::WebContents* web_contents) {
    contents_view_->SetLayoutManager(new FillLayout());
    web_view_ = new WebView(web_contents->GetBrowserContext());
    web_view_->SetWebContents(web_contents);
    web_contents->Focus();
    contents_view_->AddChildView(web_view_);
    Layout();
  }
  void SetWindowTitle(const string16& title) { title_ = title; }
  void EnableUIControl(UIControl control, bool is_enabled) {
    if (control == BACK_BUTTON) {
      back_button_->SetState(is_enabled ? CustomButton::BS_NORMAL
          : CustomButton::BS_DISABLED);
    } else if (control == FORWARD_BUTTON) {
      forward_button_->SetState(is_enabled ? CustomButton::BS_NORMAL
          : CustomButton::BS_DISABLED);
    } else if (control == STOP_BUTTON) {
      stop_button_->SetState(is_enabled ? CustomButton::BS_NORMAL
          : CustomButton::BS_DISABLED);
    }
  }

 private:
  // Initialize the UI control contained in shell window
  void InitShellWindow() {
    set_background(Background::CreateStandardPanelBackground());

    GridLayout* layout = new GridLayout(this);
    SetLayoutManager(layout);

    ColumnSet* column_set = layout->AddColumnSet(0);
    column_set->AddPaddingColumn(0, 2);
    column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
                          GridLayout::USE_PREF, 0, 0);
    column_set->AddPaddingColumn(0, 2);

    layout->AddPaddingRow(0, 2);

    // Add toolbar buttons and URL text field
    {
      layout->StartRow(0, 0);
      GridLayout* toolbar_layout = new GridLayout(toolbar_view_);
      toolbar_view_->SetLayoutManager(toolbar_layout);

      ColumnSet* toolbar_column_set =
          toolbar_layout->AddColumnSet(0);
      // Back button
      back_button_ = new NativeTextButton(this, ASCIIToUTF16("Back"));
      gfx::Size back_button_size = back_button_->GetPreferredSize();
      toolbar_column_set->AddColumn(GridLayout::CENTER,
                                    GridLayout::CENTER, 0,
                                    GridLayout::FIXED,
                                    back_button_size.width(),
                                    back_button_size.width() / 2);
      // Forward button
      forward_button_ = new NativeTextButton(this, ASCIIToUTF16("Forward"));
      gfx::Size forward_button_size = forward_button_->GetPreferredSize();
      toolbar_column_set->AddColumn(GridLayout::CENTER,
                                    GridLayout::CENTER, 0,
                                    GridLayout::FIXED,
                                    forward_button_size.width(),
                                    forward_button_size.width() / 2);
      // Refresh button
      refresh_button_ = new NativeTextButton(this, ASCIIToUTF16("Refresh"));
      gfx::Size refresh_button_size = refresh_button_->GetPreferredSize();
      toolbar_column_set->AddColumn(GridLayout::CENTER,
                                    GridLayout::CENTER, 0,
                                    GridLayout::FIXED,
                                    refresh_button_size.width(),
                                    refresh_button_size.width() / 2);
      // Stop button
      stop_button_ = new NativeTextButton(this, ASCIIToUTF16("Stop"));
      gfx::Size stop_button_size = stop_button_->GetPreferredSize();
      toolbar_column_set->AddColumn(GridLayout::CENTER,
                                    GridLayout::CENTER, 0,
                                    GridLayout::FIXED,
                                    stop_button_size.width(),
                                    stop_button_size.width() / 2);
      toolbar_column_set->AddPaddingColumn(0, 2);
      // URL entry
      url_entry_ = new Textfield();
      url_entry_->SetController(this);
      toolbar_column_set->AddColumn(GridLayout::FILL,
                                    GridLayout::FILL, 1,
                                    GridLayout::USE_PREF, 0, 0);

      // Fill up the first row
      toolbar_layout->StartRow(0, 0);
      toolbar_layout->AddView(back_button_);
      toolbar_layout->AddView(forward_button_);
      toolbar_layout->AddView(refresh_button_);
      toolbar_layout->AddView(stop_button_);
      toolbar_layout->AddView(url_entry_);

      layout->AddView(toolbar_view_);
    }

    layout->AddPaddingRow(0, 5);

    // Add web contents view as the second row
    {
      layout->StartRow(1, 0);
      layout->AddView(contents_view_);
    }

    layout->AddPaddingRow(0, 5);
  }
  // Overriden from TextfieldController
  virtual void ContentsChanged(Textfield* sender,
                               const string16& new_contents) OVERRIDE {
  }
  virtual bool HandleKeyEvent(Textfield* sender,
                              const ui::KeyEvent& key_event) OVERRIDE {
   if (sender == url_entry_ && key_event.key_code() == ui::VKEY_RETURN) {
     std::string text = UTF16ToUTF8(url_entry_->text());
     GURL url(text);
     if (!url.has_scheme()) {
       url = GURL(std::string("http://") + std::string(text));
       url_entry_->SetText(ASCIIToUTF16(url.spec()));
     }
     shell_->LoadURL(url);
     return true;
   }
   return false;
  }

  // Overriden from ButtonListener
  virtual void ButtonPressed(Button* sender, const ui::Event& event) OVERRIDE {
    if (sender == back_button_)
      shell_->GoBackOrForward(-1);
    else if (sender == forward_button_)
      shell_->GoBackOrForward(1);
    else if (sender == refresh_button_)
      shell_->Reload();
    else if (sender == stop_button_)
      shell_->Stop();
  }

  // Overriden from WidgetDelegateView
  virtual bool CanResize() const OVERRIDE { return true; }
  virtual bool CanMaximize() const OVERRIDE { return true; }
  virtual string16 GetWindowTitle() const OVERRIDE {
    return title_;
  }
  virtual void WindowClosing() OVERRIDE {
    if (shell_) delete shell_;
  }
  virtual View* GetContentsView() OVERRIDE { return this; }

  // Overriden from View
  virtual void ViewHierarchyChanged(bool is_add,
                                    View* parent,
                                    View* child) OVERRIDE {
    if (is_add && child == this) {
      InitShellWindow();
    }
  }

 private:
  // Hold a reference of Shell for deleting it when the window is closing
  content::Shell* shell_;

  // Window title
  string16 title_;

  // Toolbar view contains forward/backward/reload button and URL entry
  View* toolbar_view_;
  NativeTextButton* back_button_;
  NativeTextButton* forward_button_;
  NativeTextButton* refresh_button_;
  NativeTextButton* stop_button_;
  Textfield* url_entry_;

  // Contents view contains the web contents view
  View* contents_view_;
  WebView* web_view_;

  DISALLOW_COPY_AND_ASSIGN(ShellWindowDelegateView);
};

}  // namespace views

namespace content {

using views::ShellWindowDelegateView;
using views::ShellViewsDelegateAura;

aura::client::StackingClient* Shell::stacking_client_ = NULL;
views::ViewsDelegate* Shell::views_delegate_ = NULL;

// static
void Shell::PlatformInitialize() {
#if defined(OS_CHROMEOS)
  chromeos::DBusThreadManager::Initialize();
#endif
  aura::Env::GetInstance()->SetDisplayManager(new aura::SingleDisplayManager);
  stacking_client_ = new aura::DesktopStackingClient();
  gfx::Screen::SetInstance(aura::CreateDesktopScreen());
  views_delegate_ = new ShellViewsDelegateAura();
}

void Shell::PlatformExit() {
#if defined(OS_CHROMEOS)
  chromeos::DBusThreadManager::Shutdown();
#endif
  if (stacking_client_)
    delete stacking_client_;
  if (views_delegate_)
    delete views_delegate_;
  aura::Env::DeleteInstance();
}

void Shell::PlatformCleanUp() {
}

void Shell::PlatformEnableUIControl(UIControl control, bool is_enabled) {
  ShellWindowDelegateView* delegate_view =
    static_cast<ShellWindowDelegateView*>(window_widget_->widget_delegate());
  if (control == BACK_BUTTON) {
    delegate_view->EnableUIControl(ShellWindowDelegateView::BACK_BUTTON,
        is_enabled);
  } else if (control == FORWARD_BUTTON) {
    delegate_view->EnableUIControl(ShellWindowDelegateView::FORWARD_BUTTON,
        is_enabled);
  } else if (control == STOP_BUTTON) {
    delegate_view->EnableUIControl(ShellWindowDelegateView::STOP_BUTTON,
        is_enabled);
  }
}

void Shell::PlatformSetAddressBarURL(const GURL& url) {
  ShellWindowDelegateView* delegate_view =
    static_cast<ShellWindowDelegateView*>(window_widget_->widget_delegate());
  delegate_view->SetAddressBarURL(url);
}

void Shell::PlatformSetIsLoading(bool loading) {
}

void Shell::PlatformCreateWindow(int width, int height) {
  window_widget_ =
      views::Widget::CreateWindowWithBounds(new ShellWindowDelegateView(this),
               gfx::Rect(0, 0, width, height));
  window_ = window_widget_->GetNativeWindow();
  window_widget_->Show();
}

void Shell::PlatformSetContents() {
  ShellWindowDelegateView* delegate_view =
    static_cast<ShellWindowDelegateView*>(window_widget_->widget_delegate());
  delegate_view->SetWebContents(web_contents_.get());
}

void Shell::PlatformResizeSubViews() {
}

void Shell::Close() {
  window_widget_->Close();
}

void Shell::PlatformSetTitle(const string16& title) {
  ShellWindowDelegateView* delegate_view =
    static_cast<ShellWindowDelegateView*>(window_widget_->widget_delegate());
  delegate_view->SetWindowTitle(title);
  window_widget_->UpdateWindowTitle();
}

}  // namespace content
