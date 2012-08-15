// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_SHELL_CONTENT_CLIENT_H_
#define CONTENT_SHELL_SHELL_CONTENT_CLIENT_H_

#include <string>
#include <vector>

#include "base/compiler_specific.h"
#include "content/public/common/content_client.h"

namespace content {

class ShellContentClient : public ContentClient {
 public:
  virtual ~ShellContentClient();

  virtual std::string GetUserAgent() const OVERRIDE;
  virtual string16 GetLocalizedString(int message_id) const OVERRIDE;
  virtual base::StringPiece GetDataResource(
      int resource_id,
      ui::ScaleFactor scale_factor) const OVERRIDE;
};

}  // namespace content

#endif  // CONTENT_SHELL_SHELL_CONTENT_CLIENT_H_
