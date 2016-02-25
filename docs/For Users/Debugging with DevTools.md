# Debugging with DevTools {: .doctitle}
---

!!! note "SDK Flavor Only"
    DevTools is only available in [SDK flavor](Advanced/Build Flavors.md). It's recommended to use SDK flavor to develop and test your app. And use other flavors for production.

## Open Developer Tools

DevTools can be opened with keyboard shortcut <kbd>F12</kbd> for Windows and Linux or <kbd>&#8984;</kbd>+<kbd>&#8997;</kbd>+<kbd>i</kbd>.

Alternatively, you may open DevTools programmatically using NW.js API [win.showDevTools()`](../References/Window.md##winshowdevtoolsiframe-headless-callback) for a window.

## Remote Debugging

You can use the `--remote-debugging-port=port` command line option to specify which port the DevTools should listen to. For example, by running `nw --remote-debugging-port=9222`, you can open http://localhost:9222/ in your browser to visit the debugger remotely.

## Using devtools extensions

Devtools extensions are fully supported, including the one for ReactJS etc. To use it, add the permission "chrome-extension://*" to manifest.json of the devtools extension, and load it with '--load-extension=path/to/extension' when nw is started. The files for devtools extensions can be copied from extension folder of Chrome browser after you install them from Chrome Web Store.

### Sample

https://s3-us-west-2.amazonaws.com/nwjs/sample/react-app.zip
https://s3-us-west-2.amazonaws.com/nwjs/sample/react-devtools.zip

Unpack them, download the SDK build and run it with: nw.exe --load-extension=path/to/devtools path/to/app/folder

The app is a simple react app with 'package.json' added. The devtools files are from the Chrome browser's extension folder of official react devtools extension installed from Chrome Web store. Only the manifest file is modified to add the permission: "chrome-extension://*".