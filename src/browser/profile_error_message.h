#ifndef CONTENT_NW_SRC_BROWSER_PROFILE_ERROR_MESSAGE_H_
#define CONTENT_NW_SRC_BROWSER_PROFILE_ERROR_MESSAGE_H_

#include <string>

#include "base/values.h"

namespace nw {

// Applies the package manifest to Chromium's localized profile-version error.
// `profile-error-message` is an optional complete override and supports the
// same `%name` token used by other manifest strings. Without an override, only
// Chromium/NW.js branding is replaced, preserving the locale's wording.
std::u16string ResolveProfileTooNewErrorMessage(
    const base::DictValue& manifest,
    const std::u16string& localized_message);

}  // namespace nw

#endif  // CONTENT_NW_SRC_BROWSER_PROFILE_ERROR_MESSAGE_H_
