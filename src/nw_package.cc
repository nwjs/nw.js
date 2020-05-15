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
#include "base/environment.h"
#include "base/files/file_util.h"
#include "base/files/scoped_temp_dir.h"
#include "base/json/json_file_value_serializer.h"
#include "base/json/json_string_value_serializer.h"
#include "base/path_service.h"
#include "base/strings/pattern.h"
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
//#include "grit/nw_resources.h"
#include "media/base/media_switches.h"
#include "net/base/escape.h"
#include "third_party/node-nw/deps/uv/include/uv.h"
#include "ui/base/resource/resource_bundle.h"

#if defined(OS_WIN)
#include <windows.h>
#endif

using base::CommandLine;

namespace base {

namespace {
//const CommandLine::CharType kSwitchTerminator[] = FILE_PATH_LITERAL("--");
const CommandLine::CharType kSwitchValueSeparator[] = FILE_PATH_LITERAL("=");

// Since we use a lazy match, make sure that longer versions (like "--") are
// listed before shorter versions (like "-") of similar prefixes.
#if defined(OS_WIN)
// By putting slash last, we can control whether it is treaded as a switch
// value by changing the value of switch_prefix_count to be one less than
// the array size.
const CommandLine::CharType* const kSwitchPrefixes[] = {L"--", L"-", L"/"};
#elif defined(OS_POSIX)
// Unixes don't use slash as a switch.
const CommandLine::CharType* const kSwitchPrefixes[] = {"--", "-"};
#endif

size_t switch_prefix_count = base::size(kSwitchPrefixes);

size_t GetSwitchPrefixLength(const CommandLine::StringType& string) {
  for (size_t i = 0; i < switch_prefix_count; ++i) {
    CommandLine::StringType prefix(kSwitchPrefixes[i]);
    if (string.compare(0, prefix.length(), prefix) == 0)
      return prefix.length();
  }
  return 0;
}

}

bool IsSwitch(const CommandLine::StringType& string,
              CommandLine::StringType* switch_string,
              CommandLine::StringType* switch_value) {
  switch_string->clear();
  switch_value->clear();
  size_t prefix_length = GetSwitchPrefixLength(string);
  if (prefix_length == 0 || prefix_length == string.length())
    return false;

  const size_t equals_position = string.find(kSwitchValueSeparator);
  *switch_string = string.substr(0, equals_position);
  if (equals_position != CommandLine::StringType::npos)
    *switch_value = string.substr(equals_position + 1);
  return true;
}
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

