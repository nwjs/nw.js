0.13.4 / 04-08-2016
===================
- Update Chromium to 49.0.2623.112
- Fix: webview local file issue [(#4668)](https://github.com/nwjs/nw.js/issues/4668)
- Fix: Custom app icon with incorrect size under volume mixer [(#4649)](https://github.com/nwjs/nw.js/issues/4649)
- Fix: process Event: 'uncaughtException' not working in 0.13 [(#4679)](https://github.com/nwjs/nw.js/issues/4679)
- [docs] data-path -> user-data-dir; --disable-raf-throttling
- [docs] Clarify files can be safely removed during packaging [(#4676)](https://github.com/nwjs/nw.js/issues/4676)

0.13.3 / 04-04-2016
===================
- Update Node.js to 5.10.0
- support --disable-raf-throttling to enable RequestAnimationFrame for background apps (#4664)
- Fix: App will crash if CFBundleIdentifier in plist longer than 30 chrs (#4638)
- Fix: Crash when open a pdf file in webview. (#4644)
- Fix: [WIN] window restore size is changed when aero glass is on (#4323)
- Fix: inject_js_end not working on remote pages (#4656)
- Fix: Queries were sent to Google to update plugin metadata (#4669)
- Fix: [WIN] Window size includes title bar (#4627)
- Fix: Crash when bg-script calls nw.Window.get() (#4368)
- Fix: Disabled menu item, still accessible via hotkey (#2261)
- Fix: Unable to turn off / disable debug.log (#4637)
- [docs] data-path -> user-data-dir
- [docs] add link to buildbot
- [docs] using flash plugin
- [docs] location of nwjc and chromedriver

0.13.2 / 03-29-2016
===================
- Update Chromium to 49.0.2623.110
- Fix: crashes in CloseWatcher::CallbackAndDie sometimes (#4636)
- Fix: nwsaveas is ignored when nwworkingdir is specified in nw13 (#4630)
- Fix: "Inspect" Entry in Non-SDK PDF Viewer's Context Menu (#4642)

0.13.1 / 03-25-2016
===================
- Update Chromium to 49.0.2623.108
- Fix: Performance degradation in constructing typed array from node buffer (#4614)

0.13.0 / 03-23-2016
===================
- Fix: [OSX] Cmd-Q does not pass true as the first argument to the event after createMacBuiltin() (#4580)
- Fix: Window event 'move'/'resize' callback arguments undefined (#4610)
- [docs] Update Command Line options with full list
- [docs] List Chrome Extension APIs

0.13.0-rc4 / 03-21-2016
=======================
- Update Node.js to 5.9.0
- Fix: [WIN] unable to launch on XP 64bit system (#4600)
- Fix: Crash when using `additional_trust_anchors`
- Fix: Screen capture does not work in remote domain (#4579)
- Fix: webview events not firing in remote page (#4590)
- Fix: Crash when adding folder to workspace in devtools (#4602)
- Fix: [OSX] Crash when using nw.Menu with chrome.identity.launchWebAuthFlow (#4591)
- Fix: Crash when trying to open tabbed browser window (#4587, #4540)
- Fix: Only top left corner of window icon is shown (#4581)
- Fix: [OSX] Crash dumps doesn't match hashes of symbol files (#4450)
- Fix: Crash on promise rejection handler (#4577)
- Fix: Menu.popup blocks program (#4593)
- Fix: [OSX] Cmd-Q does not pass true as the first argument to the event (#4580)
- Fix: capturePage always return PNG (#4609)
- [WIN] dump node.dll symbols
- Remove context menu of notification (#4598)
- Remove settings button of notification (#4582)
- [docs] update crash dump location
- [docs] add link for enable logging in FAQ
- [docs] Reorder the navigation pane (#4576)
- [docs] Multiple language resources in SDK build

0.13.0-rc3 / 03-15-2016
=======================
- Enable auth password dialog
- Enable notification permission for all origin (#4521, #4533)
- Support chrome.contentSettings API (#4521)
- Fix: wrong version and info for Pepper Flash plugin in registry (#4530)
- Fix: [WIN] fix: window title and icon not shown
- Fix: [WIN] start up fail where crashpad path is not found
- Fix: nwjs 0.13.0 showed as chromium in taskbar on Linux (#4537)
- [docs] more info on bg-script
- [docs] transparecy needs frameless

0.13.0-rc2 / 03-11-2016
=======================
- Update Chromium to 49.0.2623.87
- Update Node.js to 5.8.0
- Ship .pak files for non-English languages in SDK build
- Build ffmpeg as Dll (#4492)
- Disable notification of screen sharing (#4484)
- Migrate indexed db files in user data dir (#4469)
- Migrate local storage files for app protocol in user data dir (#4527)
- Fix: node-main is broken (#4495)
- Fix: app asks for confidential information for NWJS Safe Storage in keychain (#4458)
- Fix: crash when navigate in some sites (#4422)
- Fix: Use window subfields in manifest for newly created windows (#4221)
- Fix: After press the “command+h” to minimize, but unable to recover (#4296)
- Fix: crashpad handler error in windows (debug build unable to start)
- Fix: Window title still shows with frame set to false on OSX (#4507)
- Fix: The 'width' and 'height' in the manifest file does not work properly on OSX (#4493)
- Fix: crash in chrome.app.window.create(url) without 2nd argument
- Fix: nw.Window broken on 0.13.0-rc1 with --mixed-context (#4498)
- Fix: Make menu callback work after navigation (#4313)
- Fix: Show menubar menus from top-left anchor (#4473)
- [docs] add 'Customize menubar' (#4416)

0.13.0-rc1 / 03-03-2016
=======================
- Update Chromium to 49.0.2623.75
- Update Node.js to 5.7.0
- Add `nw.Screen.DesktopCaptureMonitor.registerStream()` for using stream id in `getUserMedia()` (#4459)
- Fix: Wrong "restore" window size after click on the app in the taskbar on windows 10 (#4323)
- [docs] chromedriver: a better way to arguments of nw app

0.13.0-beta7 / 02-25-2016
=========================
- Update Chromium to 48.0.2564.116
- Support loading devtools in/for webview by adding webview.showDevTools (#4411)
- Fully support for devtools extensions (#612)
- support loading local file in webview (#4425)
- Fix: add URL scheme support for MAC (#4240)
- Fix: alert/prompt dialog with nwjs style (#4413)
- Fix: menubar button not found when navigate to another menu (#4371)

0.13.0-beta6 / 02-17-2016
=========================
- Update Chromium to 48.0.2564.109
- Update Node.js to 5.6.0
- support controlling window with external page
- Fix: enable self extraction (#4071)
- Fix: crash when using native modules in OSX (#4338)
- Fix: FileReader not working (#4399)
- Fix: console tab with DevTools jail not working until switch frame manually
- Fix: OSX menu support broken (#4374)
- Fix: JavaScript dialogs text (#4190)
- Fix: Typo: rename `App.(un)registerGlobalHotkey` to `App.(un)registerGlobalHotKey`
- Fix: Window.zoomLevel changes all windows (#4352)
- Fix: avoid polluting global namespace with 'path' etc. (#4350)
- add URL scheme support for MAC (#4240)
- support 'new_instance' option for Window.open
- support native modules built by node-gyp
- [docs] update for new_instance etc

0.13.0-beta5 / 02-01-2016
=========================
- Update Chromium to 48.0.2564.97
- Add back transparency support which is missing in beta4
- support program args in chromedriver
- port App.open argument (#4289)
- support webkit.plugin field in manifest
- Fix package script to support Win XP (#4181)
- [OSX] disable double tap with 2 fingers by default and add option for it (#4249)
- Fix: support window id in Window.open() (#4268)
- Fix: permission for webRequest API (#4220)
- rename 'inject-js-start' with 'inject_js_start' and add it to Window.open options (#4286)
- rename 'always-on-top' with 'always_on_top', and 'visible-on-all-workspaces' with 'visible_on_all_workspaces'
- Fix: infinite loop in nw.Window.close(true) (#4171)
- Fix: Window title not changed by setting document.title (#3589)
- Fix: startup slow because of scanning for invalid file name (#4303)
- Fix: [OSX] suppress warning when opening devtools
- Fix: conflict between promise rejection callbacks from Blink and Node (#4300)
- Fix: Taking heap snapshot crashes webkit (#3678)
- Fix: [OSX] App quits when closing window if window.show is false in manifest (#4187)
- Fix: crash when opening external links with browser in devtools (#4269, #4121)
- Fix: crash when opening external links with browser in print preview (#4270)
- Fix: cookies API (#4199)
- [docs] add enable-gcm
- [docs] add double_tap_to_zoom_enabled
- [docs] add FAQ for console.log
- [docs] Updated FAQ for AnuglarJS 2

0.13.0-beta4 / 01-18-2016
=========================
- Update Chromium to 48.0.2564.82
- Update Node.js to 5.4.1
- [OSX] Fix process.nextTick in some cases
- [OSX] Fix hidden window title (#4186)
- ported nw.App.fullArgv
- Fix: nw.App.quit() does nothing when use JS file as main (#4227)

0.13.0-beta3 / 01-13-2016
=========================
- Fix: save as... dialog not working (#4142)
- Fix: app with hidden window will quit on start on osx
- Fix: chrome socket api permission in NW app (#4143)
- Fix: some Node core modules not working after close window (#4130, #4140)
- Fix: chrome.fileSystem.restoreEntry breaks retainID on app reload (#4150)
- Fix: oauth not working due to browser side naviagation (#4179, #4131, #4184)
- Fix: Exception in nw.dll when running native module serialport (#4152)
- Fix: window.open not loaded using node-remote (#4180)
- Fix: POST form not working on navigation
- Fix: nwjs Helper process not closed (#4118)
- Fix: NW.js doesn't quit if DevTools has opened once (#4164)
- Fix: window not closed when listen to 'close' event in another window (#4138)
- Fix: [WIN] debug build of 'payload' tool
- Fix: Crash when using devtools to view some source (#4121)
- Fix: nw.Window.get() returns null after navigation (#4157, #4007)
- Fix: requrie() function in the shim for nw12 compatibility (#4141)
- Fix: Reversed x and y in win.moveTo and win.moveBy (#4136)
- Accept array of matched patterns in node-remote (#4132)
- [doc] Added docs for win.navigation event
- [doc] Updated docs for single-instance
- [doc] add hint for document contribution
- [doc] update for Multiple Context mode
- [doc] For Users > Getting Started. Revise example 2 sample code to cater new NW.JS API for Menu
- [doc] update for Window.id

0.13.0-beta2 / 12-31-2015
=========================
- Fix: application window is opened out of the screen on Windows.

0.13.0-beta1 / 12-30-2015
=========================
- Most of nw.gui API ported

0.12.3 / 07-31-2015
===================
- Support Mac App Store with the 'macappstore' flavor
- [WIN] Screen.DesktopCaptureMonitor API: https://github.com/nwjs/nw.js/wiki/Screen#screendesktopcapturemonitor (Thanks to Rick Edgecombe)
- [HighDPI][WIN] fix for Tray menu is huge on High-DPI Windows machine (#2847) (Thanks to Jefry)

0.13.0-alpha2 / 06-23-2015
==========================
- support Mixed Context mode (#3107)
- Window.capturePage, eval and evalNWBin ported
- 'inject-js-start' and 'inject-js-end' ported
- support setting manifest in 'new-win-policy' callback
- Window 'loading' and 'loaded' event ported

0.13.0-alpha1 / 06-11-2015
==========================
- [nw12] Fix OSX cpu hog in some cases
- [nw12] support sudo in Linux
- Fix: (windows) need to specify full path to the package directory as the command line argument
- use current process on navigation
- support Spell checking with '--enable-spell-checking' switch
- support extension API chrome.proxy
- command line switch '--url' ported
- enable local storage in NW.js app
- save/restore application window geometry between sessions
- don't open URL with external program
- 'new-win-policy' event ported (setting manifest in the callback is yet supported)
- chromedriver fixed
- don't rewrite process title
- support 'Window.navigation' event
- support JS dialogs (alert(), etc)
- allow all API permission in NW.js app
- Clipboard API ported
- Fix "__filename" and "__dirname" in "nw" object for Node compatibility
- Fix OSX packaging way of putting files in 'app.nw' dir
- New test framework

0.13.0-alpha0 / 05-10-2015
==========================
- Change to a slightly new architecture
- Support more chrome.* APIs
- Support Chrome App manifest
- Enable more features like print preview, client certificate, etc.
- Support message center based notifications

0.12.2 / 05-22-2015
===================
- Fix #2723: [OSX] cpu hog in some cases
- Fix #3361: application cache
- Fix #2720: [Linux] launching sudo hits error: effective uid is not 0
- Fix #2819: enable cookie support for web sockets
- Fix #2713: crash with 'new-win-policy' and opening window from iframe
- Fix #3123: support no-displaying-insecure-content and allow-running-insecure-content
- [Screen Selection] add application name to the UI; cancelChooseDesktopMedia implementation
- [Notification] [WIN] disable audio for toast notification, better fallback for toast notification
- Change cache backend from "simple" to "blockfile"

0.12.1 / 04-13-2015
===================
- Fix crash dump generation
- [WIN] Fix blurry text with High DPI display
- Fix: Webview : contentWindow not available at this time (#3126)
- More precise RegExp for App.argv filtering (Thanks to Anton Khlynovskiy)
- Fix #3143: remote debugging devtools page blank (Thanks to Yuehang Wu)
- [Notification][Win] fix for missing windows events
- add Window 'progress' event (Thanks to vadim-kudr <x.kydrik.x@gmail.com>)
- nw-headers is built automatically now in buildbot (Thanks to Xue Yang)

0.12.0 / 03-05-2015
=======================
- Chromium updated to 41.0.2272.76
- [Screen Selection] OSX and Win implementation (Thanks to Jefry)
- Fix #3165: crash on auth in webview

0.12.0-rc1 / 02-27-2015
=======================
- new 'nwjc' tool replaces 'nwsnapshot'; size limit removed
- add Window.evalNWBin() to work with nwjc
- Fix #2923: support pepper flash plugin on Linux
- Fix #2961: nwdirectory file dialog
- Fix #2996: setting breakpoints in Node context in devtools

0.12.0-alpha3 / 02-13-2015
==========================
- Chromium updated to 41.0.2272.32
- io.js updated to 1.2.0

0.12.0-alpha2 / 01-18-2015
==========================
- Fix: -webkit-app-region: drag; stopped working in version 0.12.0-alpha1 #2963
- Fix: [WIN] ReferrenceError in native module function createWritableDummyStream #2933
- support bypassing frame-ancestors CSP in Node frame #2967

0.12.0-alpha1 / 01-15-2015
==========================
- renamed NW.js
- Chromium is updated to 41.0.2236.2
- migrated to io.js 1.0.0
- new chrome.webrequest API
- new 'webview' tag from Chrome extensions
- new 'bg-script' field in the manifest

0.11.3 / 12-16-2014
===================
- new method in 'new-win-policy' event handler to control the options for new popup windows
- Fix: nw methods cannot be called from normal frames
- Extend Tray click event with position (Thanks to Marco Fabbri) (#1874)
- [OSX] Fix Window.focus() not taking focus (#2724)
- Add API methods and support for styling of icons (Tray, MenuItem) under Mac OS X (Yosemite) Dark Mode (#2775)
- [OSX] Fix alticon property of Tray not being updated properly (#703)
- Add Window.setVisibleOnAllWorkspaces API (#2722)
- Fix #2469: Changed Window.open to ignore slashes in parameters
- fix crash in window.open in some cases

0.11.2 / 11-26-2014
===================
- Support window transparency (#132, Thanks to Jefry Tedjokusumo)
- Fix: [Linux] broken window events (focus, blur, etc, #2631)
- Fix: memory leak on setting tray icon (#2666)
- Fix: child_process.fork() (#2664)
- Fix: bad Buffer created from strings from DOM (#1669, #2439) (Thank to Liu Cong)
- Fix: Segmentation fault by starting nw on command line with parameters #2671
- Fix: crashes if http.request gets blocked with Little Snitch (mac only) #2585
- Fix: Windows 7 64/32 - frame doesn't show #2657
- Fix: AutoFill Crashes NodeWebkit #2653
- Fix "Cancel Desktop Notification" for all platform. and implement it for win8 (toast notification)

0.11.1 / 11-20-2014
===================
- add nwsnapshot
- Support setting additional root certificates on supported platforms (thanks to Joachim Bauch)
- Support SetProxyConfig API (#916)
- [WIN] Fix startup crash on high DPI systems (#2649)
- Fix #1021: maximize frameless window in windows 8
- Fix #2590: save as Filetypes not populating
- Fix #2592: zoomLevel
- Fix #2595: Linux MenuBar crash
- Fix #2393: link target with "_blank" opens page in current window
- Fix: Don't activate app unconditionally on window "Show".

0.11.0 / 11-11-2014
===================
- Fix: notification and screen geometry API
- Fix: windows printing crash (#2515)
- Fix: mac: Fix build with 10.9 SDK
- Fix: enable 'high-dpi-support' for windows (#2524)
- Fix: 'resizable' is broken in manifest (#2319)
- Fix: crash on Flash context menu
- Fix: console.log() changes value (#2431)
- Fix: various crash cases (#2545, #2549)
- Fix: 'undefined' network request in devtools (#2529)
- Fix: devtools - breakpoints do not work (#2538)
- Fix: Jailed devtools broken in nw 0.11.0-rc1 (#2569)
- Fix: Window.setResizable(false) twice makes window resizable (#2299)
- Fix: win.setPosition('center') Invalid (#2307)

0.11.0-rc1 / 10-27-2014
=======================
- Chromium updated to 38.0.2125.104
- Fix memory leak on navigation
- Show commit id in 'nw:version' page
- Fix: fullscreen in manifest (Linux)
- Fix: #430: handle event when quit from OSX dock

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
