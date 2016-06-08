#ifndef NW_CONTENT_COMMON_HOOKS_H_
#define NW_CONTENT_COMMON_HOOKS_H_

#include <string>

#include "content/common/content_export.h"

namespace nw {
 
// common
// ref in chrome/browser/renderer_preferences_util.cc
// content/renderer/render_frame_impl.cc
// extensions/browser/app_window/app_window_contents.cc
CONTENT_EXPORT bool GetUserAgentFromManifest(std::string* agent);
// ref in extensions/renderer/dispatcher.cc
CONTENT_EXPORT void SetUserAgentOverride(const std::string& agent,
                         const std::string& name,
                         const std::string& version);

} // nw

#endif // NW_CONTENT_COMMON_HOOKS_H_
