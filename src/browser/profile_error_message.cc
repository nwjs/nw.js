#include "content/nw/src/browser/profile_error_message.h"

#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"

namespace nw {

std::u16string ResolveProfileTooNewErrorMessage(
    const base::DictValue& manifest,
    const std::u16string& localized_message) {
  const std::string* package_name = manifest.FindString("name");
  if (!package_name || package_name->empty()) {
    return localized_message;
  }

  const std::u16string name = base::UTF8ToUTF16(*package_name);
  if (const std::string* custom =
          manifest.FindString("profile-error-message")) {
    if (!custom->empty()) {
      std::u16string message = base::UTF8ToUTF16(*custom);
      base::ReplaceSubstringsAfterOffset(&message, 0, u"%name", name);
      return message;
    }
  }

  std::u16string message = localized_message;
  base::ReplaceSubstringsAfterOffset(&message, 0, u"Chromium", name);
  base::ReplaceSubstringsAfterOffset(&message, 0, u"NW.js", name);
  return message;
}

}  // namespace nw
