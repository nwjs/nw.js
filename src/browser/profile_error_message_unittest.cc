#include "content/nw/src/browser/profile_error_message.h"

#include "base/values.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace nw {
namespace {

TEST(ProfileErrorMessageTest, UsesPackageNameAndKeepsLocalizedWording) {
  base::DictValue manifest;
  manifest.Set("name", "Example App");

  EXPECT_EQ(u"Example App cannot use this newer profile. Update Example App.",
            ResolveProfileTooNewErrorMessage(
                manifest,
                u"Chromium cannot use this newer profile. Update Chromium."));
}

TEST(ProfileErrorMessageTest, SupportsCompleteManifestOverride) {
  base::DictValue manifest;
  manifest.Set("name", "Example App");
  manifest.Set("profile-error-message",
               "Open %name with a compatible data directory.");

  EXPECT_EQ(u"Open Example App with a compatible data directory.",
            ResolveProfileTooNewErrorMessage(manifest, u"NW.js default"));
}

TEST(ProfileErrorMessageTest, EmptyOverrideFallsBackToBrandReplacement) {
  base::DictValue manifest;
  manifest.Set("name", "Example App");
  manifest.Set("profile-error-message", "");

  EXPECT_EQ(u"Example App default",
            ResolveProfileTooNewErrorMessage(manifest, u"NW.js default"));
}

TEST(ProfileErrorMessageTest, MissingNamePreservesLocalizedMessage) {
  base::DictValue manifest;
  EXPECT_EQ(u"Chromium default",
            ResolveProfileTooNewErrorMessage(manifest, u"Chromium default"));
}

}  // namespace
}  // namespace nw

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
