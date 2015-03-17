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

#ifndef CONTENT_NW_SRC_BROWSER_NATIVE_WINDOW_MAC_H_
#define CONTENT_NW_SRC_BROWSER_NATIVE_WINDOW_MAC_H_

#import <Cocoa/Cocoa.h>

#include "base/mac/scoped_nsobject.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "content/nw/src/browser/native_window.h"

@class ShellNSWindow;
@class ShellToolbarDelegate;
@class NWMenuDelegate;
class SkRegion;

namespace nw {

class NativeWindowCocoa : public NativeWindow {
 public:
  explicit NativeWindowCocoa(const base::WeakPtr<content::Shell>& shell,
                             base::DictionaryValue* manifest);
  virtual ~NativeWindowCocoa();

  // NativeWindow implementation.
  virtual void Close() override;
  virtual void Move(const gfx::Rect& pos) override;
  virtual void Focus(bool focus) override;
  virtual void Show() override;
  virtual void Hide() override;
  virtual void Maximize() override;
  virtual void Unmaximize() override;
  virtual void Minimize() override;
  virtual void Restore() override;
  virtual void SetFullscreen(bool fullscreen) override;
  virtual bool IsFullscreen() override;
  virtual void SetTransparent(bool transparent) override;
  virtual void SetSize(const gfx::Size& size) override;
  virtual gfx::Size GetSize() override;
  virtual void SetMinimumSize(int width, int height) override;
  virtual void SetMaximumSize(int width, int height) override;
  virtual void SetResizable(bool resizable) override;
  virtual void SetAlwaysOnTop(bool top) override;
  virtual void SetShowInTaskbar(bool show = true) override;
  virtual void SetVisibleOnAllWorkspaces(bool all_workspaces) override;
  virtual void SetPosition(const std::string& position) override;
  virtual void SetPosition(const gfx::Point& position) override;
  virtual gfx::Point GetPosition() override;
  virtual void SetTitle(const std::string& title) override;
  virtual void FlashFrame(int count) override;
  virtual void SetBadgeLabel(const std::string& badge) override;
  virtual void SetProgressBar(double progress) override;
  virtual void SetKiosk(bool kiosk) override;
  virtual bool IsKiosk() override;
  virtual void SetMenu(nwapi::Menu* menu) override;
  virtual void ClearMenu() override;
  virtual void SetToolbarButtonEnabled(TOOLBAR_BUTTON button,
                                       bool enabled) override;
  virtual void SetToolbarUrlEntry(const std::string& url) override;
  virtual void SetToolbarIsLoading(bool loading) override;
  virtual void SetInitialFocus(bool accept_focus) override;
  virtual bool InitialFocus() override;

  // Called to handle a mouse event.
  void HandleMouseEvent(NSEvent* event);

  bool use_system_drag() const { return use_system_drag_; }
  SkRegion* draggable_region() const { return draggable_region_.get(); }

  void set_is_fullscreen(bool fullscreen) { is_fullscreen_ = fullscreen; }

  NSWindow* window() const { return window_; }

 protected:
  // NativeWindow implementation.
  virtual void AddToolbar() override;
  virtual void UpdateDraggableRegions(
      const std::vector<extensions::DraggableRegion>& regions) override;
  virtual void HandleKeyboardEvent(
      const content::NativeWebKeyboardEvent& event) override;

  void SetNonLionFullscreen(bool fullscreen);

 private:
  void InstallView();
  void UninstallView();
  void InstallDraggableRegionViews();
  void UpdateDraggableRegionsForSystemDrag(
      const std::vector<extensions::DraggableRegion>& regions,
      const extensions::DraggableRegion* draggable_area);
  void UpdateDraggableRegionsForCustomDrag(
      const std::vector<extensions::DraggableRegion>& regions);

  NSWindow* window_;

  // Delegate to the toolbar.
  base::scoped_nsobject<ShellToolbarDelegate> toolbar_delegate_;
  
  // Data for transparency
  NSColor *opaque_color_;

  bool is_fullscreen_;
  bool is_kiosk_;
  NSRect restored_bounds_;

  NSInteger attention_request_id_;  // identifier from requestUserAttention

  // Indicates whether system drag or custom drag should be used, depending on
  // the complexity of draggable regions.
  bool use_system_drag_;

  // For system drag, the whole window is draggable and the non-draggable areas
  // have to been explicitly excluded.
  std::vector<gfx::Rect> system_drag_exclude_areas_;

  // For custom drag, the whole window is non-draggable and the draggable region
  // has to been explicitly provided.
  scoped_ptr<SkRegion> draggable_region_;  // used in custom drag.

  // Mouse location since the last mouse event, in screen coordinates. This is
  // used in custom drag to compute the window movement.
  NSPoint last_mouse_location_;

  bool initial_focus_;
  bool first_show_;

  DISALLOW_COPY_AND_ASSIGN(NativeWindowCocoa);
};

}  // namespace nw

#endif  // CONTENT_NW_SRC_BROWSER_NATIVE_WINDOW_MAC_H_
