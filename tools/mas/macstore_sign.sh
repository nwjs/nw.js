#!/bin/sh

if [ "$#" -lt "5" ]; then
  echo "$(basename $0) source destination identity bundleid version [bundle-version]"
  exit
fi

BASE_DIR="$(dirname $0)"
SOURCE=$1
DESTINATION=$2
IDENTITY=$3
BUNDLEID=$4
VERSION=$5
BUNDLE_VERSION=$6

ENTITLEMENTS_PARENT="$(sed "s/\$BUNDLEID/$BUNDLEID/" "$BASE_DIR/entitlements-parent.plist")"
ENTITLEMENTS_CHILD="$(cat "$BASE_DIR/entitlements-child.plist")"

# make a copy
rm -rf "$DESTINATION/nwjs.app"
cp -p -a "$SOURCE/nwjs.app" "$DESTINATION/nwjs.app"

# fix permissions
find "$DESTINATION/nwjs.app" -type f -perm +0400 \! -perm -0044 -exec chmod a+r {} \;

# fix versions
if [ -n "$VERSION" ]; then
  OLD_VERSION="$(ls "$DESTINATION/nwjs.app/Contents/Versions")"
  find "$DESTINATION/nwjs.app" -name "Info.plist" -exec sed -i "" "s/$OLD_VERSION/$VERSION/" {} \;
fi
if [ -n "$BUNDLE_VERSION" ]; then
  OLD_VERSION="$(ls "$DESTINATION/nwjs.app/Contents/Versions")"
  OLD_BUNDLE_VERSION="${OLD_VERSION:5}"
  find "$DESTINATION/nwjs.app" -name "Info.plist" -exec sed -i "" "s/$OLD_BUNDLE_VERSION/$BUNDLE_VERSION/" {} \;
fi

# fix bundle id
find "$DESTINATION/nwjs.app" -name "Info.plist" -exec sed -i "" "s/io.nwjs.nw/$BUNDLEID/" {} \;

# sign all executables
echo "$ENTITLEMENTS_PARENT" > /tmp/entitlements.parent
echo "$ENTITLEMENTS_CHILD" > /tmp/entitlements.child
find "$DESTINATION/nwjs.app/Contents/Versions" -type f -perm +0111 -exec codesign -f --verbose -s "$IDENTITY" --entitlements /tmp/entitlements.child {} \;
find "$DESTINATION/nwjs.app" \! -path "$DESTINATION/nwjs.app/Contents/Versions/*" -type f -perm +0111 -exec codesign -f --verbose -s "$IDENTITY" --entitlements /tmp/entitlements.parent {} \;
