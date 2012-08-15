// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_SHELL_RESOURCE_CONTEXT_H_
#define CONTENT_SHELL_SHELL_RESOURCE_CONTEXT_H_

#include "base/compiler_specific.h"
#include "base/memory/ref_counted.h"
#include "content/public/browser/resource_context.h"

namespace content {

class ShellURLRequestContextGetter;

class ShellResourceContext : public ResourceContext {
 public:
  explicit ShellResourceContext(ShellURLRequestContextGetter* getter);
  virtual ~ShellResourceContext();

 private:
  // ResourceContext implementation:
  virtual net::HostResolver* GetHostResolver() OVERRIDE;
  virtual net::URLRequestContext* GetRequestContext() OVERRIDE;

  scoped_refptr<ShellURLRequestContextGetter> getter_;

  DISALLOW_COPY_AND_ASSIGN(ShellResourceContext);
};

}  // namespace content

#endif  // CONTENT_SHELL_SHELL_RESOURCE_CONTEXT_H_
