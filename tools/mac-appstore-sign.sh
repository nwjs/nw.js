#!/bin/sh
echo
echo "True Interactions (Tint) and node-webkit Mac App Store / Entitlements Signing Tool."
echo "NOTE: THIS SIGNING TOOL IS INTENDED ONLY FOR APPLICATIONS TO BE SUBMITTED TO"
echo "THE MAC APP STORE OR THAT NEED ENTITLEMENTS. IF YOU DO NOT PLAN ON USING EITHER"
echo "USE THE DEFAULT SIGNING UTILITY."
echo "www.trueinteractions.com"
echo
if [ -z "$5" ]
then
	echo "Usage: sign.sh application_file_path application_name bundle_id identity out_directory"
	echo "  application_file_name \t The full path to the application to sign, original is not modified"
	echo "  application_name \t\t The full application name (with spaces if needed), do not include .app extention"
	echo "  bundle_id \t\t\t The bundle identifier used in the Info.plist, it must be unique for helpers and the app"
	echo "  identity \t\t\t The identity to sign the application with (a valid list of id's is printed out later)"
	echo "  entitlements \t\t The file that holds the permissions for this app (see entitlements-example.xml)"
	echo "  out_directory \t\t The directory to place the signed application at"
	echo ""
	echo "If you do not know your identity pick the most appropriate one from the list below, the valid identities"
	echo "installed on your /Applications/Utilities/Keychain Access.app are:"
	echo
	security -q find-identity -p codesigning -v
	echo
	echo "Tip: your identity is the alpha-numeric, usually 10 character long string contained between parenthesis."
	echo "Note that entitlements for the Mac App Store will be applied, this will sandbox the application and may"
	echo "in certain circumstances cause new errors if your application plays outside of Mac App Stores rules."
	echo
	exit 1;
fi

export SOURCE=$1
export NAME=$2
export IDENTITY=$4
export BUNDLEID=$3
export OUTDIRECTORY=$6
export ENTITLEMENTS_PARENT=$5
export ENTITLEMENTS_CHILD='<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>com.apple.security.app-sandbox</key>
	<true/>
	<key>com.apple.security.inherit</key>
	<true/>
</dict>
</plist>
'

cat "$ENTITLEMENTS_PARENT" > /tmp/entitlements.parent
echo "$ENTITLEMENTS_CHILD" > /tmp/entitlements.child

# copy your app to this folder.
rm -rf $OUTDIRECTORY/$NAME.app
cp -p -a $SOURCE $OUTDIRECTORY/$NAME.app

echo "==Signing Code=="
codesign --deep -s $IDENTITY -i $BUNDLEID --entitlements /tmp/entitlements.child "$OUTDIRECTORY/$NAME.app/Contents/Frameworks/$NAME Helper.app"
codesign --deep -s $IDENTITY -i $BUNDLEID --entitlements /tmp/entitlements.child "$OUTDIRECTORY/$NAME.app/Contents/Frameworks/$NAME Helper EH.app"
codesign --deep -s $IDENTITY -i $BUNDLEID --entitlements /tmp/entitlements.child "$OUTDIRECTORY/$NAME.app/Contents/Frameworks/$NAME Helper NP.app"
codesign --deep -s $IDENTITY -i $BUNDLEID --entitlements /tmp/entitlements.parent "$OUTDIRECTORY/$NAME.app/Contents/MacOS/$NAME"

# validate entitlements
echo "==Validating entitlements and Mac App Store needs=="
codesign -dvvv --entitlements :- "$OUTDIRECTORY/$NAME.app/Contents/Frameworks/$NAME Helper.app/Contents/MacOS/$NAME Helper"
codesign -dvvv --entitlements :- "$OUTDIRECTORY/$NAME.app/Contents/Frameworks/$NAME Helper EH.app/Contents/MacOS/$NAME Helper EH"
codesign -dvvv --entitlements :- "$OUTDIRECTORY/$NAME.app/Contents/Frameworks/$NAME Helper NP.app/Contents/MacOS/$NAME Helper NP"
codesign -dvvv --entitlements :- "$OUTDIRECTORY/$NAME.app/Contents/MacOS/$NAME"

# validate code signatures
echo "==Validating code signature and subsequent resources=="
spctl --assess -vvvv "$OUTDIRECTORY/$NAME.app/Contents/Frameworks/$NAME Helper.app"
spctl --assess -vvvv "$OUTDIRECTORY/$NAME.app/Contents/Frameworks/$NAME Helper EH.app"
spctl --assess -vvvv "$OUTDIRECTORY/$NAME.app/Contents/Frameworks/$NAME Helper NP.app"
spctl --assess -vvvv "$OUTDIRECTORY/$NAME.app"