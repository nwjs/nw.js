#include "nw_chrome_browser_hooks.h"

// base
#include "base/command_line.h"
#include "base/files/file_enumerator.h"
#include "base/files/file_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/path_service.h"
#include "base/threading/thread_restrictions.h"

#if defined(OS_WIN)
#include "base/win/scoped_gdi_object.h"
#endif // OS_WIN

// chrome
#include "chrome/browser/browser_process.h"
#include "chrome/browser/devtools/devtools_window.h"
#include "chrome/browser/io_thread.h"
#include "chrome/browser/lifetime/application_lifetime.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "chrome/browser/lifetime/keep_alive_registry.h"
#include "chrome/browser/lifetime/keep_alive_types.h"
#include "chrome/common/chrome_constants.h"
#include "chrome/common/chrome_paths.h"

#include "components/content_settings/core/browser/content_settings_utils.h"
#include "components/crx_file/id_util.h"

// content
#include "content/common/dom_storage/dom_storage_map.h"
#include "content/browser/dom_storage/dom_storage_area.h"
#include "content/nw/src/common/shell_switches.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/child_process_security_policy.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/result_codes.h"

// content/nw
#include "content/nw/src/nw_base.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/policy_cert_verifier.h"

// extensions
#include "extensions/browser/event_router.h"
#include "extensions/browser/extension_prefs.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/common/extension.h"

#include "net/cert/x509_certificate.h"
#include "sql/connection.h"
#include "sql/meta_table.h"
#include "sql/transaction.h"
#include "storage/common/database/database_identifier.h"

#if defined(OS_WIN)
#define _USE_MATH_DEFINES
#include <math.h>
#endif

using content::DevToolsAgentHost;
using content::RenderFrameHost;
using extensions::EventRouter;
using extensions::Extension;
using extensions::ExtensionPrefs;
using extensions::ExtensionRegistry;

