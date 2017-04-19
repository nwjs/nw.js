// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/nw/src/nw_content_verifier_delegate.h"

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/metrics/field_trial.h"
#include "base/metrics/histogram.h"
#include "base/strings/string_util.h"
#include "base/version.h"
#include "chrome/browser/extensions/extension_service.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/common/extensions/extension_constants.h"
#include "extensions/browser/extension_prefs.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/browser/extension_system.h"
#include "extensions/browser/management_policy.h"
#include "extensions/common/extension.h"
#include "extensions/common/extension_urls.h"
#include "extensions/common/extensions_client.h"
#include "extensions/common/manifest.h"
#include "extensions/common/manifest_url_handlers.h"
#include "net/base/escape.h"

#include "content/nw/src/common/shell_switches.h"

#if defined(OS_CHROMEOS)
#include "chrome/browser/extensions/extension_assets_manager_chromeos.h"
#endif

#include "chrome/browser/extensions/extension_error_reporter.h"

namespace {

const uint8_t kNWSignaturesPublicKey[] = {
  0x30, 0x82, 0x01, 0x22, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01,
  0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0f, 0x00, 0x30, 0x82, 0x01, 0x0a, 0x02, 0x82, 0x01, 0x01,
  0x00, 0xcb, 0x26, 0xf3, 0xa3, 0xd7, 0x72, 0x57, 0x31, 0x1b, 0x7a, 0x19, 0xc0, 0x42, 0x4d, 0xbd,
  0x14, 0xfb, 0x52, 0xcf, 0x4c, 0x34, 0x2a, 0x54, 0x93, 0x03, 0x7a, 0x10, 0x0d, 0xac, 0xf4, 0x45,
  0xee, 0xa0, 0x12, 0x50, 0x4b, 0xdc, 0x33, 0xf6, 0x51, 0x0d, 0xea, 0x7f, 0xfb, 0x12, 0xc5, 0x3b,
  0xbb, 0xa9, 0x7d, 0xbc, 0x2a, 0xaf, 0x3f, 0x68, 0x17, 0x7c, 0x96, 0x4a, 0xe7, 0x68, 0xea, 0x76,
  0xbd, 0x4d, 0x7b, 0xa0, 0x4f, 0x5f, 0xd7, 0x8a, 0x4a, 0xc9, 0xe6, 0xdb, 0x92, 0xda, 0x79, 0x44,
  0xa4, 0x1a, 0x99, 0x4f, 0xe5, 0xea, 0x29, 0xbc, 0x2d, 0xb8, 0x7c, 0xf0, 0x6e, 0xc3, 0x53, 0x25,
  0x63, 0x97, 0x65, 0x1a, 0x74, 0xc3, 0x8b, 0xe3, 0x6b, 0x30, 0xff, 0x9b, 0x56, 0xfe, 0xdc, 0x69,
  0xd3, 0x12, 0xe2, 0xd1, 0x35, 0x38, 0x75, 0x83, 0x5c, 0xe8, 0x9a, 0xbe, 0xe7, 0xdb, 0x8f, 0x6c,
  0xf7, 0x48, 0x6e, 0x72, 0x44, 0x33, 0xe0, 0xa3, 0xf8, 0x55, 0xfb, 0x4b, 0x72, 0x13, 0x47, 0x09,
  0x2c, 0x08, 0x2b, 0x14, 0x39, 0x3f, 0x87, 0x7a, 0x49, 0xc0, 0x12, 0xec, 0xab, 0xac, 0x8b, 0x8f,
  0x12, 0x92, 0x2b, 0xda, 0x14, 0xa2, 0x14, 0xe1, 0x89, 0x7e, 0x70, 0x53, 0xb1, 0x85, 0x1d, 0x39,
  0x0a, 0xc0, 0xa2, 0x0c, 0xc0, 0x14, 0xff, 0x7c, 0x0d, 0xe6, 0x09, 0x99, 0x65, 0x9f, 0xc4, 0x9b,
  0x87, 0x10, 0xb9, 0x3b, 0x8a, 0x82, 0xc6, 0x5c, 0xf3, 0xd4, 0x59, 0x5e, 0x34, 0x54, 0xc2, 0x84,
  0x26, 0x9b, 0x61, 0x3b, 0x3b, 0xf9, 0x5e, 0x2b, 0xeb, 0xc8, 0xac, 0x67, 0x6b, 0x18, 0x8b, 0x39,
  0x69, 0x71, 0xc4, 0xe1, 0xde, 0xb1, 0xb0, 0xa3, 0x4e, 0x86, 0x5c, 0x29, 0x7c, 0x8b, 0xcd, 0x3d,
  0x27, 0xa4, 0x71, 0xcf, 0x9a, 0x62, 0xae, 0x54, 0x54, 0xaa, 0x0f, 0x1e, 0xb3, 0x78, 0x72, 0x67,
  0x91, 0x02, 0x03, 0x01, 0x00, 0x01};
  
const int kNWSignaturesPublicKeySize =
    arraysize(kNWSignaturesPublicKey);

}  // namespace

