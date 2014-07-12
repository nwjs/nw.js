// Copyright (c) 2014 Intel Corp
// Copyright (c) 2014 The Chromium Authors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell co
// pies of the Software, and to permit persons to whom the Software is furnished
// to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IM
// PLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNES
// S FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
// OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WH
// ETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef CONTENT_NW_SRC_API_SHORTCUT_SHORTCUT_H_
#define CONTENT_NW_SRC_API_SHORTCUT_SHORTCUT_H_

#include <string>

#include "content/nw/src/api/base/base.h"
#include "content/nw/src/api/shortcut/global_shortcut_listener.h"
#include "ui/base/accelerators/accelerator.h"

namespace nwapi {

class Shortcut : public Base, public GlobalShortcutListener::Observer {
 public:
  Shortcut(int id,
           const base::WeakPtr<DispatcherHost>& dispatcher_host,
           const base::DictionaryValue& option);
  virtual ~Shortcut();

  const ui::Accelerator& GetAccelerator() const {
    return accelerator_;
  }

  void OnActive();
  void OnFailed(const std::string failed_msg);

  // GlobalShortcutListener::Observer implementation.
  virtual void OnKeyPressed(const ui::Accelerator& accelerator);

 private:
  ui::Accelerator accelerator_;
};

}  // namespace nwapi

#endif  // CONTENT_NW_SRC_API_SHORTCUT_SHORTCUT_H_