namespace nw
{

namespace {

gfx::Image g_app_icon;
int g_cdt_process_id = -1;
std::string g_extension_id;
bool g_reloading_app = false;

#if defined(OS_WIN)
base::win::ScopedHICON g_window_hicon;
base::win::ScopedHICON g_app_hicon;
#endif

}

void ReloadExtensionHook(const extensions::Extension* extension) {
  g_reloading_app = true;
  KeepAliveRegistry::GetInstance()->Register(KeepAliveOrigin::APP_CONTROLLER, KeepAliveRestartOption::ENABLED);
  //keep alive count is paired with
  //appwindow's lifetime, so it's
  //needed to keep alive during
  //reload
}

void CreateAppWindowHook(extensions::AppWindow* app_window) {
  if (g_reloading_app) {
    g_reloading_app = false;
    KeepAliveRegistry::GetInstance()->Unregister(KeepAliveOrigin::APP_CONTROLLER, KeepAliveRestartOption::ENABLED);
  }
}

bool IsReloadingApp() {
  return g_reloading_app;
}

typedef void (*SendEventToAppFn)(const std::string& event_name, std::unique_ptr<base::ListValue> event_args);
CONTENT_EXPORT extern SendEventToAppFn gSendEventToApp;

void SendEventToApp(const std::string& event_name, std::unique_ptr<base::ListValue> event_args) {
  Profile* profile = ProfileManager::GetActiveUserProfile();
  const extensions::ExtensionSet& extensions =
    ExtensionRegistry::Get(profile)->enabled_extensions();
  ExtensionPrefs* extension_prefs = ExtensionPrefs::Get(profile);

  for (extensions::ExtensionSet::const_iterator it = extensions.begin();
       it != extensions.end(); ++it) {
    const Extension* extension = it->get();
    if (extension_prefs->IsExtensionRunning(extension->id()) &&
        extension->location() == extensions::Manifest::COMMAND_LINE) {
      std::unique_ptr<extensions::Event> event(new extensions::Event(extensions::events::UNKNOWN,
                                                                event_name,
                                                                std::move(event_args)));
      event->restrict_to_browser_context = profile;
      EventRouter::Get(profile)
        ->DispatchEventToExtension(extension->id(), std::move(event));
    }
  }
}

typedef bool (*CheckStoragePartitionMatchesFn)(int render_process_id, const GURL& url);
CONTENT_EXPORT extern CheckStoragePartitionMatchesFn gCheckStoragePartitionMatches;

bool CheckStoragePartitionMatches(int render_process_id, const GURL& url) {
  return render_process_id == g_cdt_process_id && url.SchemeIs(content_settings::kChromeDevToolsScheme);
}

void ShowDevtools(bool show, content::WebContents* web_contents, content::WebContents* container) {
  content::RenderFrameHost* rfh = web_contents->GetMainFrame();
  if (container) {
    scoped_refptr<DevToolsAgentHost> agent_host(DevToolsAgentHost::GetOrCreateFor(rfh));
    g_cdt_process_id = container->GetRenderProcessHost()->GetID();
    content::ChildProcessSecurityPolicy::GetInstance()->GrantAll(g_cdt_process_id);
    
    DevToolsWindow* window = DevToolsWindow::FindDevToolsWindow(agent_host.get());
    if (!window) {
      Profile* profile = Profile::FromBrowserContext(
             web_contents->GetBrowserContext());
      window = DevToolsWindow::Create(profile, nullptr, DevToolsWindow::kFrontendDefault, std::string(), false, std::string(), std::string(), container);
      if (!window)
        return;
      window->bindings_->AttachTo(agent_host);
    }
    return;
  }

  if (show) {
    DevToolsWindow::InspectElement(rfh, 0, 0);
    return;
  }
  scoped_refptr<DevToolsAgentHost> agent(
      DevToolsAgentHost::GetOrCreateFor(rfh));
  DevToolsWindow* window = DevToolsWindow::FindDevToolsWindow(agent.get());
  if (window)
    window->InspectedContentsClosing();
}

const char* GetChromiumVersion() {
  return chrome::kChromeVersion;
}

bool GetDirUserData(base::FilePath *user_data_dir) {
  return PathService::Get(chrome::DIR_USER_DATA, user_data_dir);
}

void SetTrustAnchorsOnIOThread(IOThread* io_thread, const net::CertificateList& trust_anchors) {
  PolicyCertVerifier* verifier =
    (PolicyCertVerifier*)io_thread->globals()->cert_verifier.get();
  verifier->SetTrustAnchors(trust_anchors);
}

void SetTrustAnchors(net::CertificateList& trust_anchors) {
  // LOG(INFO)
  //   << "Added " << trust_anchors.size() << " certificates to trust anchors.";
  content::BrowserThread::PostTask(
    content::BrowserThread::IO,
    FROM_HERE,
    base::Bind(SetTrustAnchorsOnIOThread, g_browser_process->io_thread(), trust_anchors));
}

void SetAppIcon(gfx::Image &icon) {
  g_app_icon = icon;
}

gfx::ImageSkia* GetAppIcon() {
  if (g_app_icon.IsEmpty())
    return nullptr;
  return const_cast<gfx::ImageSkia*>(g_app_icon.ToImageSkia());
}

void SetMainExtensionId(const std::string& id) {
  g_extension_id = id;
}

const std::string& GetMainExtensionId() {
  return g_extension_id;
}

#if defined(OS_WIN)
HICON GetAppHIcon() {
  return g_app_hicon.get();
}

HICON GetWindowHIcon() {
  return g_window_hicon.get();
}

void SetAppHIcon(base::win::ScopedHICON icon) {
  g_app_hicon = std::move(icon);
}

void SetWindowHIcon(base::win::ScopedHICON icon) {
  g_window_hicon = std::move(icon);
}

#endif

int MainPartsPreCreateThreadsHook() {
  gCheckStoragePartitionMatches = CheckStoragePartitionMatches;
#if defined(OS_MACOSX)
  gSendEventToApp = SendEventToApp;
#endif
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
#if defined(OS_WIN)
      base::FilePath old_dom_storage_dir = user_data_dir.DirName()
        .Append(FILE_PATH_LITERAL("Local Storage"));
#else
      base::FilePath old_dom_storage_dir = user_data_dir
        .Append(FILE_PATH_LITERAL("Local Storage"));
#endif
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
#if defined(OS_WIN)
      base::FilePath old_websqldir = user_data_dir.DirName()
        .Append(FILE_PATH_LITERAL("databases"));
#else
      base::FilePath old_websqldir = user_data_dir
        .Append(FILE_PATH_LITERAL("databases"));
#endif
      base::FileEnumerator enum1(old_websqldir, false, base::FileEnumerator::DIRECTORIES, FILE_PATH_LITERAL("app_*"));
      base::FilePath app_websql_dir = enum1.Next();
      std::string old_id("file__0");
      if (!app_websql_dir.empty())
        old_id = app_websql_dir.BaseName().AsUTF8Unsafe();
      if (base::PathExists(old_websqldir)) {
        std::string id = domain.empty() ? crx_file::id_util::GenerateId(name) : domain;
        GURL origin("chrome-extension://" + id + "/");
        base::FilePath new_websql_dir = user_data_dir.Append(FILE_PATH_LITERAL("Default"))
          .Append(FILE_PATH_LITERAL("databases"))
          .AppendASCII(storage::GetIdentifierFromOrigin(origin));
        if (!base::PathExists(new_websql_dir.DirName())) {
          base::CreateDirectory(new_websql_dir.DirName());
          base::CopyDirectory(old_websqldir, new_websql_dir.DirName().DirName(), true);
          base::Move(new_websql_dir.DirName().Append(base::FilePath::FromUTF8Unsafe(old_id)), new_websql_dir);

          base::FilePath metadb_path = new_websql_dir.DirName().Append(FILE_PATH_LITERAL("Databases.db"));
          sql::Connection metadb;
          if (metadb.Open(metadb_path) && sql::MetaTable::DoesTableExist(&metadb)) {
            std::string stmt = "UPDATE Databases SET origin='" + storage::GetIdentifierFromOrigin(origin) + "' WHERE origin='" + old_id + "'";
            if (!metadb.Execute(stmt.c_str()))
              LOG_IF(INFO, true) << "Fail to execute migrate SQL.";
          }
          LOG_IF(INFO, true) << "Migrated WebSql DB from " << old_websqldir.AsUTF8Unsafe() << " to " << new_websql_dir.AsUTF8Unsafe();
        }
      }
#if defined(OS_WIN)
      base::FilePath old_indexeddb = user_data_dir.DirName()
        .Append(FILE_PATH_LITERAL("IndexedDB"))
        .Append(FILE_PATH_LITERAL("file__0.indexeddb.leveldb"));
#else
      base::FilePath old_indexeddb = user_data_dir
        .Append(FILE_PATH_LITERAL("IndexedDB"))
        .Append(FILE_PATH_LITERAL("file__0.indexeddb.leveldb"));
#endif
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

} // namespace nw
