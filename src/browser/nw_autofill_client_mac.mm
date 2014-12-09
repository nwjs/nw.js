// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/nw_autofill_client.h"

#import <Cocoa/Cocoa.h>

#include "base/logging.h"
#include "base/mac/scoped_nsobject.h"
#include "components/autofill/core/browser/personal_data_manager.h"

namespace autofill {

void NWAutofillClient::RegisterForKeystoneNotifications() {
}

void NWAutofillClient::UnregisterFromKeystoneNotifications() {
}

}  // namespace autofill
