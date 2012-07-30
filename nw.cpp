// Copyright (c) 2010 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <cstdlib>
#include <stdio.h>
#include <sstream>
#include <string>

#include "base/file_util.h"
#include "base/json/json_file_value_serializer.h"
#include "base/logging.h"
#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/cef_frame.h"
#include "include/cef_runnable.h"
#include "include/cef_web_plugin.h"
#include "nw/client_handler.h"
#include "nw/client_switches.h"
#include "nw/nw.h"
#include "nw/string_util.h"
#include "nw/util.h"

namespace {

// Return the int representation of the specified string.
int GetIntValue(const CefString& str) {
  if (str.empty())
    return 0;

  std::string stdStr = str;
  return atoi(stdStr.c_str());
}

}  // namespace

CefRefPtr<ClientHandler> g_handler;
CefRefPtr<CefCommandLine> g_command_line;

// Don't bother with its life since it's used through out the program
base::DictionaryValue* g_manifest;

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

void ManifestConvertRelativePaths(
    FilePath path,
    base::DictionaryValue* manifest) {
  if (manifest->HasKey("main")) {
    std::string out;
    if (!manifest->GetString("main", &out)) {
      manifest->Remove("main", NULL);
      LOG(WARNING) << "'main' field in manifest must be a string.";
      return;
    }
    FilePath main_path = path.Append(out);
    std::string url("file://");
    g_manifest->SetString("main", url + main_path.value());
  } else {
    LOG(WARNING) << "'main' field in manifest should be specifed.";
  }
}

void AppInitManifest() {
  // Get first argument
  CefCommandLine::ArgumentList arguments;
  g_command_line->GetArguments(arguments);

  FilePath path(arguments[0]);
  if (!file_util::PathExists(path)) {
    LOG(WARNING) << "Package does not exist.";
    return;
  }

  // If it's a file then try to extract from it
  if (!file_util::DirectoryExists(path)) {
    DLOG(INFO) << "Extracting packaging...";
  }

  FilePath manifest_path = path.Append("package.json");
  if (!file_util::PathExists(manifest_path)) {
    LOG(WARNING) << "No 'package.json' in package.";
    return;
  }

  std::string error;
  JSONFileValueSerializer serializer(manifest_path);
  scoped_ptr<Value> root(serializer.Deserialize(NULL, &error));
  if (!root.get()) {
    if (error.empty()) {
      LOG(WARNING) << "It's not able to read the manifest file.";
    } else {
      LOG(WARNING) << "Parsing error: " << error;
    }
    return;
  }

  if (!root->IsType(Value::TYPE_DICTIONARY)) {
    LOG(WARNING) << "Manifest file is invalid, we need a dictionary type";
    return;
  }

  g_manifest = static_cast<DictionaryValue*>(root.release());
  ManifestConvertRelativePaths(path, g_manifest);
}

void AppInitCommandLine(int argc, const char* const* argv) {
  g_command_line = CefCommandLine::CreateCommandLine();
#if defined(OS_WIN)
  g_command_line->InitFromString(::GetCommandLineW());
#else
  g_command_line->InitFromArgv(argc, argv);
#endif

  if (g_command_line->HasArguments())
    AppInitManifest();
}

// Returns the application command line object.
CefRefPtr<CefCommandLine> AppGetCommandLine() {
  return g_command_line;
}

