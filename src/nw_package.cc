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

#include "content/nw/src/nw_package.h"

#include <vector>

#include "base/command_line.h"
#include "base/files/file_util.h"
#include "base/files/scoped_temp_dir.h"
#include "base/json/json_file_value_serializer.h"
#include "base/json/json_string_value_serializer.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_tokenizer.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/thread_restrictions.h"
#include "base/values.h"
#include "third_party/zlib/google/zip.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/public/common/content_switches.h"
#include "url/gurl.h"
#include "grit/nw_resources.h"
#include "media/base/media_switches.h"
#include "net/base/escape.h"
#include "third_party/node/deps/uv/include/uv.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/gfx/image/image.h"
#include "ui/gfx/image/image_skia_rep.h"
#include "ui/gfx/codec/png_codec.h"

namespace base {
bool IsSwitch(const CommandLine::StringType& string,
              CommandLine::StringType* switch_string,
              CommandLine::StringType* switch_value);
}

namespace nw {

// Separator for string of |chromium_args| from |manifest|.
const char kChromiumArgsSeparator[] = " ";

namespace {

#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif

bool MakePathAbsolute(FilePath* file_path) {
  DCHECK(file_path);

  FilePath current_directory;
  if (!base::GetCurrentDirectory(&current_directory))
    return false;

  if (file_path->IsAbsolute())
    return true;

  if (current_directory.empty()) {
    *file_path = MakeAbsoluteFilePath(*file_path);
    return true;
  }

  if (!current_directory.IsAbsolute())
    return false;

  *file_path = current_directory.Append(*file_path);
  return true;
}

FilePath GetSelfPath() {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();

  FilePath path;

  size_t size = 2*PATH_MAX;
  char* execPath = new char[size];
  if (uv_exepath(execPath, &size) == 0) {
    path = FilePath::FromUTF8Unsafe(std::string(execPath, size));
  } else {
    path = FilePath(command_line->GetProgram());
  }

#if defined(OS_MACOSX)
  // Find if we have node-webkit.app/Resources/app.nw.
  path = path.DirName().DirName().Append("Resources").Append("app.nw");
#endif

  return path;
}

void RelativePathToURI(FilePath root, base::DictionaryValue* manifest) {
  std::string old;
  if (!manifest->GetString(switches::kmMain, &old))
    return;

  // Don't append path if there is already a prefix
  if (MatchPattern(old, "*://*"))
    return;

  FilePath main_path = root.Append(FilePath::FromUTF8Unsafe(old));
  manifest->SetString(switches::kmMain,
                      std::string("file://") + main_path.AsUTF8Unsafe());
}

#if defined(OS_WIN)
std::wstring ASCIIToWide(const std::string& ascii) {
  DCHECK(base::IsStringASCII(ascii)) << ascii;
  return std::wstring(ascii.begin(), ascii.end());
}
#endif

}  // namespace

Package::Package()
    : path_(GetSelfPath()),
      self_extract_(true) {
  // First try to extract self.
  if (InitFromPath())
    return;

  // Try to load from the folder where the exe resides.
  // Note: self_extract_ is true here, otherwise a 'Invalid Package' error
  // would be triggered.
  path_ = GetSelfPath().DirName();
#if defined(OS_MACOSX)
  path_ = path_.DirName().DirName().DirName();
#endif
  if (InitFromPath())
    return;

  path_ = path_.AppendASCII("package.nw");
  if (InitFromPath())
    return;

  // Then see if we have arguments and extract it.
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  const base::CommandLine::StringVector& args = command_line->GetArgs();
  if (args.size() > 0) {
    self_extract_ = false;
    path_ = FilePath(args[0]);
  }
  if (InitFromPath())
    return;

  // Finally we init with default settings.
  self_extract_ = false;
  InitWithDefault();
}

Package::Package(FilePath path)
    : path_(path),
      self_extract_(false) {
  if (!InitFromPath())
    InitWithDefault();
}

Package::~Package() {
}

FilePath Package::ConvertToAbsoutePath(const FilePath& path) {
  if (path.IsAbsolute())
    return path;

  return this->path().Append(path);
}

bool Package::GetImage(const FilePath& icon_path, gfx::Image* image) {
  FilePath path = ConvertToAbsoutePath(icon_path);

  // Read the file from disk.
  std::string file_contents;
  if (path.empty() || !base::ReadFileToString(path, &file_contents))
    return false;

  // Decode the bitmap using WebKit's image decoder.
  const unsigned char* data =
      reinterpret_cast<const unsigned char*>(file_contents.data());
  scoped_ptr<SkBitmap> decoded(new SkBitmap());
  // Note: This class only decodes bitmaps from extension resources. Chrome
  // doesn't (for security reasons) directly load extension resources provided
  // by the extension author, but instead decodes them in a separate
  // locked-down utility process. Only if the decoding succeeds is the image
  // saved from memory to disk and subsequently used in the Chrome UI.
  // Chrome is therefore decoding bitmaps here that were generated by Chrome.
  gfx::PNGCodec::Decode(data, file_contents.length(), decoded.get());
  if (decoded->empty())
    return false;  // Unable to decode.

  *image = gfx::Image::CreateFrom1xBitmap(*decoded);
  return true;
}

GURL Package::GetStartupURL() {
  std::string url;
  // Specify URL in --url
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  if (command_line->HasSwitch(switches::kUrl)) {
    url = command_line->GetSwitchValueASCII(switches::kUrl);
    GURL gurl(url);
    if (!gurl.has_scheme())
      return GURL(std::string("http://") + url);

    return gurl;
  }

  // Report if encountered errors.
  if (!error_page_url_.empty())
    return GURL(error_page_url_);

  // Read from manifest.
  if (root()->GetString(switches::kmMain, &url)) {
    VLOG(1) << "Package startup URL: " << GURL(url);
    return GURL(url);
  }else
    return GURL("nw:blank");
}

std::string Package::GetName() {
  std::string name("node-webkit");
  root()->GetString(switches::kmName, &name);
  return name;
}

bool Package::GetUseNode() {
  bool use_node = true;
  root()->GetBoolean(switches::kNodejs, &use_node);
  return use_node;
}

bool Package::GetUseExtension() {
  bool use_ext = true;
  root()->GetBoolean(switches::kChromeExtension, &use_ext);
  return use_ext;
}

base::DictionaryValue* Package::window() {
  base::DictionaryValue* window;
  root()->GetDictionaryWithoutPathExpansion(switches::kmWindow, &window);
  return window;
}

bool Package::InitFromPath() {
  base::ThreadRestrictions::SetIOAllowed(true);

  if (!ExtractPath())
    return false;

  // path_/package.json
  FilePath manifest_path = path_.AppendASCII("package.json");
  manifest_path = MakeAbsoluteFilePath(manifest_path);
  if (!base::PathExists(manifest_path)) {
    if (!self_extract())
      ReportError("Invalid package",
                  "There is no 'package.json' in the package, please make "
                  "sure the 'package.json' is in the root of the package.");
    return false;
  }

  // Parse file.
  std::string error;
  JSONFileValueSerializer serializer(manifest_path);
  scoped_ptr<base::Value> root(serializer.Deserialize(NULL, &error));
  if (!root.get()) {
    ReportError("Unable to parse package.json",
                error.empty() ?
                    "Failed to read the manifest file: " +
                        manifest_path.AsUTF8Unsafe() :
                    error);
    return false;
  } else if (!root->IsType(base::Value::TYPE_DICTIONARY)) {
    ReportError("Invalid package.json",
                "package.json's content should be a object type.");
    return false;
  }

  // Save result in global
  root_.reset(static_cast<base::DictionaryValue*>(root.release()));

  // Save origin package info
  // Since we will change some value in root_,
  // We need to catch the origin value of package.json
  package_string_ = "";
  JSONStringValueSerializer stringSerializer(&package_string_);
  stringSerializer.Serialize(*root_);

  // Check fields
  const char* required_fields[] = {
    switches::kmName
  };
  for (unsigned i = 0; i < arraysize(required_fields); i++)
    if (!root_->HasKey(required_fields[i])) {
      ReportError("Invalid package.json",
                  std::string("Field '") + required_fields[i] + "'"
                      " is required.");
      return false;
    }

  // Force window field no empty.
  if (!root_->HasKey(switches::kmWindow)) {
    base::DictionaryValue* window = new base::DictionaryValue();
    window->SetString(switches::kmPosition, "center");
    root_->Set(switches::kmWindow, window);
  }

  std::string bufsz_str;
  if (root_->GetString(switches::kAudioBufferSize, &bufsz_str)) {
    int buffer_size = 0;
    if (base::StringToInt(bufsz_str, &buffer_size) && buffer_size > 0) {
      base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
      command_line->AppendSwitchASCII(switches::kAudioBufferSize, bufsz_str);
    }
  }

  // Read chromium command line args.
  ReadChromiumArgs();

  // Read flags for v8 engine.
  ReadJsFlags();

  RelativePathToURI(path_, this->root());
  return true;
}

void Package::InitWithDefault() {
  root_.reset(new base::DictionaryValue());
  root()->SetString(switches::kmName, "node-webkit");
  root()->SetString(switches::kmMain, "nw:blank");
  base::DictionaryValue* window = new base::DictionaryValue();
  root()->Set(switches::kmWindow, window);

  // Hide toolbar if specifed in the command line.
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kNoToolbar))
    window->SetBoolean(switches::kmToolbar, false);

  // Window should show in center by default.
  window->SetString(switches::kmPosition, "center");
}

