// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_APP_CHROME_CRASH_REPORTER_CLIENT_H_
#define CHROME_APP_CHROME_CRASH_REPORTER_CLIENT_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "components/crash/app/crash_reporter_client.h"

namespace chrome {

class ChromeCrashReporterClient : public crash_reporter::CrashReporterClient {
 public:
  ChromeCrashReporterClient();
  ~ChromeCrashReporterClient() override;

  // crash_reporter::CrashReporterClient implementation.
  void SetCrashReporterClientIdFromGUID(
      const std::string& client_guid) override;
#if defined(OS_WIN)
  virtual bool GetAlternativeCrashDumpLocation(base::FilePath* crash_dir)
      override;
  virtual void GetProductNameAndVersion(const base::FilePath& exe_path,
                                        base::string16* product_name,
                                        base::string16* version,
                                        base::string16* special_build,
                                        base::string16* channel_name) override;
  virtual bool ShouldShowRestartDialog(base::string16* title,
                                       base::string16* message,
                                       bool* is_rtl_locale) override;
  virtual bool AboutToRestart() override;
  virtual bool GetDeferredUploadsSupported(bool is_per_user_install) override;
  virtual bool GetIsPerUserInstall(const base::FilePath& exe_path) override;
  virtual bool GetShouldDumpLargerDumps(bool is_per_user_install) override;
  virtual int GetResultCodeRespawnFailed() override;
  virtual void InitBrowserCrashDumpsRegKey() override;
  virtual void RecordCrashDumpAttempt(bool is_real_crash) override;
#endif

#if defined(OS_POSIX) && !defined(OS_MAC) && !defined(OS_IOS)
  void GetProductNameAndVersion(const char** product_name,
                                const char** version) override;
  base::FilePath GetReporterLogFilename() override;
#endif

  bool GetCrashDumpLocation(base::FilePath* crash_dir) override;

  size_t RegisterCrashKeys() override;

  bool IsRunningUnattended() override;

  bool GetCollectStatsConsent() override;

#if defined(OS_WIN) || defined(OS_MAC)
  bool ReportingIsEnforcedByPolicy(bool* breakpad_enabled) override;
#endif

#if defined(OS_ANDROID)
  virtual int GetAndroidMinidumpDescriptor() override;
#endif

#if defined(OS_MAC)
  void InstallAdditionalFilters(BreakpadRef breakpad) override;
#endif

  bool EnableBreakpadForProcess(const std::string& process_type) override;

 private:
  DISALLOW_COPY_AND_ASSIGN(ChromeCrashReporterClient);
};

}  // namespace chrome

#endif  // CHROME_APP_CHROME_CRASH_REPORTER_CLIENT_H_
