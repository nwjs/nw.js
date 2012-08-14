// Copyright (c) 2010 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>

#include "base/file_util.h"
#include "base/json/json_file_value_serializer.h"
#include "base/logging.h"
#include "base/scoped_temp_dir.h"
#include "base/string_util.h"
#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/cef_frame.h"
#include "include/cef_runnable.h"
#include "include/cef_web_plugin.h"
#include "client_handler.h"
#include "client_switches.h"
#include "common/zip.h"
#include "nw.h"
#include "string_util.h"
#include "third_party/node/src/node_version.h"
#include "util.h"
#include "version.h"

#if defined(OS_WIN)
#include "base/string16.h"
#endif

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

void ManifestConvertRelativePaths(
    FilePath path,
    base::DictionaryValue* manifest) {
  if (manifest->HasKey(nw::kmMain)) {
#if defined(OS_WIN)
    string16 out;
#else
    std::string out;
#endif
    if (!manifest->GetString(nw::kmMain, &out)) {
      manifest->Remove(nw::kmMain, NULL);
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
    manifest->SetString(nw::kmMain, url + main_path.value());
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

void PrintVersionAndExit() {
  printf("nw %s\nnode %s\n", NW_VERSION, NODE_VERSION);
  exit(0);
}

}  // namespace

CefRefPtr<ClientHandler> g_handler;
CefRefPtr<CefCommandLine> g_command_line;

// Don't bother with lifetime since it's used through out the program
base::DictionaryValue* g_manifest = NULL;
base::DictionaryValue* g_features = NULL;

CefRefPtr<CefBrowser> AppGetBrowser() {
  if (!g_handler.get())
    return NULL;
  return g_handler->GetBrowser();
}

CefWindowHandle AppGetMainHwnd() {
  if (!g_handler.get())
    return NULL;
  return g_handler->GetMainHwnd();
}

bool AppInitManifest() {
  FilePath path;
  bool is_self_extract = false;
  if (!g_command_line->HasArguments()) {
    // See itself as a package (allowing standalone)
    path = FilePath(g_command_line->GetProgram());
    is_self_extract = true;
  } else {
    // Get first argument
    CefCommandLine::ArgumentList arguments;
    g_command_line->GetArguments(arguments);
    path = FilePath(arguments[0]);

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
  ManifestConvertRelativePaths(path, g_manifest);

  // And save the root path
  g_manifest->SetString(nw::kmRoot, path.value());

  g_manifest->GetDictionary(nw::kmWebkit, &g_features);
  return true;
}

void AppInitCommandLine(int argc, const char* const* argv) {
  g_command_line = CefCommandLine::CreateCommandLine();
#if defined(OS_WIN)
  g_command_line->InitFromString(::GetCommandLineW());
#else
  g_command_line->InitFromArgv(argc, argv);
#endif

  if (g_command_line->HasSwitch(nw::kVersion)) {
    PrintVersionAndExit();
  }

  if (!AppInitManifest()) {
    // TODO show an empty page
  }

  if (g_manifest == NULL) {
    g_manifest = new base::DictionaryValue();
    base::DictionaryValue* window = new base::DictionaryValue();
    g_manifest->Set(nw::kmWindow, window);

    // Hide toolbar is specifed in the command line
    if (g_command_line->HasSwitch(nw::kNoToolbar))
      window->SetBoolean(nw::kmToolbar, false);
  }
}

// Returns the application command line object.
CefRefPtr<CefCommandLine> AppGetCommandLine() {
  return g_command_line;
}

// Returns the manifest.
base::DictionaryValue* AppGetManifest() {
  return g_manifest;
}

bool ManifestIsFeatureEnabled(const char* name, bool default_value) {
  if (!g_features)
    return default_value;

  bool result = default_value;
  g_features->GetBoolean(name, &result);
  return result;
}

// Returns the application settings based on command line arguments.
void AppGetSettings(CefSettings& settings, CefRefPtr<ClientApp> app) {
  ASSERT(app.get());
  ASSERT(g_command_line.get());
  if (!g_command_line.get())
    return;

  CefString str;

#if defined(OS_WIN)
  settings.multi_threaded_message_loop =
      ManifestIsFeatureEnabled(nw::kMultiThreadedMessageLoop);
#endif

  CefString(&settings.cache_path) =
      g_command_line->GetSwitchValue(nw::kCachePath);

  // Retrieve command-line proxy configuration, if any.
  bool has_proxy = false;
  cef_proxy_type_t proxy_type = PROXY_TYPE_DIRECT;
  CefString proxy_config;

  if (ManifestIsFeatureEnabled(nw::kProxyType)) {
    std::string str = g_command_line->GetSwitchValue(nw::kProxyType);
    if (str == nw::kProxyType_Direct) {
      has_proxy = true;
      proxy_type = PROXY_TYPE_DIRECT;
    } else if (str == nw::kProxyType_Named ||
               str == nw::kProxyType_Pac) {
      proxy_config = g_command_line->GetSwitchValue(nw::kProxyConfig);
      if (!proxy_config.empty()) {
        has_proxy = true;
        proxy_type = (str == nw::kProxyType_Named?
                      PROXY_TYPE_NAMED:PROXY_TYPE_PAC_STRING);
      }
    }
  }

  if (has_proxy) {
    // Provide a ClientApp instance to handle proxy resolution.
    app->SetProxyConfig(proxy_type, proxy_config);
  }
}

// Returns the application browser settings based on command line arguments.
void AppGetBrowserSettings(CefBrowserSettings& settings) {
  ASSERT(g_command_line.get());
  if (!g_command_line.get())
    return;

  // Since we're running local pages, some values are sure
  settings.javascript_disabled = false;
  settings.javascript_open_windows_disallowed = false;
  settings.javascript_close_windows_disallowed = false;
  settings.javascript_access_clipboard_disallowed = false;
  settings.dom_paste_disabled = false;
  settings.universal_access_from_file_urls_allowed = true;
  settings.file_access_from_file_urls_allowed = true;
  settings.web_security_disabled = true;
  settings.xss_auditor_enabled = false;
  settings.image_load_disabled = false;
  settings.hyperlink_auditing_disabled = true;
  settings.local_storage_disabled = false;
  settings.databases_disabled = false;
  settings.application_cache_disabled = false;

  CefString(&settings.default_encoding) =
      g_command_line->GetSwitchValue(nw::kDefaultEncoding);

  settings.encoding_detector_enabled =
      ManifestIsFeatureEnabled(nw::kEncodingDetectorEnabled);
  settings.remote_fonts_disabled =
      ManifestIsFeatureEnabled(nw::kRemoteFontsDisabled);
  settings.caret_browsing_enabled = 
      ManifestIsFeatureEnabled(nw::kCaretBrowsingEnabled);
  settings.java_disabled =
      ManifestIsFeatureEnabled(nw::kJavaDisabled);
  settings.plugins_disabled =
      ManifestIsFeatureEnabled(nw::kPluginsDisabled);
  settings.shrink_standalone_images_to_fit =
      ManifestIsFeatureEnabled(nw::kShrinkStandaloneImagesToFit);
  settings.site_specific_quirks_disabled =
      ManifestIsFeatureEnabled(nw::kSiteSpecificQuirksDisabled);
  settings.text_area_resize_disabled =
      ManifestIsFeatureEnabled(nw::kTextAreaResizeDisabled);
  settings.page_cache_disabled =
      ManifestIsFeatureEnabled(nw::kPageCacheDisabled);
  settings.tab_to_links_disabled =
      ManifestIsFeatureEnabled(nw::kTabToLinksDisabled);
  settings.user_style_sheet_enabled =
      ManifestIsFeatureEnabled(nw::kUserStyleSheetEnabled);

  CefString(&settings.user_style_sheet_location) =
      g_command_line->GetSwitchValue(nw::kUserStyleSheetLocation);

  settings.author_and_user_styles_disabled =
      ManifestIsFeatureEnabled(nw::kAuthorAndUserStylesDisabled);
  settings.webgl_disabled =
      ManifestIsFeatureEnabled(nw::kWebglDisabled);
  settings.accelerated_compositing_disabled =
      ManifestIsFeatureEnabled(nw::kAcceleratedCompositingDisabled);
  settings.accelerated_layers_disabled =
      ManifestIsFeatureEnabled(nw::kAcceleratedLayersDisabled);
  settings.accelerated_video_disabled =
      ManifestIsFeatureEnabled(nw::kAcceleratedVideoDisabled);
  settings.accelerated_2d_canvas_disabled =
      ManifestIsFeatureEnabled(nw::kAcceledated2dCanvasDisabled);
  settings.accelerated_painting_enabled =
      ManifestIsFeatureEnabled(nw::kAcceleratedPaintingEnabled);
  settings.accelerated_filters_enabled =
      ManifestIsFeatureEnabled(nw::kAcceleratedFiltersEnabled);
  settings.accelerated_plugins_disabled =
      ManifestIsFeatureEnabled(nw::kAcceleratedPluginsDisabled);
  settings.developer_tools_disabled =
      ManifestIsFeatureEnabled(nw::kDeveloperToolsDisabled);
  settings.fullscreen_enabled =
      ManifestIsFeatureEnabled(nw::kFullscreenEnabled);
}
