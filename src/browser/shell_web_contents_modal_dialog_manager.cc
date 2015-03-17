// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/web_modal/web_contents_modal_dialog_manager.h"

namespace web_modal {

SingleWebContentsDialogManager*
WebContentsModalDialogManager::CreateNativeWebModalManager(
    NativeWebContentsModalDialog dialog,
    SingleWebContentsDialogManagerDelegate* native_delegate) {
  // TODO(oshima): Investigate if we need to implement this.
  NOTREACHED();
  return NULL;
}

}  // namespace web_modal
