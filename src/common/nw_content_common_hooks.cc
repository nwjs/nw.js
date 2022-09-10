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

#include "chrome/common/chrome_version.h"
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
  std::string* str = package->root()->FindString(switches::kmUserAgent);
  if (str) {
    g_user_agent = *str;
    std::string name, version;
    std::string* str2 = package->root()->FindString(switches::kmName);
    if (str2)
      name = *str2;
    str2 = package->root()->FindString("version");
    if (str2)
      version = *str2;
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
  base::ReplaceSubstringsAfterOffset(&g_user_agent, 0, "%chromium_ver", CHROME_VERSION_STRING);
}

} // nw
