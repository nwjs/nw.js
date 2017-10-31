#include "nw_content_common_hooks.h"

// base
#include "base/strings/string_util.h"
#include "base/values.h"

// content
#include "content/public/common/user_agent.h"

// content/nw
#include "content/nw/src/common/shell_switches.h"
#include "content/nw/src/nw_base.h"
#include "content/nw/src/nw_package.h"
#include "content/nw/src/nw_version.h"

namespace nw {

namespace {
std::string g_user_agent;
}

// common
bool GetUserAgentFromManifest(std::string* agent) {
  if (!g_user_agent.empty()) {
    *agent = g_user_agent;
    return true;
  }
  nw::Package* package = nw::package();
  if (!package)
    return false;
  if (package->root()->GetString(switches::kmUserAgent, &g_user_agent)) {
    std::string name, version;
    package->root()->GetString(switches::kmName, &name);
    package->root()->GetString("version", &version);
    SetUserAgentOverride(g_user_agent, name, version);
    *agent = g_user_agent;
    return true;
  }
  return false;
}

void SetUserAgentOverride(const std::string& agent,
                          const std::string& name,
                          const std::string& version) {
  g_user_agent = agent;
  base::ReplaceSubstringsAfterOffset(&g_user_agent, 0, "%name", name);
  base::ReplaceSubstringsAfterOffset(&g_user_agent, 0, "%ver", version);
  base::ReplaceSubstringsAfterOffset(&g_user_agent, 0, "%nwver", NW_VERSION_STRING);
  base::ReplaceSubstringsAfterOffset(&g_user_agent, 0, "%webkit_ver", content::GetWebKitVersion());
  base::ReplaceSubstringsAfterOffset(&g_user_agent, 0, "%osinfo", content::BuildOSInfo());
}

} // nw