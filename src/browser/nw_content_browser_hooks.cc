#include "nw_content_browser_hooks.h"

// base
#include "base/command_line.h"
#include "base/files/file_enumerator.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/threading/thread_restrictions.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"

#include "components/crx_file/id_util.h"

// content
#include "content/browser/dom_storage/dom_storage_area.h"
#include "content/common/dom_storage/dom_storage_map.h"
#include "content/public/browser/child_process_security_policy.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/notification_types.h"
// #include "content/public/browser/plugin_service.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/child/v8_value_converter.h"
#include "content/public/common/result_codes.h"
#include "content/public/common/web_preferences.h"

// content/nw
#include "content/nw/src/nw_base.h"
#include "content/nw/src/common/shell_switches.h"

#include "net/cert/x509_certificate.h"
#include "storage/common/database/database_identifier.h"

// ui
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/ui_base_types.h"
#include "ui/gfx/image/image.h"
#include "ui/gfx/image/image_skia_rep.h"
#include "ui/gfx/codec/png_codec.h"

// gen
#include "nw/id/commit.h"

#if defined(OS_WIN)
#define _USE_MATH_DEFINES
#include <math.h>
#endif

using content::RenderProcessHost;

namespace content {
  class WebContents;
}

namespace nw {

namespace {

bool g_pinning_renderer = true;

} //namespace

void SendEventToApp(const std::string& event_name, std::unique_ptr<base::ListValue> event_args);

bool GetDirUserData(base::FilePath*);

void SetTrustAnchors(net::CertificateList&);

// browser
int MainPartsPreCreateThreadsHook() {
  base::ThreadRestrictions::ScopedAllowIO allow_io;
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  nw::Package* package = InitNWPackage();
  if (package && !package->path().empty()) {
    base::FilePath path = package->path().NormalizePathSeparators();

    command_line->AppendSwitchPath("nwapp", path);
    int dom_storage_quota_mb;
    if (package->root()->GetInteger("dom_storage_quota", &dom_storage_quota_mb)) {
      content::DOMStorageMap::SetQuotaOverride(dom_storage_quota_mb * 1024 * 1024);
    }

    base::FilePath user_data_dir;
    std::string name, domain;
    package->root()->GetString("name", &name);
    package->root()->GetString("domain", &domain);
    if (!name.empty() && GetDirUserData(&user_data_dir)) {
      base::FilePath old_dom_storage_dir = user_data_dir
        .Append(FILE_PATH_LITERAL("Local Storage"));
      base::FileEnumerator enum0(old_dom_storage_dir, false, base::FileEnumerator::FILES, FILE_PATH_LITERAL("*_0.localstorage"));
      base::FilePath old_dom_storage = enum0.Next();
      if (!old_dom_storage.empty()) {
        std::string id = domain.empty() ? crx_file::id_util::GenerateId(name) : domain;
        GURL origin("chrome-extension://" + id + "/");
        base::FilePath new_storage_dir = user_data_dir.Append(FILE_PATH_LITERAL("Default"))
          .Append(FILE_PATH_LITERAL("Local Storage"));
        base::CreateDirectory(new_storage_dir);

        base::FilePath new_dom_storage = new_storage_dir
          .Append(content::DOMStorageArea::DatabaseFileNameFromOrigin(origin));
        base::FilePath new_dom_journal = new_dom_storage.ReplaceExtension(FILE_PATH_LITERAL("localstorage-journal"));
        base::FilePath old_dom_journal = old_dom_storage.ReplaceExtension(FILE_PATH_LITERAL("localstorage-journal"));
        if (!base::PathExists(new_dom_journal) && !base::PathExists(new_dom_storage)) {
          base::Move(old_dom_journal, new_dom_journal);
          base::Move(old_dom_storage, new_dom_storage);
          LOG_IF(INFO, true) << "Migrate DOM storage from " << old_dom_storage.AsUTF8Unsafe() << " to " << new_dom_storage.AsUTF8Unsafe();
        }
      }
      base::FilePath old_indexeddb = user_data_dir
        .Append(FILE_PATH_LITERAL("IndexedDB"))
        .Append(FILE_PATH_LITERAL("file__0.indexeddb.leveldb"));
      if (base::PathExists(old_indexeddb)) {
        std::string id = domain.empty() ? crx_file::id_util::GenerateId(name) : domain;
        GURL origin("chrome-extension://" + id + "/");
        base::FilePath new_indexeddb_dir = user_data_dir.Append(FILE_PATH_LITERAL("Default"))
          .Append(FILE_PATH_LITERAL("IndexedDB"))
          .AppendASCII(storage::GetIdentifierFromOrigin(origin))
          .AddExtension(FILE_PATH_LITERAL(".indexeddb.leveldb"));
        if (!base::PathExists(new_indexeddb_dir.DirName())) {
          base::CreateDirectory(new_indexeddb_dir.DirName());
          base::CopyDirectory(old_indexeddb, new_indexeddb_dir.DirName(), true);
          base::Move(new_indexeddb_dir.DirName().Append(FILE_PATH_LITERAL("file__0.indexeddb.leveldb")), new_indexeddb_dir);
          LOG_IF(INFO, true) << "Migrated IndexedDB from " << old_indexeddb.AsUTF8Unsafe() << " to " << new_indexeddb_dir.AsUTF8Unsafe();
        }
      }
    }

  }
  return content::RESULT_CODE_NORMAL_EXIT;
}

void MainPartsPreMainMessageLoopRunHook() {
  nw::Package* package = nw::package();
  const base::ListValue *additional_trust_anchors = NULL;
  if (package->root()->GetList("additional_trust_anchors", &additional_trust_anchors)) {
    net::CertificateList trust_anchors;
    for (size_t i = 0; i<additional_trust_anchors->GetSize(); i++) {
      std::string certificate_string;
      if (!additional_trust_anchors->GetString(i, &certificate_string)) {
        // LOG(WARNING)
        //   << "Could not get string from entry " << i;
        continue;
      }

      net::CertificateList loaded =
        net::X509Certificate::CreateCertificateListFromBytes(
        certificate_string.c_str(), certificate_string.size(),
        net::X509Certificate::FORMAT_AUTO);
      if (loaded.empty() && !certificate_string.empty()) {
        // LOG(WARNING)
        //   << "Could not load certificate from entry " << i;
        continue;
      }

      trust_anchors.insert(trust_anchors.end(), loaded.begin(), loaded.end());
    }
    if (!trust_anchors.empty()) {
      SetTrustAnchors(trust_anchors);
    }
  }
}

bool GetPackageImage(nw::Package* package,
                     const FilePath& icon_path,
                     gfx::Image* image) {
  FilePath path = package->ConvertToAbsoutePath(icon_path);

  // Read the file from disk.
  std::string file_contents;
  if (path.empty() || !base::ReadFileToString(path, &file_contents))
    return false;

  // Decode the bitmap using WebKit's image decoder.
  const unsigned char* data =
      reinterpret_cast<const unsigned char*>(file_contents.data());
  std::unique_ptr<SkBitmap> decoded(new SkBitmap());
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

void MainPartsPostDestroyThreadsHook() {
  ReleaseNWPackage();
}

void RendererProcessTerminatedHook(content::RenderProcessHost* process,
                                   const content::NotificationDetails& details) {
  content::RenderProcessHost::RendererClosedDetails* process_details =
    content::Details<content::RenderProcessHost::RendererClosedDetails>(details).ptr();
  int exit_code = process_details->exit_code;
#if defined(OS_POSIX)
  if (WIFEXITED(exit_code))
    exit_code = WEXITSTATUS(exit_code);
#endif
  SetExitCode(exit_code);
}

bool GetImage(Package* package, const FilePath& icon_path, gfx::Image* image) {
  FilePath path = package->ConvertToAbsoutePath(icon_path);

  // Read the file from disk.
  std::string file_contents;
  if (path.empty() || !base::ReadFileToString(path, &file_contents))
    return false;

  // Decode the bitmap using WebKit's image decoder.
  const unsigned char* data =
      reinterpret_cast<const unsigned char*>(file_contents.data());
  std::unique_ptr<SkBitmap> decoded(new SkBitmap());
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

bool ProcessSingletonNotificationCallbackHook(const base::CommandLine& command_line,
                                              const base::FilePath& current_directory) {
  nw::Package* package = nw::package();
  bool single_instance = true;
  package->root()->GetBoolean(switches::kmSingleInstance, &single_instance);
  if (single_instance) {
#if defined(OS_WIN)
    std::string cmd = base::UTF16ToUTF8(command_line.GetCommandLineString());
#else
    std::string cmd = command_line.GetCommandLineString();
#endif
    std::unique_ptr<base::ListValue> arguments(new base::ListValue());
    arguments->AppendString(cmd);
    SendEventToApp("nw.App.onOpen", std::move(arguments));
  }
    
  return single_instance;
}

#if defined(OS_MACOSX)
bool ApplicationShouldHandleReopenHook(bool hasVisibleWindows) {
  std::unique_ptr<base::ListValue> arguments(new base::ListValue());
  SendEventToApp("nw.App.onReopen",std::move(arguments));
  return true;
}

void OSXOpenURLsHook(const std::vector<GURL>& startup_urls) {
  std::unique_ptr<base::ListValue> arguments(new base::ListValue());
  for (size_t i = 0; i < startup_urls.size(); i++)
    arguments->AppendString(startup_urls[i].spec());
  SendEventToApp("nw.App.onOpen", std::move(arguments));
}
#endif

void OverrideWebkitPrefsHook(content::RenderViewHost* rvh, content::WebPreferences* web_prefs) {
  nw::Package* package = nw::package();
  if (!package)
    return;
  base::DictionaryValue* webkit;
  web_prefs->plugins_enabled = true;
  if (package->root()->GetDictionary(switches::kmWebkit, &webkit)) {
    webkit->GetBoolean("double_tap_to_zoom_enabled", &web_prefs->double_tap_to_zoom_enabled);
    webkit->GetBoolean("plugin",                     &web_prefs->plugins_enabled);
  }
#if 0
  FilePath plugins_dir = package->path();

  plugins_dir = plugins_dir.AppendASCII("plugins");

  content::PluginService* plugin_service = content::PluginService::GetInstance();
  plugin_service->AddExtraPluginDir(plugins_dir);
#endif
}

#if 0
bool ShouldServiceRequestHook(int child_id, const GURL& url) {
  RenderProcessHost* rph = RenderProcessHost::FromID(child_id);
  if (!rph)
    return false;
  return RphGuestFilterURLHook(rph, &url);
}
#endif

bool PinningRenderer() {
  return g_pinning_renderer;
}

void SetPinningRenderer(bool pin) {
  g_pinning_renderer = pin;
}

} //namespace nw
