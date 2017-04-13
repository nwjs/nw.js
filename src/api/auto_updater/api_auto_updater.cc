// Copyright (c) 2016 Jefry Tedjokusumo <jtg_gg@yahoo.com.sg>
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

#include "base/values.h"
#include "content/nw/src/api/auto_updater/api_auto_updater.h"
#include "content/nw/src/api/nw_auto_updater.h"
#include "content/nw/src/browser/auto_updater.h"
#include "extensions/browser/extensions_browser_client.h"

using namespace extensions::nwapi::nw__auto_updater;
using namespace content;

namespace extensions {

  static void DispatchEvent(events::HistogramValue histogram_value,
                     const std::string& event_name,
                     std::unique_ptr<base::ListValue> args) {
    DCHECK_CURRENTLY_ON(BrowserThread::UI);
    ExtensionsBrowserClient::Get()->BroadcastEventToRenderers(
      histogram_value, event_name, std::move(args));
  }

  
  class AutoUpdaterObserver : public auto_updater::Delegate {

    // An error happened.
    void OnError(const std::string& error) override {
      std::unique_ptr<base::ListValue> arguments (new base::ListValue());
      arguments->AppendString(error);
      DispatchEvent(events::HistogramValue::UNKNOWN,
                    nwapi::nw__auto_updater::OnError::kEventName,
                    std::move(arguments));
    }
    
    // Checking to see if there is an update
    void OnCheckingForUpdate() override {
      std::unique_ptr<base::ListValue> arguments (new base::ListValue());
      DispatchEvent(events::HistogramValue::UNKNOWN,
                    nwapi::nw__auto_updater::OnCheckingForUpdate::kEventName,
                    std::move(arguments));
    }
    
    // There is an update available and it is being downloaded
    void OnUpdateAvailable() override {
      std::unique_ptr<base::ListValue> arguments (new base::ListValue());
      DispatchEvent(events::HistogramValue::UNKNOWN,
                    nwapi::nw__auto_updater::OnUpdateAvailable::kEventName,
                    std::move(arguments));
    }
    
    // There is no available update.
    void OnUpdateNotAvailable() override {
      std::unique_ptr<base::ListValue> arguments (new base::ListValue());
      DispatchEvent(events::HistogramValue::UNKNOWN,
                    nwapi::nw__auto_updater::OnUpdateNotAvailable::kEventName,
                    std::move(arguments));
    }
    
    // There is a new update which has been downloaded.
    void OnUpdateDownloaded(const std::string& release_notes,
                                    const std::string& release_name,
                                    const base::Time& release_date,
                                    const std::string& update_url) override {
      std::unique_ptr<base::ListValue> arguments (new base::ListValue());
      arguments->AppendString(release_notes);
      arguments->AppendString(release_name);
      arguments->AppendDouble(release_date.ToJsTime());
      arguments->AppendString(update_url);
      DispatchEvent(events::HistogramValue::UNKNOWN,
                    nwapi::nw__auto_updater::OnUpdateDownloaded::kEventName,
                    std::move(arguments));
    }
  
  public:
    AutoUpdaterObserver() {
    }
    
    ~AutoUpdaterObserver() override {
    }

  };
  
  AutoUpdaterObserver gAutoUpdateObserver;
  NwAutoUpdaterNativeCallSyncFunction::NwAutoUpdaterNativeCallSyncFunction() {}

  bool NwAutoUpdaterNativeCallSyncFunction::RunNWSync(base::ListValue* response, std::string* error) {
    if (!auto_updater::AutoUpdater::GetDelegate()) {
      auto_updater::AutoUpdater::SetDelegate(&gAutoUpdateObserver);
    }

    std::string method;
    EXTENSION_FUNCTION_VALIDATE(args_->GetString(0, &method));

    if (method == "SetFeedURL") {
      auto_updater::AutoUpdater::HeaderMap headers;
      std::string url;
      EXTENSION_FUNCTION_VALIDATE(args_->GetString(1, &url));
      auto_updater::AutoUpdater::SetFeedURL(url, headers);
    } else if (method == "GetFeedURL") {
      std::string url = auto_updater::AutoUpdater::GetFeedURL();
      response->AppendString(url);
    } else if (method == "QuitAndInstall") {
      auto_updater::AutoUpdater::QuitAndInstall();
    } else if (method == "CheckForUpdates") {
      auto_updater::AutoUpdater::CheckForUpdates();
    }
    return true;
  }

  
} // namespace extension
