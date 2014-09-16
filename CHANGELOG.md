0.10.5 / 09-16-2014
===================
- Fix: support more Chromium command line args (#1743, Thanks to Joachim Bauch)
- Fix #2171: crash when opening window 
- Fix #2326: some websites crashes NW in Windows (fixed with the same file as updating to VS2013 Update 2)
- Fix: win: crash on invalid parameter error (thanks to Mikael Roos)
- Fix #1991 in a better way: [WIN] stalling for seconds under threaded composition on some hardware (#1991)
- Fix: [WIN] single-instance crash
- Fix: autofill crash when filling number in input box (#2310)
- Fix: CSP is not effective (#1672)
- Fix: crash when calling console.log() with a cross-domain window object in some cases (#1573)
- Fix: crash when stepping into a breakpoint set in scripts loaded by require() (#2214)

0.10.4 / 09-05-2014
===================
- Fix: [WIN] child_process.fork() by making nw executable run as node
- Fix: [WIN] stalling for seconds under threaded composition on some hardware (#1991)
- Fix: [OSX] disable File Quarantine (#2294)
- Fix: [OSX] disable sound for notification
- support 'chrome://gpu' diagnosic page

0.10.3 / 09-01-2014
===================
- Fix: child_process.fork() (#213) by making nw executable run as node
- Fix: [OSX] process.nextTick() blocked in some cases (#2170)

0.10.2 / 08-12-2014
===================
- Support screen geometry API (#2178 Thanks to Jefry Tedjokusumo
- Support progress bar (#2175 Thanks to Jefry Tedjokusumo)
- Window.requestAttention() now accepts an integer parameter [4]
- Fix: JS source code snapshot is now working
- Fix: Linux: shift modifier not working for window menu
- Fix: Win: window.navigator.language is empty
- Fix: require works in wrong path in new-instance window (#2167)
- Fix: support for screencapture media requests (Thanks to Joachim Bauch)
- Fix: Win: cursor not loaded in some cases (#2150, #2152)
- Fix: crash on some cases (#2155, #2148)
- Fix: Large combo-box does not scroll properly and values overlaps on each other (#2161; upstream #357480)

0.10.1 / 07-30-2014
===================
- Support Desktop notification (#27 Thanks to Jefry Tedjokusumo)
- Support Fullscreen API (#55)
- Official 64bit binary for Mac OS X (#296)
- Support F-keys in global shortcut (Thanks to Bas Wegh)
- Option to hide "Edit" and "Window" OS X menus (Thanks to Eric Newport)
- Fix #2072: [WIN] context menu popup in wrong (screen) position
- Fix #2136: crash when popup new window in some cases
- Fix: Linux symbol files are incomplete in 0.10.0
- Fix #1908: allows redirection to App protocol for OAuth usage
- Fix: new-win-policy is fired on each navigation

0.10.0 / 07-22-2014
===================
- Fix: [WIN] download dialog
- Fix: [WIN] MenuItem.enabled and other properties needs to be called twice to work (#1132)

0.10.0-rc2 / 07-18-2014
=======================

- [API] Implement register/unregister global desktop keyboard shortcut. (#1735, thanks to Chaobin Zhang and Haojian Wu)
- support for nwsnapshot is back
- Fix: authentication dialog
- check invalid URL on navigation
- [OSX] Fix #1996: resize with flash on 10.9 (upstream #385120)
- Fix regression: node-main
- Fix regression: [WIN] resize and drag frameless window
- Fix: crash on Win XP (#1985)

0.10.0-rc1 / 06-22-2014
=======================

- Chromium is updated to 35.0.1916.113
- Node.js is updated to 0.11.13
- Window event listeners in iframe works now
- New API: App.addOriginAccessWhitelistEntry & App.removeOriginAccessWhitelistEntry (#1016, #1514)
- API to set Menu item shortcut
- [OSX] Create & access to built-in menu from JS
- SetBadgeLabel support on OSX and Win
- Support for 'single-instance' when app is started in folder
- better handling of MIME types and dataURI in capture page API


0.8.6 / 04-18-2014
==================
- Fix: new instance window is force closed and cannot receive 'close' event on App.closeAllWindows() (Cmd-Q) (#1713, #1778)

0.8.5 / 02-26-2014
==================
Backport 0.9 features to 0.8 branch

- Support `'inject-js-start'` and `'inject-js-end'` in manifest (#1585)
- `'new-win-policy'` event now works for windows opened by `nwgui.Window.open()` (#1519)
- `'new-win-policy'` handler supports Ctrl click and middle click (#1547)
- Support NTLM and the settings (`--auth-server-whitelist`, `--auth-schemes`, etc) (#590)
- Support app frameworks like AngularJS better by returning HTTP response code in `'app://'` protocol handler (#1314)
- Injecting JavaScript in window or iframe in various cases:
    - 'inject-js' option in Window.open or manifest 
    - 'document-start' and 'document-end' event for iframe: 
    - Window.eval() to execute JavaScript in target window or iframe 
- Handler to decide how new window is request from iframe, see 'new-win-policy' event in https://github.com/rogerwang/node-webkit/wiki/Window 
- Overriding 'User-Agent' in iframe: https://github.com/rogerwang/node-webkit/wiki/Changes-to-dom#nwUserAgent 

0.9.2 / 02-20-2014 
==================

- Support skipping taskbar or dock: `Window.setShowInTaskbar(bool)` is added. It can be set from manifest as well. (#1076) (Thanks to Zhang Chaobin & Zhao Zeyi)
- Support `'inject-js-start'` and `'inject-js-end'` in manifest (#1585)
- `'new-win-policy'` event now works for windows opened by `nwgui.Window.open()` (#1519)
- `'new-win-policy'` handler supports Ctrl click and middle click (#1547)
- Support NTLM and the settings (`--auth-server-whitelist`, `--auth-schemes`, etc) (#590)
- Support app frameworks like AngularJS better by returning HTTP response code in `'app://'` protocol handler (#1314)
- Fix: crashing when setting DOM event handler `global.window.onkeydown` from Node context (#1562)


0.9.1 / 02-10-2014 
==================

- Chromium is updated to 32.0.1700.107 
- [OSX] `process.nextTick` will not fire 
- Fix: crashing on `alert()` 


0.9.0 / 02-08-2014 
==================

- Chromium updated to version 32
- Node.js updated to v0.11
- nw-gyp updated to v0.12.2 to support VS2013 (Thanks to Robert Konrad) 
- Injecting JavaScript in window or iframe in various cases:
    - 'inject-js' option in Window.open or manifest 
    - 'document-start' and 'document-end' event for iframe: 
    - Window.eval() to execute JavaScript in target window or iframe 
- Better experience for working with native modules if they support Node v0.11.
- Handler to decide how new window is request from iframe, see 'new-win-policy' event in https://github.com/rogerwang/node-webkit/wiki/Window 
- Overriding 'User-Agent' in iframe: https://github.com/rogerwang/node-webkit/wiki/Changes-to-dom#nwUserAgent 
- Disable uploading of crash dumps in 0.9.0-rc1 
- Don't use "Upload" in directory file dialog UI (#1457) 
- Remove '--enable-experimental-web-platform-features' from default cmdline to render pages correctly in OSX in some cases 
- Call Node's uncaughtException handler in Node frames 
- Export v8 symbols properly for native modules (#1522) 
- Enable scripts to emulate user gestures 
- Fix: Uncaught exception warnings of SetZoomLevel 
- Fix: Crashing on window.print() (#1545) 
- Fix: Clicking the app reload button followed by a call to the cookies api causes a crash (#1146)
- Fix: Can't close NW after refresh while using Dev Tools on Windows (#1330)
- Fix: Fatal error in ../../v8/src/mark-compact.cc, line 2751 (#1379)


0.8.4 / 12-30-2013 
==================

- Fix for OSX: the message loop integration between Chromium and Node.js has improved.
- Fix: Need a way to differentiate Cmd-Q from closing window(s) (#430) 
- Fix: Fix the way we integrate libuv to NSApp (#82) 


0.8.3 / 12-20-2013 
==================

- Remove duplicate Node.js license notice (#1178) 
- Fix: maximize and unmaximize event not fired. (#753) 
- Fix: crash in ClearCache in some cases 
- Fix: Fast open&close devtools crashes node-webkit 0.8.2 on OSX (#1391) 
- Fix: `focus` doesn't work on devtools window (#1387) 


0.8.2 / 12-06-2013 
==================

- Support native Node.js module better by adding deprecated MakeWeak function (#533 #1355). Some native modules can be built with 'nw-gyp' now. 
- Support 'move' and 'resize' events of the Window object (#799) 
- Event support on devtools window object 
- HTTP (proxy) login dialog will be shown for only once on multiple initial requests. 
- [OSX] Turn on `--enable-threaded-compositing` by default (#1340): this fixes the flash for CSS3 animation 
- Cherry-pick upstream fix: Update OOM killer to patch out CFAllocator on 10.9 (#1271 #1277) 
- Crashes when open some window again (#1364) 
- Do not quit on API call on invalid object (#1339) 


0.8.1 / 11-22-2013 
==================

- Node updated to 0.10.22 
- New Window.cookie API: It's for manipulating cookies. See https://github.com/rogerwang/node-webkit/wiki/Window 
- Window.showDevTools() now returns a Window object for the devtools window. So it can be moved/resized etc. Note that the events on this Window is not working yet. 
- Fix: WebRTC audio gets choppy because the device is enumerated twice (#1270) 
- Fix: Screen capture regression (#1309) 
- Fix: [WIN] crash sometimes when Back/Forward key is received 
- Fix: Crash when access xhr.response as ArrayBuffer from Node context 
- Fix: [security] app:// protocol available in iframe with nwdisable (#1255) 


0.8.0 / 10-30-2013 
==================

- Chromium updated to 30.0.1599.66 (now shown in nw:version) 
- Support crash dumping: if node-webkit crashes, your users or you can attach the dump file in bug reports. It contains stack trace information which is helpful to locate the root cause. See https://github.com/rogerwang/node-webkit/wiki/Crash-dump 
- i18n of built-in mac menus: the built-in menus created for Mac are now translated to 53 languages with strings from upstream. For the list of languages, see https://github.com/rogerwang/node-webkit/tree/master/src/resources/locale 
- Linux: environment variable override for the proxy settings. Use '--v=1' to see whether it's applied from the environment or GSettings. The following environment variables are supported: 'all_proxy', 'http_proxy', 'https_proxy', 'ftp_proxy', 'no_proxy', 'SOCKS_VERSION', 'SOCKS_SERVER'. 
- Fix: Date pickers language is always English regardless of current locale. (#669) 
- Fix: File dialog's language is always English regardless of current locale.(#103) 
- Fix: gui.App error in CallStaticMethodSync when using multiple windows (#1187) 
- Fix: console.log() output in terminal. 
- Fix: -webkit-app-region: drag don't work any more once it maximized. 
- Fix: crash on using filesystem API: quota and persistent storage 
- nw.gui.Window: add 'devtools-closed' event; always send 'devtools-opened' event. 
- nw.gui.Window: add 'isDevToolsOpen()' to query the status of devtools. 
- Support media enumerable API from upstream 30 (#632) 
- Win: embedding manifest to fix tooltip 
- Override lang setting from cmdline argument (--lang) 
- Use current locale in Header 'Accept-Language' (#1240) 
- Mac: Fallback to 'en-US' when locale pak file is missing and don't quit. 
- Fix crash on exit: PageClickTracker is deleted twice 
- Linux: make frameless window resizable (#142) 
- Fix crash when there is apostrophe in the full path (#1206) 
- Fix Devtools: Use the most recent version of script for the same URL 
- undefine window.webkitSpeechRecognition before it's supported 

0.7.2 / 08-26-2013 
==================

- Window.showDevTools() accepts iframe object as the first parameter 
- Fix: Mac - 0.7.x crash after copy/paste/edit (keypress) input field (introduced in upstream) (#1017) 
- Fix: 0.7.0 nw.exe still in memory after using a "close/quit" command bug (#984) 
- Fix: have some means to disable the debug.log file; debug.log file is now disabled by default. Use '--enable-logging' to turn it on. (#1031) 

0.7.1 / 08-19-2013 
==================

- Enable the screen capture support for getUserMedia (#576) 
- nw-gyp updated to 0.10.9 to work with latest Node (v0.10.16) 
- Fix regression: app cannot start from path contains '\x' (win) 
- Fix: A query string in app:// address in "main" property inside the manifest freezes node-webkit (#990) 
- Fix: nw.exe still in memory after calling 'App.quit()' in some cases (#984) 
- Fix: window unmaximizes when show is called (win) (#987) 
- Fix: Application Crash when quitting with secondary window on top (OSX) (#486) 
- Fix: Headers file is updated to allow building native modules 
- Report a clear message to console when nw.pak is missing 


0.7.0 / 08-12-2013 
==================

- Support `app://` protocol (#363) 
- Support setting value for file input (#927) 
- New API: get the values from manifest with `App.manifest`
- Changed API: full command line is passed as the parameter of `App.open` event 
- Fix: remove `alert()` dialog title on OSX 
- Fix: Sometimes `Window.focus()` doesn't work (#933) 
  

0.6.3 / 07-23-2013 
==================

- Add App.getProxyForURL() to query proxy for URL (#130)
- Save breakpoints and settings in devtools (#98)
- [Win] keep the ampersands character in dialogs (#901)
- Fix: can't access `file://` URIs when using a remotely hosted app (#871)
- Fix: devtools freeze on breakpoints in Ubuntu 12 VM


0.6.2 / 07-08-2013 
==================

- Node.js updated to v0.10.12
- New way to start applications: placing 'package.nw' in the same directory with node-webkit executable. NW will search for app as proposed in #843.
- [Mac] show the window without the focus side effect (#497)
- [Win] embed manifest to apply visual style for showing tooltip (#72 #810)
- Fix: `'require()'` is not defined when opening devtools (#809)
- Fix `'dom_storage_quota'`
- Fix regression of using datalist for autocompletion (#434)
- Fix regression: node modules cannot be stopped in debugger (#719)


0.6.1 / 06-23-2013 
==================

- Use 'App.dataPath' to get the application's data path in user's directory
- Fixed regression: source map in devtools is working after setting `"node-remote"` to `"127.0.0.1"` in manifest (#727)
- 'App.open' event is sent to all the windows now (#787)
- 'dom_storage_quota' in manifest is fixed


0.6.0 / 06-17-2013 
==================

- Chromium updated to 28.0.1500.11.
- A new feature "Devtools jail" is introduced for IDE developers.
- A new API function `App.clearCache()` is added to clear HTTP cache in both memory and disk.
- A new field in manifest `dom-storage-quota` to specify the number of mega bytes for the quota of the DOM storage. 
- Chromedriver binary is provided to use automatic testing tools such as `selenium` with node-webkit applications.
- Fix: Crash when using devtools (#793)
- `dom-storage-quota` in manifest is renamed to `dom_storage_quota`
- Using correct icon size on Windows to fix the one in win7 audio mixer
- Running nw from the command line on Mac always opens a second instance (#726) 
- How to clear the page cache in js? (#672) 
- OSX Page redrawing issues with twitter bootstrap (#476) 
- NW + WebGL (three.js) problem (#452) 
- Canvas freezes window on OSX (#381) 
- Popping up a menu twice will crash nw on ubuntu. (#721)
