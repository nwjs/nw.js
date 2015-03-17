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
  ~ShellContentClient() final;

   std::string GetUserAgent() const override;
   base::string16 GetLocalizedString(int message_id) const override;
   base::StringPiece GetDataResource(
      int resource_id,
      ui::ScaleFactor scale_factor) const override;
   base::RefCountedStaticMemory* GetDataResourceBytes(
      int resource_id) const override;
   gfx::Image& GetNativeImageNamed(int resource_id) const override;
   bool CanHandleWhileSwappedOut(const IPC::Message& msg) override;
   void AddAdditionalSchemes(
      std::vector<std::string>* standard_schemes,
      std::vector<std::string>* saveable_shemes) override;
   void AddPepperPlugins(
      std::vector<content::PepperPluginInfo>* plugins) override;
};

}  // namespace content

#endif  // CONTENT_SHELL_SHELL_CONTENT_CLIENT_H_
