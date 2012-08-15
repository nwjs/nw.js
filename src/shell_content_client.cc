// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/shell_content_client.h"

#include "base/string_piece.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/resource/resource_bundle.h"
#include "webkit/glue/user_agent.h"

namespace content {

ShellContentClient::~ShellContentClient() {
}

std::string ShellContentClient::GetUserAgent() const {
  // The "19" is so that sites that sniff for version think that this is
  // something reasonably current; the "77.34.5" is a hint that this isn't a
  // standard Chrome.
  return webkit_glue::BuildUserAgentFromProduct("Chrome/19.77.34.5");
}

string16 ShellContentClient::GetLocalizedString(int message_id) const {
  return l10n_util::GetStringUTF16(message_id);
}

base::StringPiece ShellContentClient::GetDataResource(
    int resource_id,
    ui::ScaleFactor scale_factor) const {
  return ResourceBundle::GetSharedInstance().GetRawDataResource(
      resource_id, scale_factor);
}

}  // namespace content
