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

#include "content/nw/src/shell_content_client.h"

#include "base/command_line.h"
#include "base/strings/string_piece.h"
#include "base/strings/string_split.h"
#include "chrome/common/chrome_switches.h"
#include "content/nw/src/api/api_messages.h"
#include "content/nw/src/nw_version.h"
#include "content/nw/src/renderer/common/render_messages.h"
#include "content/public/common/content_constants.h"
#include "content/public/common/pepper_plugin_info.h"
#include "content/public/common/user_agent.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/resource/resource_bundle.h"

#include "extensions/common/constants.h"
#include "ppapi/shared_impl/ppapi_permissions.h"

int32 kPepperFlashPermissions = ppapi::PERMISSION_DEV |
                                ppapi::PERMISSION_PRIVATE |
                                ppapi::PERMISSION_BYPASS_USER_GESTURE |
                                ppapi::PERMISSION_FLASH;

namespace {

content::PepperPluginInfo CreatePepperFlashInfo(const base::FilePath& path,
                                                const std::string& version) {
  content::PepperPluginInfo plugin;

  plugin.is_out_of_process = true;
  plugin.name = content::kFlashPluginName;
  plugin.path = path;
  plugin.permissions = kPepperFlashPermissions;

  std::vector<std::string> flash_version_numbers;
  base::SplitString(version, '.', &flash_version_numbers);
  if (flash_version_numbers.size() < 1)
    flash_version_numbers.push_back("11");
  // |SplitString()| puts in an empty string given an empty string. :(
  else if (flash_version_numbers[0].empty())
    flash_version_numbers[0] = "11";
  if (flash_version_numbers.size() < 2)
    flash_version_numbers.push_back("2");
  if (flash_version_numbers.size() < 3)
    flash_version_numbers.push_back("999");
  if (flash_version_numbers.size() < 4)
    flash_version_numbers.push_back("999");
  // E.g., "Shockwave Flash 10.2 r154":
  plugin.description = plugin.name + " " + flash_version_numbers[0] + "." +
      flash_version_numbers[1] + " r" + flash_version_numbers[2];
  plugin.version = JoinString(flash_version_numbers, '.');
  content::WebPluginMimeType swf_mime_type(content::kFlashPluginSwfMimeType,
                                           content::kFlashPluginSwfExtension,
                                           content::kFlashPluginSwfDescription);
  plugin.mime_types.push_back(swf_mime_type);
  content::WebPluginMimeType spl_mime_type(content::kFlashPluginSplMimeType,
                                           content::kFlashPluginSplExtension,
                                           content::kFlashPluginSplDescription);
  plugin.mime_types.push_back(spl_mime_type);

  return plugin;
}


}

namespace content {

ShellContentClient::~ShellContentClient() {
}

std::string ShellContentClient::GetUserAgent() const {
  return content::BuildUserAgentFromProduct("Chrome/" CHROME_VERSION);
}

base::string16 ShellContentClient::GetLocalizedString(int message_id) const {
  return l10n_util::GetStringUTF16(message_id);
}

base::StringPiece ShellContentClient::GetDataResource(
    int resource_id,
    ui::ScaleFactor scale_factor) const {
  return ResourceBundle::GetSharedInstance().GetRawDataResourceForScale(
      resource_id, scale_factor);
}

base::RefCountedStaticMemory* ShellContentClient::GetDataResourceBytes(
    int resource_id) const {
  return ResourceBundle::GetSharedInstance().LoadDataResourceBytes(resource_id);
}

gfx::Image& ShellContentClient::GetNativeImageNamed(int resource_id) const {
  return ResourceBundle::GetSharedInstance().GetNativeImageNamed(resource_id);
}

// This is for message handling after the rvh is swapped out
// FIXME: move dispatcher_host to WebContentsObserver

bool ShellContentClient::CanHandleWhileSwappedOut(
    const IPC::Message& msg) {
  switch (msg.type()) {
    case ShellViewHostMsg_Allocate_Object::ID:
    case ShellViewHostMsg_Deallocate_Object::ID:
    case ShellViewHostMsg_Call_Object_Method::ID:
    case ShellViewHostMsg_Call_Object_Method_Sync::ID:
    case ShellViewHostMsg_Call_Static_Method::ID:
    case ShellViewHostMsg_Call_Static_Method_Sync::ID:
    case ShellViewHostMsg_UncaughtException::ID:
    case ShellViewHostMsg_GetShellId::ID:
    case ShellViewHostMsg_CreateShell::ID:
      return true;
    default:
      break;
  }
  return false;
}

void ShellContentClient::AddAdditionalSchemes(
    std::vector<std::string>* standard_schemes,
    std::vector<std::string>* savable_schemes) {
  standard_schemes->push_back("app");
  standard_schemes->push_back(extensions::kExtensionScheme);
  savable_schemes->push_back(extensions::kExtensionScheme);
  standard_schemes->push_back(extensions::kExtensionResourceScheme);
  savable_schemes->push_back(extensions::kExtensionResourceScheme);
}

void ShellContentClient::AddPepperPlugins(std::vector<content::PepperPluginInfo>* plugins) {
  const base::CommandLine::StringType flash_path =
    base::CommandLine::ForCurrentProcess()->GetSwitchValueNative(
          switches::kPpapiFlashPath);
  if (flash_path.empty())
    return;

  // Also get the version from the command-line. Should be something like 11.2
  // or 11.2.123.45.
  std::string flash_version =
    base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
          switches::kPpapiFlashVersion);

  plugins->push_back(
      CreatePepperFlashInfo(base::FilePath(flash_path), flash_version));
}


}  // namespace content
