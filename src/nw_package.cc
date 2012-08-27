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
#include "base/values.h"
#include "common/zip.h"
#include "content/nw/src/shell_switches.h"

#if defined(OS_WIN)
#include "base/string16.h"
#endif

namespace {

// Don't bother with lifetime since it's used through out the program
base::DictionaryValue* g_manifest = NULL;

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

void ManifestConvertRelativePaths(
    FilePath path,
    base::DictionaryValue* manifest) {
  if (manifest->HasKey(switches::kmMain)) {
#if defined(OS_WIN)
    string16 out;
#else
    std::string out;
#endif
    if (!manifest->GetString(switches::kmMain, &out)) {
      manifest->Remove(switches::kmMain, NULL);
      LOG(WARNING) << "'main' field in manifest must be a string.";
      return;
    }

    // Don't append path if there is already a prefix
#if defined(OS_WIN)
    if (MatchPattern(out, L"*://*")) {
#else
    if (MatchPattern(out, "*://*")) {
#endif
      return;
    }


    FilePath main_path = path.Append(out);
#if defined(OS_WIN)
    string16 url(L"file://");
#else
    std::string url("file://");
#endif
    manifest->SetString(switches::kmMain, url + main_path.value());
  } else {
    LOG(WARNING) << "'main' field in manifest should be specifed.";
  }
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

bool InitPackage() {
  FilePath path;
  bool is_self_extract = false;

  CommandLine* command_line = CommandLine::ForCurrentProcess();
  const CommandLine::StringVector& args = command_line->GetArgs();

  if (args.size() == 0) {
    // See itself as a package (allowing standalone)
    path = FilePath(command_line->GetProgram());
    is_self_extract = true;
  } else {
    // Get first argument
    path = FilePath(args[0]);
    if (!file_util::PathExists(path)) {
      LOG(WARNING) << "Package does not exist.";
      return false;
    }
  }

  // Convert to absoulute path
  if (!MakePathAbsolute(&path)) {
    DLOG(ERROR) << "Cannot make absolute path from " << path.value();
  }

  // If it's a file then try to extract from it
  if (!file_util::DirectoryExists(path)) {
    DLOG(INFO) << "Extracting packaging...";
    FilePath zip_file(path);
    if (!ExtractPackage(zip_file, &path)) {
      if (!is_self_extract)
        LOG(ERROR) << "Unable to extract package.";
      return false;
    }
  }

#if defined(OS_WIN)
  FilePath manifest_path = path.Append(L"package.json");
#else
  FilePath manifest_path = path.Append("package.json");
#endif
  if (!file_util::PathExists(manifest_path)) {
    LOG(ERROR) << "No 'package.json' in package.";
    return false;
  }

  // Parse file
  std::string error;
  JSONFileValueSerializer serializer(manifest_path);
  scoped_ptr<Value> root(serializer.Deserialize(NULL, &error));
  if (!root.get()) {
    if (error.empty()) {
      LOG(ERROR) << "It's not able to read the manifest file.";
    } else {
      LOG(ERROR) << "Manifest parsing error: " << error;
    }
    return false;
  }

  if (!root->IsType(Value::TYPE_DICTIONARY)) {
    LOG(ERROR) << "Manifest file is invalid, we need a dictionary type";
    return false;
  }

  // Save result in global
  g_manifest = static_cast<DictionaryValue*>(root.release());
  // And save the root path
  g_manifest->SetString(switches::kmRoot, path.value());

  // Check fields
  const char* required_fields[] = { switches::kmMain, switches::kmName };
  for (unsigned i = 0; i < arraysize(required_fields); i++) {
    if (!g_manifest->HasKey(required_fields[i])) {
      LOG(ERROR) << "'" << required_fields[i] << "' field is required.";
      return false;
    }
  }

  ManifestConvertRelativePaths(path, g_manifest);
  return true;
}

}  // namespace

namespace nw {

base::DictionaryValue* GetManifest() {
  return g_manifest;
}

GURL GetStartupURL() {
#if defined(OS_ANDROID)
  // Delay renderer creation on Android until surface is ready.
  return GURL();
#endif

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
  base::DictionaryValue* manifest = nw::GetManifest();
  manifest->GetString(switches::kmMain, &url);

  if (url.empty())
    url = "about:blank";

  return GURL(url);
}

bool GetUseNode() {
  bool use_node = true;
  GetManifest()->GetBoolean(switches::kmNodejs, &use_node);
  return use_node;
}

void InitPackageForceNoEmpty() {
  InitPackage();

  if (g_manifest == NULL) {
    g_manifest = new base::DictionaryValue();
    g_manifest->SetString(switches::kmName, "node-webkit");
  }

  if (!g_manifest->HasKey(switches::kmWindow)) {
    base::DictionaryValue* window = new base::DictionaryValue();
    g_manifest->Set(switches::kmWindow, window);

    // Hide toolbar is specifed in the command line
    if (CommandLine::ForCurrentProcess()->HasSwitch(switches::kNoToolbar))
      window->SetBoolean(switches::kmToolbar, false);
  }
}

}  // namespace nw
