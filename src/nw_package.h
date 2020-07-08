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

#include "base/macros.h"
#include "base/files/file_path.h"
#include "base/memory/ptr_util.h"
#include "base/files/scoped_temp_dir.h"

#include "content/nw/src/nw_export.h"

#include <string>

class GURL;

namespace base {
class DictionaryValue;
class FilePath;
}

namespace gfx {
class Image;
}

namespace nw {

using base::FilePath;
class NW_EXPORT Package {
 public:
  // Init package from command line parameters.
  Package();

  // Init package from specifed path.
  Package(FilePath path);
  ~Package();

  // Convert path to absoulte path.
  FilePath ConvertToAbsoutePath(const FilePath& path);

  // Get startup url.
  GURL GetStartupURL();

  // Get app's name.
  std::string GetName();

  // Return if we enable node.js.
  bool GetUseNode();

  bool GetUseExtension();

  // Root path of package.
  FilePath path() const { return path_; }

  // If the package is extracting itself.
  bool self_extract() const { return self_extract_; }

  // Manifest root.
  base::DictionaryValue* root() { return root_.get(); }

  // Window field of manifest.
  base::DictionaryValue* window();

  // Manifest string.
  std::string package_string() { return package_string_; }

  std::string cached_error_content() { return cached_error_content_; }
  base::ScopedTempDir& temp_dir() { return scoped_temp_dir_; }

 private:
  bool InitFromPath(const base::FilePath&);
  void InitWithDefault();
  bool ExtractPath(const base::FilePath&, FilePath* out);
  bool ExtractPackage(const FilePath& zip_file, FilePath* where);

  // Read chromium command line args from the package.json if specifed.
  void ReadChromiumArgs();

  // Read js flags from the package.json if specifed.
  void ReadJsFlags();

  // Convert error info into data url.
  void ReportError(const std::string& title, const std::string& content);

  // Root path of the package.
  FilePath path_;

  // Is it a standalone and self-extractable app?
  bool self_extract_;

  // The parsed package.json.
  std::unique_ptr<base::DictionaryValue> root_;

  // The origin JSON string package.json.
  std::string package_string_;

  // Stored url for error page.
  std::string error_page_url_;

  std::string cached_error_content_;

  // Auto clean our temporary directory
  base::ScopedTempDir scoped_temp_dir_;

  DISALLOW_COPY_AND_ASSIGN(Package);
};

}  // namespae nw

#endif  // CONTENT_NW_SRC_NW_PACKAGE_H
