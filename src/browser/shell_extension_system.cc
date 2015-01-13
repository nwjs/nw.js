// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/browser/shell_extension_system.h"

#include <string>

#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/path_service.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/notification_details.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/notification_source.h"
#include "extensions/browser/api/app_runtime/app_runtime_api.h"
#include "extensions/browser/event_router.h"
#include "extensions/browser/extension_prefs.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/browser/info_map.h"
#include "extensions/browser/lazy_background_task_queue.h"
#include "extensions/browser/notification_types.h"
#include "extensions/browser/quota_service.h"
#include "extensions/browser/runtime_data.h"
#include "extensions/common/constants.h"
#include "extensions/common/file_util.h"
#include "extensions/common/manifest_constants.h"

#include "components/crx_file/id_util.h"

#include "content/nw/src/nw_shell.h"
#include "content/nw/src/nw_package.h"

using content::BrowserContext;
using content::BrowserThread;

namespace extensions {

ShellExtensionSystem::ShellExtensionSystem(BrowserContext* browser_context)
    : browser_context_(browser_context) {
}

ShellExtensionSystem::~ShellExtensionSystem() {
}

const Extension* ShellExtensionSystem::LoadInternalApp() {
  base::DictionaryValue manifest;
  manifest.SetString("name", "node-webkit");
  manifest.SetString("version", "1");
  manifest.SetInteger("manifest_version", 2);
  base::ListValue* list = new base::ListValue();
  list->Append(new base::StringValue("nw:*"));
  list->Append(new base::StringValue("file://*"));
  list->Append(new base::StringValue("app://*"));

  manifest.Set(extensions::manifest_keys::kWebURLs, list);

  scoped_ptr<base::ListValue> scripts(new base::ListValue);
  scripts->AppendString("nwapp/background.js");
  nw::Package* package = content::Shell::GetPackage();
  std::string bg_script;
  if (package->root()->GetString("bg-script", &bg_script))
    scripts->AppendString(bg_script);

  manifest.Set(extensions::manifest_keys::kPlatformAppBackgroundScripts, scripts.release());

  base::ListValue* permission_list = new base::ListValue;
  permission_list->Append(new base::StringValue("webview"));
  permission_list->Append(new base::StringValue("webRequest"));
  permission_list->Append(new base::StringValue("webRequestBlocking"));
  permission_list->Append(new base::StringValue("<all_urls>"));
  manifest.Set(extensions::manifest_keys::kPermissions, permission_list);

  std::string error;
  base::FilePath path = package->path();
  //PathService::Get(base::FILE_EXE, &path);
  scoped_refptr<const Extension> extension(Extension::Create(
      path, Manifest::INTERNAL, manifest, Extension::NO_FLAGS,
      //      crx_file::id_util::GenerateId("io-blink"),
      &error));

  if (!extension.get()) {
    LOG(ERROR) << "Loading internal extension "
               << " failed with: " << error;
    return nullptr;
  }

  ExtensionRegistry::Get(browser_context_)->AddEnabled(extension.get());

  RegisterExtensionWithRequestContexts(extension.get());

  content::NotificationService::current()->Notify(
      extensions::NOTIFICATION_EXTENSION_LOADED_DEPRECATED,
      content::Source<BrowserContext>(browser_context_),
      content::Details<const Extension>(extension.get()));

  return extension.get();
}

const Extension* ShellExtensionSystem::LoadApp(const base::FilePath& app_dir) {
  // app_shell only supports unpacked extensions.
  // NOTE: If you add packed extension support consider removing the flag
  // FOLLOW_SYMLINKS_ANYWHERE below. Packed extensions should not have symlinks.
  CHECK(base::DirectoryExists(app_dir)) << app_dir.AsUTF8Unsafe();
  int load_flags = Extension::FOLLOW_SYMLINKS_ANYWHERE;
  std::string load_error;
  scoped_refptr<Extension> extension = file_util::LoadExtension(
      app_dir, Manifest::COMMAND_LINE, load_flags, &load_error);
  if (!extension.get()) {
    LOG(ERROR) << "Loading extension at " << app_dir.value()
               << " failed with: " << load_error;
    return nullptr;
  }

  // TODO(jamescook): We may want to do some of these things here:
  // * Create a PermissionsUpdater.
  // * Call PermissionsUpdater::GrantActivePermissions().
  // * Call ExtensionService::SatisfyImports().
  // * Call ExtensionPrefs::OnExtensionInstalled().
  // * Send NOTIFICATION_EXTENSION_WILL_BE_INSTALLED_DEPRECATED.

  ExtensionRegistry::Get(browser_context_)->AddEnabled(extension.get());

  RegisterExtensionWithRequestContexts(extension.get());

  content::NotificationService::current()->Notify(
      extensions::NOTIFICATION_EXTENSION_LOADED_DEPRECATED,
      content::Source<BrowserContext>(browser_context_),
      content::Details<const Extension>(extension.get()));

  return extension.get();
}

void ShellExtensionSystem::Init() {
  // Inform the rest of the extensions system to start.
  ready_.Signal();
  content::NotificationService::current()->Notify(
      extensions::NOTIFICATION_EXTENSIONS_READY_DEPRECATED,
      content::Source<BrowserContext>(browser_context_),
      content::NotificationService::NoDetails());
}

void ShellExtensionSystem::LaunchApp(const ExtensionId& extension_id) {
  // Send the onLaunched event.
  DCHECK(ExtensionRegistry::Get(browser_context_)
             ->enabled_extensions()
             .Contains(extension_id));
  const Extension* extension = ExtensionRegistry::Get(browser_context_)
                                   ->enabled_extensions()
                                   .GetByID(extension_id);
  AppRuntimeEventRouter::DispatchOnLaunchedEvent(
      browser_context_, extension, extensions::SOURCE_UNTRACKED);
}

void ShellExtensionSystem::Shutdown() {
}

void ShellExtensionSystem::InitForRegularProfile(bool extensions_enabled) {
  runtime_data_.reset(
      new RuntimeData(ExtensionRegistry::Get(browser_context_)));
  lazy_background_task_queue_.reset(
      new LazyBackgroundTaskQueue(browser_context_));
  event_router_.reset(
      new EventRouter(browser_context_, ExtensionPrefs::Get(browser_context_)));
  quota_service_.reset(new QuotaService);
}

ExtensionService* ShellExtensionSystem::extension_service() {
  return NULL;
}

RuntimeData* ShellExtensionSystem::runtime_data() {
  return runtime_data_.get();
}

ManagementPolicy* ShellExtensionSystem::management_policy() {
  return NULL;
}

SharedUserScriptMaster* ShellExtensionSystem::shared_user_script_master() {
  return NULL;
}

StateStore* ShellExtensionSystem::state_store() {
  return NULL;
}

StateStore* ShellExtensionSystem::rules_store() {
  return NULL;
}

InfoMap* ShellExtensionSystem::info_map() {
  if (!info_map_.get())
    info_map_ = new InfoMap;
  return info_map_.get();
}

LazyBackgroundTaskQueue* ShellExtensionSystem::lazy_background_task_queue() {
  return lazy_background_task_queue_.get();
}

EventRouter* ShellExtensionSystem::event_router() {
  return event_router_.get();
}

ErrorConsole* ShellExtensionSystem::error_console() {
  return NULL;
}

InstallVerifier* ShellExtensionSystem::install_verifier() {
  return NULL;
}

QuotaService* ShellExtensionSystem::quota_service() {
  return quota_service_.get();
}

void ShellExtensionSystem::RegisterExtensionWithRequestContexts(
    const Extension* extension) {
  BrowserThread::PostTask(BrowserThread::IO,
                          FROM_HERE,
                          base::Bind(&InfoMap::AddExtension,
                                     info_map(),
                                     make_scoped_refptr(extension),
                                     base::Time::Now(),
                                     false,
                                     false));
}

void ShellExtensionSystem::UnregisterExtensionWithRequestContexts(
    const std::string& extension_id,
    const UnloadedExtensionInfo::Reason reason) {
}

const OneShotEvent& ShellExtensionSystem::ready() const {
  return ready_;
}

ContentVerifier* ShellExtensionSystem::content_verifier() {
  return NULL;
}

scoped_ptr<ExtensionSet> ShellExtensionSystem::GetDependentExtensions(
    const Extension* extension) {
  return make_scoped_ptr(new ExtensionSet());
}

DeclarativeUserScriptMaster*
ShellExtensionSystem::GetDeclarativeUserScriptMasterByExtension(
    const ExtensionId& extension_id) {
  return NULL;
}

}  // namespace extensions