// Returns the manifest.
base::DictionaryValue* AppGetManifest() {
  return g_manifest;
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
      g_command_line->HasSwitch(nw::kMultiThreadedMessageLoop);
#endif

  CefString(&settings.cache_path) =
      g_command_line->GetSwitchValue(nw::kCachePath);

  // Retrieve command-line proxy configuration, if any.
  bool has_proxy = false;
  cef_proxy_type_t proxy_type = PROXY_TYPE_DIRECT;
  CefString proxy_config;

  if (g_command_line->HasSwitch(nw::kProxyType)) {
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

  settings.remote_fonts_disabled =
      g_command_line->HasSwitch(nw::kRemoteFontsDisabled);

  CefString(&settings.default_encoding) =
      g_command_line->GetSwitchValue(nw::kDefaultEncoding);

  settings.encoding_detector_enabled =
      g_command_line->HasSwitch(nw::kEncodingDetectorEnabled);
  settings.javascript_disabled =
      g_command_line->HasSwitch(nw::kJavascriptDisabled);
  settings.javascript_open_windows_disallowed =
      g_command_line->HasSwitch(nw::kJavascriptOpenWindowsDisallowed);
  settings.javascript_close_windows_disallowed =
      g_command_line->HasSwitch(nw::kJavascriptCloseWindowsDisallowed);
  settings.javascript_access_clipboard_disallowed =
      g_command_line->HasSwitch(
          nw::kJavascriptAccessClipboardDisallowed);
  settings.dom_paste_disabled =
      g_command_line->HasSwitch(nw::kDomPasteDisabled);
  settings.caret_browsing_enabled =
      g_command_line->HasSwitch(nw::kCaretBrowsingDisabled);
  settings.java_disabled =
      g_command_line->HasSwitch(nw::kJavaDisabled);
  settings.plugins_disabled =
      g_command_line->HasSwitch(nw::kPluginsDisabled);
  settings.universal_access_from_file_urls_allowed =
      g_command_line->HasSwitch(nw::kUniversalAccessFromFileUrlsAllowed);
  settings.file_access_from_file_urls_allowed =
      g_command_line->HasSwitch(nw::kFileAccessFromFileUrlsAllowed);
  settings.web_security_disabled =
      g_command_line->HasSwitch(nw::kWebSecurityDisabled);
  settings.xss_auditor_enabled =
      g_command_line->HasSwitch(nw::kXssAuditorEnabled);
  settings.image_load_disabled =
      g_command_line->HasSwitch(nw::kImageLoadingDisabled);
  settings.shrink_standalone_images_to_fit =
      g_command_line->HasSwitch(nw::kShrinkStandaloneImagesToFit);
  settings.site_specific_quirks_disabled =
      g_command_line->HasSwitch(nw::kSiteSpecificQuirksDisabled);
  settings.text_area_resize_disabled =
      g_command_line->HasSwitch(nw::kTextAreaResizeDisabled);
  settings.page_cache_disabled =
      g_command_line->HasSwitch(nw::kPageCacheDisabled);
  settings.tab_to_links_disabled =
      g_command_line->HasSwitch(nw::kTabToLinksDisabled);
  settings.hyperlink_auditing_disabled =
      g_command_line->HasSwitch(nw::kHyperlinkAuditingDisabled);
  settings.user_style_sheet_enabled =
      g_command_line->HasSwitch(nw::kUserStyleSheetEnabled);

  CefString(&settings.user_style_sheet_location) =
      g_command_line->GetSwitchValue(nw::kUserStyleSheetLocation);

  settings.author_and_user_styles_disabled =
      g_command_line->HasSwitch(nw::kAuthorAndUserStylesDisabled);
  settings.local_storage_disabled =
      g_command_line->HasSwitch(nw::kLocalStorageDisabled);
  settings.databases_disabled =
      g_command_line->HasSwitch(nw::kDatabasesDisabled);
  settings.application_cache_disabled =
      g_command_line->HasSwitch(nw::kApplicationCacheDisabled);
  settings.webgl_disabled =
      g_command_line->HasSwitch(nw::kWebglDisabled);
  settings.accelerated_compositing_disabled =
      g_command_line->HasSwitch(nw::kAcceleratedCompositingDisabled);
  settings.accelerated_layers_disabled =
      g_command_line->HasSwitch(nw::kAcceleratedLayersDisabled);
  settings.accelerated_video_disabled =
      g_command_line->HasSwitch(nw::kAcceleratedVideoDisabled);
  settings.accelerated_2d_canvas_disabled =
      g_command_line->HasSwitch(nw::kAcceledated2dCanvasDisabled);
  settings.accelerated_painting_enabled =
      g_command_line->HasSwitch(nw::kAcceleratedPaintingEnabled);
  settings.accelerated_filters_enabled =
      g_command_line->HasSwitch(nw::kAcceleratedFiltersEnabled);
  settings.accelerated_plugins_disabled =
      g_command_line->HasSwitch(nw::kAcceleratedPluginsDisabled);
  settings.developer_tools_disabled =
      g_command_line->HasSwitch(nw::kDeveloperToolsDisabled);
  settings.fullscreen_enabled =
      g_command_line->HasSwitch(nw::kFullscreenEnabled);
}
