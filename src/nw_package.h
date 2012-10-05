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

#ifndef CONTENT_NW_SRC_NW_PACKAGE_H
#define CONTENT_NW_SRC_NW_PACKAGE_H

#include "base/basictypes.h"
#include "base/file_path.h"
#include "base/memory/scoped_ptr.h"

#include <string>

class GURL;

namespace base {
  class DictionaryValue;
}

namespace nw {

class Package {
 public:
  // Init package from command line parameters.
  Package();

  // Init package from specifed path.
  Package(FilePath path);
  ~Package();

  // Get startup url.
  GURL GetStartupURL();

  // Return if we enable node.js.
  bool GetUseNode();

  // Root path of package.
  FilePath path() const { return path_; }

  // If the package is extracting itself.
  bool self_extract() const { return self_extract_; }

  // Manifest root.
  base::DictionaryValue* root() { return root_.get(); }

  // Window field of manifest.
  base::DictionaryValue* window();

 private:
  bool InitFromPath();
  void InitWithDefault();
  bool ExtractPath(FilePath* path);

  FilePath path_;
  bool self_extract_;
  bool initialized_;
  scoped_ptr<base::DictionaryValue> root_;

  DISALLOW_COPY_AND_ASSIGN(Package);
};

}  // namespae nw

#endif  // CONTENT_NW_SRC_NW_PACKAGE_H
