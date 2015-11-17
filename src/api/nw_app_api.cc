#include "content/nw/src/api/nw_app_api.h"

#include "base/command_line.h"
#include "chrome/browser/browsing_data/browsing_data_helper.h"
#include "chrome/browser/devtools/devtools_window.h"
#include "chrome/browser/extensions/devtools_util.h"
#include "chrome/browser/extensions/extension_service.h"
#include "content/nw/src/nw_base.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "extensions/browser/extension_system.h"
#include "extensions/common/error_utils.h"

namespace extensions {
NwAppQuitFunction::NwAppQuitFunction() {

}

NwAppQuitFunction::~NwAppQuitFunction() {
}

bool NwAppQuitFunction::RunAsync() {
  ExtensionService* service =
    ExtensionSystem::Get(browser_context())->extension_service();
  base::MessageLoop::current()->PostTask(
        FROM_HERE,
        base::Bind(&ExtensionService::TerminateExtension,
                   service->AsWeakPtr(),
                   extension_id()));
  return true;
}

NwAppGetArgvSyncFunction::NwAppGetArgvSyncFunction() {
}

NwAppGetArgvSyncFunction::~NwAppGetArgvSyncFunction() {
}

bool NwAppGetArgvSyncFunction::RunNWSync(base::ListValue* response, std::string* error) {

  nw::Package* package = nw::package();
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  base::CommandLine::StringVector args = command_line->GetArgs();
  base::CommandLine::StringVector argv = command_line->original_argv();

  // Ignore first non-switch arg if it's not a standalone package.
  bool ignore_arg = !package->self_extract();
  for (unsigned i = 1; i < argv.size(); ++i) {
    if (ignore_arg && args.size() && argv[i] == args[0]) {
      ignore_arg = false;
      continue;
    }

    response->AppendString(argv[i]);
  }
  return true;
}

NwAppClearCacheFunction::NwAppClearCacheFunction() {
}

NwAppClearCacheFunction::~NwAppClearCacheFunction() {
}

bool NwAppClearCacheFunction::RunNWSync(base::ListValue* response, std::string* error) {
  BrowsingDataRemover* remover = BrowsingDataRemover::CreateForUnboundedRange(
      Profile::FromBrowserContext(context_));

  remover->AddObserver(this);
  remover->Remove(BrowsingDataRemover::REMOVE_CACHE,
                  BrowsingDataHelper::ALL);
  // BrowsingDataRemover deletes itself.
  run_loop_.Run();
  return true;
}

void NwAppClearCacheFunction::OnBrowsingDataRemoverDone() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  run_loop_.Quit();
}

} // namespace extensions
