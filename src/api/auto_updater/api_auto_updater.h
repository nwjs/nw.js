// Copyright (c) 2016 Jefry Tedjokusumo <jtg_gg@yahoo.com.sg>
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


#ifndef CONTENT_NW_SRC_API_AUTO_UPDATER_H_
#define CONTENT_NW_SRC_API_AUTO_UPDATER_H_

#include "extensions/browser/extension_function.h"

namespace extensions {
  
  class NwAutoUpdaterNativeCallSyncFunction: public NWSyncExtensionFunction {
  public:
    NwAutoUpdaterNativeCallSyncFunction();
    bool RunNWSync(base::ListValue* response, std::string* error) override;

  protected:
    ~NwAutoUpdaterNativeCallSyncFunction() override {}
    DECLARE_EXTENSION_FUNCTION("nw.AutoUpdater.NativeCallSync", UNKNOWN)
    
  private:
    DISALLOW_COPY_AND_ASSIGN(NwAutoUpdaterNativeCallSyncFunction);
  };
  
}  // namespace extensions

#endif //CONTENT_NW_SRC_API_AUTO_UPDATER_H_
