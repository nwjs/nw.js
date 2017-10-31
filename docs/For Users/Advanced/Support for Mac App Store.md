# Support for Mac App Store {: .doctitle}
---

[TOC]

## Overview

You can distribute your macOS apps through official Mac App Store or outside the store. But the apps should always be signed before distribution. Unsigned apps are refused to be launched by the [Gatekeeper](https://support.apple.com/en-us/HT202491).

This guide will show you how to sign NW.js based apps for macOS.

## Prerequisits

* Create a macOS app through [iTunesConnect](https://itunesconnect.apple.com)
* Obtain Application & Installer certificates from [Apple Developer](https://developer.apple.com).
    - If you distribute your app through **Mac App Store**:
        + 3rd Party Mac Developer Application: Foo (XXXXXXXXXX)
        + 3rd Party Mac Developer Installer: Foo (XXXXXXXXXX)
    - If you distribute your app **outside the store**:
        + Developer ID Application: Foo (XXXXXXXXXX)
        + Developer ID Installer: Foo (XXXXXXXXXX)

## Build the App

Download NW.js MAS build from [nwjs.io](http://dl.nwjs.io/v0.19.5-mas-beta/) and build your app as described in [Package and Distribute](../Package and Distribute.md).

## Sign the App

`build_mas.py` is used to sign your app for macOS. And the script can generate an uploadable `.pkg` file for Mac App Store after signing by giving `--pkg` argument.

**Basic Usage**

```bash
python build_mas.py -C build.cfg -I myapp-dev.app -O MyApp.app
```

## Configuration File Format

Configuration file (`build.cfg`) is a human readable text file. It contains important settings for signing and packaging the app.

`ApplicationIdentity` and `InstallerIdentity` are the names of the certificates used for signing and packaging your app. See [Prerequisits](#prerequisits) for which certificates you need.

`NWTeamID` is used to establish IPC channels for launch NW.js based app. It can be obtained from Apple Developer -> Membership -> Team ID.

`ParentEntitlements` and `ChildEntitlements` should be valid [entitlements files](https://developer.apple.com/library/content/documentation/Miscellaneous/Reference/EntitlementKeyReference/Chapters/AboutEntitlements.html). By default, your app is signed with minimal privileges as below.

**entitlements-parent.plist**

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>com.apple.security.app-sandbox</key>
  <true/>
  <key>com.apple.security.application-groups</key>
  <string>NWTeamID.your.app.bundle.id</string>
</dict>
</plist>
```

**entitlements-child.plist**

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>com.apple.security.app-sandbox</key>
  <true/>
  <key>com.apple.security.inherit</key>
  <true/>
</dict>
</plist>
```

Read the sample `build.cfg` for detailed meanings all fields.