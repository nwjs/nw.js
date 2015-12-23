# Debugging with DevTools {: .doctitle}
---

!!! note "SDK Flavor Only"
    DevTools is only available in [SDK flavor](Advanced/Build Flavors.md). It's recommended to use SDK flavor to develop and test your app. And use other flavors for production.

## Open Developer Tools

DevTools can be opened with keyboard shortcut <kbd>F12</kbd> for Windows and Linux or <kbd>&#8984;</kbd>+<kbd>&#8997;</kbd>+<kbd>i</kbd>.

Alternatively, you may open DevTools programmatically using NW.js API [win.showDevTools()`](../References/Window.md##winshowdevtoolsiframe-headless-callback) for a window.

## Remote Debugging

You can use the `--remote-debugging-port=port` command line option to specify which port the DevTools should listen to. For example, by running `nw --remote-debugging-port=9222`, you can open http://localhost:9222/ in your browser to visit the debugger remotely.
