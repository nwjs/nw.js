// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "extensions/shell/browser/shell_runtime_api_delegate.h"

#include "extensions/common/api/runtime.h"

#if defined(OS_CHROMEOS)
#include "chromeos/dbus/dbus_thread_manager.h"
#include "chromeos/dbus/power_manager_client.h"
#endif

using extensions::core_api::runtime::PlatformInfo;

namespace extensions {

ShellRuntimeAPIDelegate::ShellRuntimeAPIDelegate() {
}

ShellRuntimeAPIDelegate::~ShellRuntimeAPIDelegate() {
}

void ShellRuntimeAPIDelegate::AddUpdateObserver(UpdateObserver* observer) {
}

void ShellRuntimeAPIDelegate::RemoveUpdateObserver(UpdateObserver* observer) {
}

base::Version ShellRuntimeAPIDelegate::GetPreviousExtensionVersion(
    const Extension* extension) {
  return base::Version();
}

void ShellRuntimeAPIDelegate::ReloadExtension(const std::string& extension_id) {
}

bool ShellRuntimeAPIDelegate::CheckForUpdates(
    const std::string& extension_id,
    const UpdateCheckCallback& callback) {
  return false;
}

void ShellRuntimeAPIDelegate::OpenURL(const GURL& uninstall_url) {
}

bool ShellRuntimeAPIDelegate::GetPlatformInfo(PlatformInfo* info) {
#if defined(OS_CHROMEOS)
  info->os = PlatformInfo::OS_CROS_;
#elif defined(OS_LINUX)
  info->os = PlatformInfo::OS_LINUX_;
#endif
  return true;
}

bool ShellRuntimeAPIDelegate::RestartDevice(std::string* error_message) {
// We allow chrome.runtime.restart() to request a device restart on ChromeOS.
#if defined(OS_CHROMEOS)
  chromeos::DBusThreadManager::Get()->GetPowerManagerClient()->RequestRestart();
  return true;
#endif
  *error_message = "Restart is only supported on ChromeOS.";
  return false;
}

}  // namespace extensions
