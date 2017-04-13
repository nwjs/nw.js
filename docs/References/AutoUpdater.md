# AutoUpdater {: .doctitle}
---

[TOC]

`AutoUpdater` Enable apps to automatically update themselves.


The `AutoUpdater` module provides an interface for the
[Squirrel](https://github.com/Squirrel) framework.

You can quickly launch a multi-platform release server for distributing your
application by using one of these projects:

- [nuts][nuts]: *A smart release server for your applications, using GitHub as a backend. Auto-updates with Squirrel (Mac & Windows)*
- [squirrel-updates-server][squirrel-updates-server]: *A simple node.js server
  for Squirrel.Mac and Squirrel.Windows which uses GitHub releases*
- [squirrel-release-server][squirrel-release-server]: *A simple PHP application for Squirrel.Windows which reads updates from a folder. Supports delta updates.*

## Synopsis
```javascript
// listen to error events
nw.AutoUpdater.on('error', function(msg){
  console.log(msg);
});

// listen to update-downloaded event
nw.AutoUpdater.on('update-downloaded', function(releaseNotes, releaseName, releaseDate, updateURL){
  console.log(releaseNotes);
  console.log(releaseName);
  console.log(releaseDate);
  console.log(updateURL);
  nw.AutoUpdater.QuitAndInstall();
});

// set the update feed url
nw.AutoUpdater.SetFeedURL('file:///tmp/nwjs.autoupdate.json');
// the real autoupdate
nw.AutoUpdater.CheckForUpdates();
```

## Platform notices

Though `AutoUpdater` provides a uniform API for different platforms, there are
still some subtle differences on each platform.

### macOS

On macOS, the `AutoUpdater` module is built upon [Squirrel.Mac][squirrel-mac],
meaning you don't need any special setup to make it work. For server-side
requirements, you can read [Server Support][server-support]. Note that [App
Transport Security](https://developer.apple.com/library/content/documentation/General/Reference/InfoPlistKeyReference/Articles/CocoaKeys.html#//apple_ref/doc/uid/TP40009251-SW35) (ATS) applies to all requests made as part of the
update process. Apps that need to disable ATS can add the
`NSAllowsArbitraryLoads` key to their app's plist.

**Note:** Your application must be signed for automatic updates on macOS.
This is a requirement of `Squirrel.Mac`.

We have a [test suite](../../test/manual/autoupdate) to quickly test if the autoupdate feature is working.
Run `autoupdate.sh` from the folder, it needs 2 parameters:
* `IDENTITY` is your developer id, to sign the app
* `DIRECTORY` is your nwjs.app location

### Windows

On Windows, you have to install your app into a user's machine before you can
use the `AutoUpdater`, so it is recommended that you use the
[electron-winstaller][installer-lib], [electron-builder][electron-builder-lib] or the [grunt-electron-installer][installer] package to generate a Windows installer.

When using [electron-winstaller][installer-lib] or [electron-builder][electron-builder-lib] make sure you do not try to update your app [the first time it runs](https://github.com/electron/windows-installer#handling-squirrel-events) (Also see [this issue for more info](https://github.com/electron/electron/issues/7155)). It's also recommended to use [electron-squirrel-startup](https://github.com/mongodb-js/electron-squirrel-startup) to get desktop shortcuts for your app.

The installer generated with Squirrel will create a shortcut icon with an
[Application User Model ID][app-user-model-id] in the format of
`com.squirrel.PACKAGE_ID.YOUR_EXE_WITHOUT_DOT_EXE`, examples are
`com.squirrel.slack.Slack` and `com.squirrel.code.Code`. You have to use the
same ID for your app with `app.setAppUserModelId` API, otherwise Windows will
not be able to pin your app properly in task bar.

The server-side setup is also different from macOS. You can read the documents of
[Squirrel.Windows][squirrel-windows] to get more details.

### Linux

There is no built-in support for auto-updater on Linux, so it is recommended to
use the distribution's package manager to update your app.

## AutoUpdater.SetFeedURL(url)

* `url` String

Sets the `url` and initialize the auto updater.

## AutoUpdater.GetFeedURL()

Returns `String` - The current update feed URL.

## AutoUpdater.CheckForUpdates()

Asks the server whether there is an update. You must call `setFeedURL` before
using this API.

## AutoUpdater.QuitAndInstall()

Restarts the app and installs the update after it has been downloaded. It
should only be called after `update-downloaded` has been emitted.

## Event: error (msg)

Emitted when there is an error while updating.

## Event: checking-for-update

Emitted when checking if an update has started.

## Event: update-available

Emitted when there is an available update. The update is downloaded
automatically.

## Event: update-not-available

Emitted when there is no available update.

## Event: update-downloaded (releaseNotes, releaseName, releaseDate, updateURL)

Emitted when an update has been downloaded, with following parameters:

* `releaseNotes` String
* `releaseName` String, macOS only, NOT available on windows
* `releaseDate` Date
* `updateURL` String

[squirrel-mac]: https://github.com/Squirrel/Squirrel.Mac
[server-support]: https://github.com/Squirrel/Squirrel.Mac#server-support
[squirrel-windows]: https://github.com/Squirrel/Squirrel.Windows
[installer]: https://github.com/electron/grunt-electron-installer
[installer-lib]: https://github.com/electron/windows-installer
[electron-builder-lib]: https://github.com/electron-userland/electron-builder
[app-user-model-id]: https://msdn.microsoft.com/en-us/library/windows/desktop/dd378459(v=vs.85).aspx
[squirrel-updates-server]: https://github.com/Aluxian/squirrel-updates-server
[nuts]: https://github.com/GitbookIO/nuts
[squirrel-release-server]: https://github.com/Arcath/squirrel-release-server