bool Package::ExtractPath() {
  // Convert to absoulute path.
  if (!MakePathAbsolute(&path_)) {
    ReportError("Cannot extract package",
                "Path is invalid: " + path_.AsUTF8Unsafe());
    return false;
  }

  // Read symbolic link.
#if defined(OS_POSIX)
  FilePath target;
  if (base::ReadSymbolicLink(path_, &target))
    path_ = target;
#endif

  // If it's a file then try to extract from it.
  if (!base::DirectoryExists(path_)) {
    FilePath extracted_path;
    if (ExtractPackage(path_, &extracted_path)) {
      path_ = extracted_path;
    } else if (!self_extract()) {
      ReportError("Cannot extract package",
                  "Failed to unzip the package file: " + path_.AsUTF8Unsafe());
      return false;
    }
  }

  return true;
}

bool Package::ExtractPackage(const FilePath& zip_file, FilePath* where) {

  if (!scoped_temp_dir_.IsValid()) {
#if defined(OS_WIN)
    if (!base::CreateNewTempDirectory(L"nw", where)) {
#else
    if (!base::CreateNewTempDirectory("nw", where)) {
#endif
      ReportError("Cannot extract package",
                  "Unable to create temporary directory.");
      return false;
    }
    if (!scoped_temp_dir_.Set(*where)) {
      ReportError("Cannot extract package",
                  "Unable to set temporary directory.");
      return false;
    }
  }else{
    *where = scoped_temp_dir_.path();
  }

  return zip::Unzip(zip_file, *where);
}

void Package::ReadChromiumArgs() {
  if (!root()->HasKey(switches::kmChromiumArgs))
    return;

  std::string args;
  if (!root()->GetStringASCII(switches::kmChromiumArgs, &args))
    return;

  std::vector<std::string> chromium_args;
  base::StringTokenizer tokenizer(args, kChromiumArgsSeparator);
  tokenizer.set_quote_chars("\'");
  while (tokenizer.GetNext()) {
    std::string token = tokenizer.token();
    base::RemoveChars(token, "\'", &token);
    chromium_args.push_back(token);
  }

  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();

  for (unsigned i = 0; i < chromium_args.size(); ++i) {
    base::CommandLine::StringType key, value;
#if defined(OS_WIN)
    // Note:: On Windows, the |CommandLine::StringType| will be |std::wstring|,
    // so the chromium_args[i] is not compatible. We convert the wstring to
    // string here is safe beacuse we use ASCII only.
    if (!base::IsSwitch(ASCIIToWide(chromium_args[i]), &key, &value))
      continue;
    command_line->AppendSwitchASCII(base::UTF16ToASCII(key),
                                    base::UTF16ToASCII(value));
#else
    if (!base::IsSwitch(chromium_args[i], &key, &value))
      continue;
    command_line->AppendSwitchASCII(key, value);
#endif
  }
}

void Package::ReadJsFlags() {
  if (!root()->HasKey(switches::kmJsFlags))
    return;

  std::string flags;
  if (!root()->GetStringASCII(switches::kmJsFlags, &flags))
    return;

  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  command_line->AppendSwitchASCII("js-flags", flags);
}

void Package::ReportError(const std::string& title,
                          const std::string& content) {
  if (!error_page_url_.empty())
    return;

  const base::StringPiece template_html(
      ResourceBundle::GetSharedInstance().GetRawDataResource(
          IDR_NW_PACKAGE_ERROR));

  if (template_html.empty()) {
    // Print hand written error info if nw.pak doesn't exist.
    NOTREACHED() << "Unable to load error template.";
    error_page_url_ = "data:text/html;base64,VW5hYmxlIHRvIGZpbmQgbncucGFrLgo=";
    return;
  }

  std::vector<std::string> subst;
  subst.push_back(title);
  subst.push_back(content);
  error_page_url_ = "data:text/html;charset=utf-8," + 
      net::EscapeQueryParamValue(
          ReplaceStringPlaceholders(template_html, subst, NULL), false);
}

}  // namespace nw
