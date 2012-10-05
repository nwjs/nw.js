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

#include "base/command_line.h"
#include "base/file_util.h"
#include "base/json/json_file_value_serializer.h"
#include "base/logging.h"
#include "base/scoped_temp_dir.h"
#include "base/string_util.h"
#include "base/string16.h"
#include "base/threading/thread_restrictions.h"
#include "base/values.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/common/zip.h"
#include "googleurl/src/gurl.h"

namespace nw {

namespace {

bool MakePathAbsolute(FilePath* file_path) {
  DCHECK(file_path);

  FilePath current_directory;
  if (!file_util::GetCurrentDirectory(&current_directory))
    return false;

  if (file_path->IsAbsolute())
    return true;

  if (current_directory.empty())
    return file_util::AbsolutePath(file_path);

  if (!current_directory.IsAbsolute())
    return false;

  *file_path = current_directory.Append(*file_path);
  return true;
}

FilePath GetPathFromCommandLine(bool* self_extract) {
  FilePath path;
  CommandLine* command_line = CommandLine::ForCurrentProcess();
  const CommandLine::StringVector& args = command_line->GetArgs();

  if (args.size() == 0) {
    *self_extract = true;
#if defined(OS_MACOSX)
    // Find if we have node-webkit.app/Resources/app.nw.
    path = path.DirName().DirName().Append("Resources").Append("app.nw");
#else
    // See itself as a package (allowing standalone).
    path = FilePath(command_line->GetProgram());
#endif
  } else {
    *self_extract = false;
    // Get first argument.
    path = FilePath(args[0]);
  }

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

bool ExtractPackage(const FilePath& zip_file, FilePath* where) {
  // Auto clean our temporary directory
  static scoped_ptr<ScopedTempDir> scoped_temp_dir;

#if defined(OS_WIN)
  if (!file_util::CreateNewTempDirectory(L"nw", where)) {
#else
  if (!file_util::CreateNewTempDirectory("nw", where)) {
#endif
    LOG(ERROR) << "Unable to create temporary directory.";
    return false;
  }

  scoped_temp_dir.reset(new ScopedTempDir());
  if (!scoped_temp_dir->Set(*where)) {
    LOG(ERROR) << "Unable to set temporary directory.";
    return false;
  }

  return zip::Unzip(zip_file, *where);
}

}  // namespace

Package::Package()
    : path_(GetPathFromCommandLine(&self_extract_)) {
  if (!InitFromPath())
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

GURL Package::GetStartupURL() {
  std::string url; 
  // Specify URL in --url
  CommandLine* command_line = CommandLine::ForCurrentProcess();
  if (command_line->HasSwitch(switches::kUrl)) {
    url = command_line->GetSwitchValueASCII(switches::kUrl);
    GURL gurl(url);
    if (!gurl.has_scheme())
      return GURL(std::string("http://") + url);

    return gurl;
  }

  // Read from manifest
  if (root()->GetString(switches::kmMain, &url))
    return GURL(url);
  else
    return GURL("nw:blank");
}

bool Package::GetUseNode() {
  bool use_node = true;
  root()->GetBoolean(switches::kmNodejs, &use_node);
  return use_node;
}

base::DictionaryValue* Package::window() {
  base::DictionaryValue* window;
  root()->GetDictionaryWithoutPathExpansion(switches::kmWindow, &window);
  return window;
}

bool Package::InitFromPath() {
  base::ThreadRestrictions::SetIOAllowed(true);

  if (!ExtractPath(&path_))
    return false;

  file_util::SetCurrentDirectory(path_);

  // path_/package.json
  FilePath manifest_path = path_.AppendASCII("package.json");
  if (!file_util::PathExists(manifest_path)) {
    if (!self_extract())
      LOG(ERROR) << "No 'package.json' in package.";
    return false;
  }

  // Parse file.
  std::string error;
  JSONFileValueSerializer serializer(manifest_path);
  scoped_ptr<Value> root(serializer.Deserialize(NULL, &error));
  if (!root.get()) {
    if (error.empty())
      LOG(ERROR) << "It's not able to read the manifest file.";
    else
      LOG(ERROR) << "Manifest parsing error: " << error;
    return false;
  } else if (!root->IsType(Value::TYPE_DICTIONARY)) {
    LOG(ERROR) << "Manifest file is invalid, we need a dictionary type";
    return false;
  }

  // Save result in global
  root_.reset(static_cast<DictionaryValue*>(root.release()));

  // Check fields
  const char* required_fields[] = {
    switches::kmMain,
    switches::kmName
  };
  for (unsigned i = 0; i < arraysize(required_fields); i++)
    if (!root_->HasKey(required_fields[i])) {
      LOG(ERROR) << "'" << required_fields[i] << "' field is required.";
      return false;
    }

  // Force window field no empty
  if (!root_->HasKey(switches::kmWindow))
    root_->Set(switches::kmWindow, new base::DictionaryValue());

  RelativePathToURI(path_, this->root());
  return true;
}

void Package::InitWithDefault() {
  root_.reset(new base::DictionaryValue());
  root()->SetString(switches::kmName, "node-webkit");
  root()->SetString(switches::kmMain, "nw:blank");
  base::DictionaryValue* window = new base::DictionaryValue();
  root()->Set(switches::kmWindow, window);

  // Hide toolbar is specifed in the command line
  if (CommandLine::ForCurrentProcess()->HasSwitch(switches::kNoToolbar))
    window->SetBoolean(switches::kmToolbar, false);
}

bool Package::ExtractPath(FilePath* path) {
  // Convert to absoulute path.
  if (!MakePathAbsolute(&path_)) {
    LOG(ERROR) << "Cannot make absolute path from " << path_.value();
    return false;
  }

  // If it's a file then try to extract from it.
  if (!file_util::DirectoryExists(path_)) {
    DLOG(INFO) << "Extracting packaging...";
    FilePath extracted_path;
    if (ExtractPackage(path_, &extracted_path)) {
      path_ = extracted_path;
    } else if (!self_extract()) {
      LOG(ERROR) << "Unable to extract package.";
      return false;
    }
  }

  return true;
}

}  // namespace nw