namespace extensions {

// static
ContentVerifierDelegate::Mode NWContentVerifierDelegate::GetDefaultMode() {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();

  Mode experiment_value = NONE;

  Mode cmdline_value = NONE;
  if (command_line->HasSwitch(switches::kVerifyContent)) {
    std::string switch_value = command_line->GetSwitchValueASCII(
        switches::kVerifyContent);
    if (switch_value == switches::kExtensionContentVerificationBootstrap)
      cmdline_value = ContentVerifierDelegate::BOOTSTRAP;
    else if (switch_value == switches::kExtensionContentVerificationEnforce)
      cmdline_value = ContentVerifierDelegate::ENFORCE;
    else if (switch_value ==
             switches::kExtensionContentVerificationEnforceStrict)
      cmdline_value = ContentVerifierDelegate::ENFORCE_STRICT;
    else
      // If no value was provided (or the wrong one), just default to enforce.
      cmdline_value = ContentVerifierDelegate::ENFORCE;
  }

  // We don't want to allow the command-line flags to eg disable enforcement
  // if the experiment group says it should be on, or malware may just modify
  // the command line flags. So return the more restrictive of the 2 values.
  return std::max(experiment_value, cmdline_value);
}

NWContentVerifierDelegate::NWContentVerifierDelegate(
    content::BrowserContext* context)
    : context_(context), default_mode_(GetDefaultMode()) {
}

NWContentVerifierDelegate::~NWContentVerifierDelegate() {
}

ContentVerifierDelegate::Mode NWContentVerifierDelegate::ShouldBeVerified(
    const Extension& extension) {

  if (extension.is_nwjs_app() && !Manifest::IsComponentLocation(extension.location()))
    return default_mode_;
  if (!extension.is_extension() && !extension.is_legacy_packaged_app())
    return ContentVerifierDelegate::NONE;
  if (!Manifest::IsAutoUpdateableLocation(extension.location()))
    return ContentVerifierDelegate::NONE;

  return default_mode_;
}

ContentVerifierKey NWContentVerifierDelegate::GetPublicKey() {
  return ContentVerifierKey(kNWSignaturesPublicKey,
                            kNWSignaturesPublicKeySize);
}

GURL NWContentVerifierDelegate::GetSignatureFetchUrl(
    const std::string& extension_id,
    const base::Version& version) {
  return GURL();
}

std::set<base::FilePath> NWContentVerifierDelegate::GetBrowserImagePaths(
    const extensions::Extension* extension) {
  return ExtensionsClient::Get()->GetBrowserImagePaths(extension);
}

void NWContentVerifierDelegate::VerifyFailed(
    const std::string& extension_id,
    const base::FilePath& relative_path,
    ContentVerifyJob::FailureReason reason) {
  ExtensionRegistry* registry = ExtensionRegistry::Get(context_);
  const Extension* extension =
      registry->enabled_extensions().GetByID(extension_id);
  if (!extension)
    return;
  ExtensionSystem* system = ExtensionSystem::Get(context_);
  Mode mode = ShouldBeVerified(*extension);
  if (mode >= ContentVerifierDelegate::ENFORCE) {
    ExtensionErrorReporter::GetInstance()->
      ReportLoadError(extension->path(), "Extension file corrupted: " + relative_path.AsUTF8Unsafe(),
                      system->extension_service()->profile(), true);
    system->extension_service()->DisableExtension(extension_id,
                                                  Extension::DISABLE_CORRUPTED);
  }
}

void NWContentVerifierDelegate::Shutdown() {
}

}  // namespace extensions
