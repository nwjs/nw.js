
if [ "$#" -ne 2 ] || ! [ -d "$2" ]; then
  echo "Usage: $0 IDENTITY DIRECTORY" >&2
  exit 1
fi

identity="$1"
rm -r nwjs.app
cp -r $2/nwjs.app .

codesign --force --verify --verbose --sign "$identity" nwjs.dummy.app
zip -r nwjs.dummy.zip nwjs.dummy.app/

app="nwjs.app"
version=$(ls "$app"/Contents/Versions)

echo "### signing frameworks"
codesign --force --verify --verbose --sign "$identity" "$app/Contents/Versions/$version/nwjs Framework.framework/Helpers/crashpad_handler"
codesign --force --verify --verbose --sign "$identity" "$app/Contents/Versions/$version/nwjs Framework.framework/libffmpeg.dylib"
codesign --force --verify --verbose --sign "$identity" "$app/Contents/Versions/$version/nwjs Framework.framework/libnode.dylib"
codesign --force --verify --verbose --sign "$identity" "$app/Contents/Versions/$version/nwjs Framework.framework/"
codesign --force --verify --verbose --sign "$identity" "$app/Contents/Versions/$version/nwjs Helper.app/"
codesign --force --verify --verbose --sign "$identity" "$app/Contents/Versions/$version/Mantle.framework/"
codesign --force --verify --verbose --sign "$identity" "$app/Contents/Versions/$version/ReactiveCocoa.framework/"
codesign --force --verify --verbose --sign "$identity" "$app/Contents/Versions/$version/Squirrel.framework/"

echo "### signing app"
codesign --force --verify --verbose --sign "$identity" "$app"

echo "### verifying signature"
codesign -vvv -d "$app"

nwjs.app/Contents/MacOS/nwjs