  if (!base::PathService::Get(base::FILE_EXE, &path)) {
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
  if (base::MatchPattern(old, "*://*"))
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
    : path_(),
      self_extract_(true) {

  base::FilePath path;
  
  // 1. try to extract self
  self_extract_ = true;
  path = GetSelfPath();
  if (InitFromPath(path))
    return;

  // 2. try to load from the folder where the exe resides.
  // Note: self_extract_ is true here, otherwise a 'Invalid Package' error
  // would be triggered.
  path = GetSelfPath().DirName();
#if defined(OS_MACOSX)
  path = path.DirName().DirName().DirName();
#endif
  if (InitFromPath(path))
    return;

  // 3. try to load from <exe-folder>/package.nw
  path = path.AppendASCII("package.nw");
  if (InitFromPath(path))
    return;

  // 4. see if we have arguments and extract it.
  CommandLine* command_line = CommandLine::ForCurrentProcess();
  // LOG(INFO) << command_line->GetCommandLineString();
  const base::CommandLine::StringVector& args = command_line->GetArgs();

  // 4.1. try --nwapp= argument
  if (command_line->HasSwitch("nwapp")) {
    path = command_line->GetSwitchValuePath("nwapp");
    self_extract_ = false;
    if (InitFromPath(path))
      return;
  }

  // 4.2 try first CLI argument
  if (args.size() > 0) {
    self_extract_ = false;
    path = FilePath(args[0]);
    if (InitFromPath(path))
      return;
  }

  // 5. init with default settings
  self_extract_ = false;
  InitWithDefault();
}

Package::Package(FilePath path)
    : path_(),
      self_extract_(false) {
  if (!InitFromPath(path))
    InitWithDefault();
}

Package::~Package() {
}

FilePath Package::ConvertToAbsoutePath(const FilePath& path) {
  if (path.IsAbsolute())
    return path;

  return MakeAbsoluteFilePath(this->path()).Append(path);
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
  std::string name("nwjs");
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

bool Package::InitFromPath(const base::FilePath& path_in) {
  base::ThreadRestrictions::SetIOAllowed(true);
  FilePath extracted_path, path(path_in);
  if (!ExtractPath(path, &extracted_path))
    return false;
  if (!extracted_path.empty())
	path = extracted_path;
  // path_/package.json
  FilePath manifest_path = path.AppendASCII("package.json");
  manifest_path = MakeAbsoluteFilePath(manifest_path);
  if (!base::PathExists(manifest_path)) {
    manifest_path = path.AppendASCII("manifest.json");
    manifest_path = MakeAbsoluteFilePath(manifest_path);
    if (!base::PathExists(manifest_path)) {
      if (!self_extract())
        ReportError("Invalid package",
                    "There is no 'package.json' in the package, please make "
                    "sure the 'package.json' is in the root of the package.");
      return false;
    }
  }
  path_ = path;
  // Parse file.
  std::string error;
  JSONFileValueDeserializer serializer(manifest_path);
  std::unique_ptr<base::Value> root(serializer.Deserialize(NULL, &error));
  if (!root.get()) {
    ReportError("Unable to parse package.json",
                error.empty() ?
                    "Failed to read the manifest file: " +
                        manifest_path.AsUTF8Unsafe() :
                    error);
    return false;
  } else if (!root->is_dict()) {
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
  for (unsigned i = 0; i < base::size(required_fields); i++)
    if (!root_->HasKey(required_fields[i])) {
      ReportError("Invalid package.json",
                  std::string("Field '") + required_fields[i] + "'"
                      " is required.");
      return false;
    }

  // Force window field no empty.
  if (!root_->HasKey(switches::kmWindow)) {
    auto window =  std::make_unique<base::DictionaryValue>();
    window->SetString(switches::kmPosition, "center");
    root_->Set(switches::kmWindow, std::move(window));
  }

#if 0
  std::string bufsz_str;
  if (root_->GetString(switches::kAudioBufferSize, &bufsz_str)) {
    int buffer_size = 0;
    if (base::StringToInt(bufsz_str, &buffer_size) && buffer_size > 0) {
      base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
      command_line->AppendSwitchASCII(switches::kAudioBufferSize, bufsz_str);
    }
  }
#endif

  // Read chromium command line args.
  ReadChromiumArgs();

  // Read flags for v8 engine.
  ReadJsFlags();

  RelativePathToURI(path_, this->root());
  return true;
}

void Package::InitWithDefault() {
  root_.reset(new base::DictionaryValue());
  root()->SetString(switches::kmName, "nwjs");
  root()->SetString(switches::kmMain, "nw:blank");
  auto window = std::make_unique<base::DictionaryValue>();

  // Hide toolbar if specifed in the command line.
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kNoToolbar))
    window->SetBoolean(switches::kmToolbar, false);

  // Window should show in center by default.
  window->SetString(switches::kmPosition, "center");
  root()->Set(switches::kmWindow, std::move(window));

  ReadChromiumArgs();
}

bool Package::ExtractPath(const base::FilePath& path_to_extract, 
	                      base::FilePath* extracted_path_out) {
  base::FilePath path = path_to_extract;
  // Convert to absoulute path.
  if (!MakePathAbsolute(&path)) {
    ReportError("Cannot extract package",
                "Path is invalid: " + path.AsUTF8Unsafe());
    return false;
  }

  // Read symbolic link.
#if defined(OS_POSIX)
  FilePath target;
  if (base::ReadSymbolicLink(path, &target))
    path = target;
#endif

  // If it's a file then try to extract from it.
  if (!base::DirectoryExists(path)) {
    FilePath extracted_path;
    if (ExtractPackage(path, &extracted_path)) {
      *extracted_path_out = extracted_path;
    } else if (!self_extract()) {
      ReportError("Cannot extract package",
                  "Failed to unzip the package file: " + path.AsUTF8Unsafe());
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
    *where = scoped_temp_dir_.GetPath();
  }

  return zip::Unzip(zip_file, *where);
}

void Package::ReadChromiumArgs() {
  std::string args, env_args;
  std::unique_ptr<base::Environment> env(base::Environment::Create());

  bool got_env = env->GetVar("NW_PRE_ARGS", &env_args);
  if (!root()->HasKey(switches::kmChromiumArgs))
    if (!got_env)
      return;

  if (!root()->GetStringASCII(switches::kmChromiumArgs, &args))
    if (!got_env)
      return;

  // Expand Windows psudovars (ex; %APPDATA%) passed in chromium-args in the same way as when 
  // passed as command line parameters.
  #if defined(OS_WIN)
  TCHAR szEnvPath[MAX_PATH];
  std::wstring ws; 
  ws.assign( args.begin(), args.end());
  if (ExpandEnvironmentStrings(ws.c_str(), szEnvPath,MAX_PATH) != 0) {
    std::wstring ws_out = szEnvPath;
    args = std::string(ws_out.begin(), ws_out.end());
  } else {
    ReportError("Failed to expand chromium args",args.c_str());
  }
  #endif
  args = env_args + kChromiumArgsSeparator + args;

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
  cached_error_content_ = content;
}

}  // namespace nw
