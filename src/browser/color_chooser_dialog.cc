// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/color_chooser_dialog.h"

#include <commdlg.h>

#include "base/bind.h"
#include "base/message_loop/message_loop.h"
#include "base/threading/thread.h"
#include "content/public/browser/browser_thread.h"
#include "skia/ext/skia_utils_win.h"
#include "ui/views/color_chooser/color_chooser_listener.h"

using content::BrowserThread;

// static
COLORREF ColorChooserDialog::g_custom_colors[16];

ColorChooserDialog::ExecuteOpenParams::ExecuteOpenParams(SkColor color,
                                                         RunState run_state,
                                                         HWND owner)
    : color(color),
      run_state(run_state),
      owner(owner) {
}

ColorChooserDialog::ColorChooserDialog(views::ColorChooserListener* listener,
                                       SkColor initial_color,
                                       gfx::NativeWindow owning_window)
    : listener_(listener) {
  DCHECK(listener_);
  CopyCustomColors(g_custom_colors, custom_colors_);
  ExecuteOpenParams execute_params(initial_color, BeginRun((HWND)owning_window),
                                   (HWND)owning_window);
  execute_params.run_state.dialog_thread->message_loop()->PostTask(FROM_HERE,
      base::Bind(&ColorChooserDialog::ExecuteOpen, this, execute_params));
}

ColorChooserDialog::~ColorChooserDialog() {
}

bool ColorChooserDialog::IsRunning(gfx::NativeWindow owning_hwnd) const {
  return listener_ && IsRunningDialogForOwner((HWND)owning_hwnd);
}

void ColorChooserDialog::ListenerDestroyed() {
  // Our associated listener has gone away, so we shouldn't call back to it if
  // our worker thread returns after the listener is dead.
  listener_ = NULL;
}

void ColorChooserDialog::ExecuteOpen(const ExecuteOpenParams& params) {
  CHOOSECOLOR cc;
  cc.lStructSize = sizeof(CHOOSECOLOR);
  cc.hwndOwner = params.owner;
  cc.rgbResult = skia::SkColorToCOLORREF(params.color);
  cc.lpCustColors = custom_colors_;
  cc.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
  bool success = !!ChooseColor(&cc);
  DisableOwner(cc.hwndOwner);
  BrowserThread::PostTask(BrowserThread::UI, FROM_HERE,
      base::Bind(&ColorChooserDialog::DidCloseDialog, this, success,
                 skia::COLORREFToSkColor(cc.rgbResult), params.run_state));
}

void ColorChooserDialog::DidCloseDialog(bool chose_color,
                                        SkColor color,
                                        RunState run_state) {
  if (!listener_)
    return;
  EndRun(run_state);
  CopyCustomColors(custom_colors_, g_custom_colors);
  if (chose_color)
    listener_->OnColorChosen(color);
  listener_->OnColorChooserDialogClosed();
}

void ColorChooserDialog::CopyCustomColors(COLORREF* src, COLORREF* dst) {
  memcpy(dst, src, sizeof(COLORREF) * arraysize(g_custom_colors));
}
