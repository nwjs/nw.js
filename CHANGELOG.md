0.27.2 / 12-15-2017
===================
- Update Chromium to [63.0.3239.108](https://chromereleases.googleblog.com/2017/12/stable-channel-update-for-desktop_14.html)
- Update Node.js to [v9.3.0](https://nodejs.org/en/blog/release/v9.3.0/)
- Fix: extension with "all_frames": true option triggers false assertion [(#6324)](https://github.com/nwjs/nw.js/issues/6324)
- Fix: Webview with content scripts crashes on load non .swf object [(#6323)](https://github.com/nwjs/nw.js/issues/6323)
- Fix: user agent string handling for main resource of the page [(#6321)](https://github.com/nwjs/nw.js/issues/6321)
- Fix: Find in webview page freeze by enabling GuestViewCrossProcessFrames [(#6326)](https://github.com/nwjs/nw.js/issues/6326)

0.27.1 / 12-11-2017
===================
- Update Node.js to [v9.2.1](https://nodejs.org/en/blog/release/v9.2.1/)

0.27.0 / 12-07-2017
===================
- Update Chromium to [63.0.3239.84](https://chromereleases.googleblog.com/2017/12/stable-channel-update-for-desktop.html)
- Update Node.js to [v9.2.0](https://nodejs.org/en/blog/release/v9.2.0/)
- support loading compiled JS binary in Web Worker [(#6203)](https://github.com/nwjs/nw.js/issues/6203)
- support options for injecting in main or isolated world of webview ([#6299](https://github.com/nwjs/nw.js/issues/6299), [#6229](https://github.com/nwjs/nw.js/issues/6229))
- support disabling Flash plugin via chrome.contentSettings API [(#6216)](https://github.com/nwjs/nw.js/issues/6216)
- Fix: crashes with some native modules
- Fix: stale process with '--in-process-gpu' and Steam client [(#6059)](https://github.com/nwjs/nw.js/issues/6059)
- Fix: crashes when the specific PDF is loaded in the webview [(#6293)](https://github.com/nwjs/nw.js/issues/6293)
- Fix: process exit when native node module calls node::AtExit() [(#6047)](https://github.com/nwjs/nw.js/issues/6047)
- Remove: migrate indexdb from nw12
- Support compiling ES6 module source with nwjc '--nw-module'; Partly fix [#6303](https://github.com/nwjs/nw.js/issues/6303)
- [docs] executeScript in main world of webview
- [docs] use NW JS binary in workers

0.26.6 / 11-14-2017
===================
- Update Chromium to [62.0.3202.94](https://chromereleases.googleblog.com/2017/11/stable-channel-update-for-desktop_13.html)
- Fix: nw-gyp rebuild is not working on linux [(#5971)](https://github.com/nwjs/nw.js/issues/5971)
- Fix: transparent feature regression in some win7 system [(#6265)](https://github.com/nwjs/nw.js/issues/6265)
- Fix: transparent feature in some win10 system [(#6275)](https://github.com/nwjs/nw.js/issues/6275)

0.26.5 / 11-09-2017
===================
- Update Node.js to [v9.1.0](https://nodejs.org/en/blog/release/v9.1.0/)
- Allow inject script into main world of webview [(#6229)](https://github.com/nwjs/nw.js/issues/6229)

0.26.4 / 11-07-2017
===================
- Update Chromium to [62.0.3202.89](https://chromereleases.googleblog.com/2017/11/stable-channel-update-for-desktop.html)
- Fix: Frameless transparent window is maximizing/overflowing outside window limits [(#6248)](https://github.com/nwjs/nw.js/issues/6248)
- Fix: NW.js loading files does not seem to provide file's MIME type [(#6106)](https://github.com/nwjs/nw.js/issues/6106)
- Fix: Empty temp directory not properly deleted [(#5208)](https://github.com/nwjs/nw.js/issues/5208)
- Fix: Crash when opening some web page [(#6247)](https://github.com/nwjs/nw.js/issues/6247)

0.26.3 / 11-01-2017
===================
- Update Node.js to [v9.0.0](https://nodejs.org/en/blog/release/v9.0.0/)
- Fix: Cannot screenshare from within iframe Cross domain. InvalidStateError [(#6212)](https://github.com/nwjs/nw.js/issues/6212)
- Fix: Windows component build [(#6172)](https://github.com/nwjs/nw.js/issues/6172)

0.26.2 / 10-28-2017
===================
- Update Chromium to [62.0.3202.75](https://chromereleases.googleblog.com/2017/10/stable-channel-update-for-desktop_26.html)

0.26.1 / 10-26-2017
===================
- Update Node.js to [v8.8.1](https://nodejs.org/en/blog/release/v8.8.1/)
- Fix: WebSocket and resource loader throttling [(#6231)](https://github.com/nwjs/nw.js/issues/6231)
- Fix: Rendering glitches in High Sierra [(upstream bug #773705)](https://bugs.chromium.org/p/chromium/issues/detail?id=773705)
- Fix: Maximized frameless transparent window maintains right resize area exposed [(#6221)](https://github.com/nwjs/nw.js/issues/6221)
- Fix: Windows snap is lost with frameless transparent window and webkit-app-region drag [(#6219)](https://github.com/nwjs/nw.js/issues/6219)
- Fix: The frame border will be displayed when it is started [(#6217)](https://github.com/nwjs/nw.js/issues/6217)
- Fix: Remove settings button from native notifications on Linux [(#6194)](https://github.com/nwjs/nw.js/issues/6194)
- Fix: Cross-origin iframe doesn't have accessible "parent" and "top" properties in "window.parent" [(#6144)](https://github.com/nwjs/nw.js/issues/6144)

0.26.0 / 10-18-2017
===================
- Update Chromium to [62.0.3202.62](https://chromereleases.googleblog.com/2017/10/stable-channel-update-for-desktop.html)
- Update Node.js to [v8.7.0](https://nodejs.org/en/blog/release/v8.7.0/)
- Support scale option for printing [(#6166)](https://github.com/nwjs/nw.js/issues/6166)
- Fix: Maximizing an always on top transparent frameless window causes the bottom to overflow under the taskbar [(#6204)](https://github.com/nwjs/nw.js/issues/6204)

0.25.4 / 09-29-2017
===================
- Update Node.js to [v8.6.0](https://nodejs.org/en/blog/release/v8.6.0/)

0.25.3 / 09-23-2017
===================
- Update Chromium to [61.0.3163.100](https://chromereleases.googleblog.com/2017/09/stable-channel-update-for-desktop_21.html)
- Ship swiftshader on Linux & Win; Fix Failed to load libGLESv2.so [(#6142)](https://github.com/nwjs/nw.js/issues/6142)
- Fix: [macOS] crash if additional_trust_anchors provided [(#6135)](https://github.com/nwjs/nw.js/issues/6135)
- Fix: [macOS] Crash when called window nw.Window.get().setProgessBar() [(#6143)](https://github.com/nwjs/nw.js/issues/6143)
- Fix: Second instance hang with '--in-process-gpu' [(#6059)](https://github.com/nwjs/nw.js/issues/6059)
- Fix: crash with some compiled JS binary [(#6061)](https://github.com/nwjs/nw.js/issues/6061)

0.25.2 / 09-18-2017
===================
- Update Chromium to [61.0.3163.91](https://chromereleases.googleblog.com/2017/09/stable-channel-update-for-desktop_14.html)

0.25.1 / 09-15-2017
===================
- Update Node.js to [v8.5.0](https://nodejs.org/en/blog/release/v8.5.0/)

0.25.0 / 09-06-2017
===================
- Update Chromium to [61.0.3163.79](https://chromereleases.googleblog.com/2017/09/stable-channel-update-for-desktop.html)
- Fix: hyperlink "download" attribute doesn't seem to work [(#6094)](https://github.com/nwjs/nw.js/issues/6094)

0.24.4 / 08-25-2017
===================
- Update Chromium to [60.0.3112.113](https://chromereleases.googleblog.com/2017/08/stable-channel-update-for-desktop_24.html)
- Support rebranding the Helper app on macOS [(#6103)](https://github.com/nwjs/nw.js/issues/6103)
- Partly support loading Node inspector module in forked process [(#6115)](https://github.com/nwjs/nw.js/issues/6115)
- Fix: crash on macOS : chrome.windows.create() [(#6113)](https://github.com/nwjs/nw.js/issues/6113)
- Fix: crash on start in macOS 10.9 [(#6108)](https://github.com/nwjs/nw.js/issues/6108)
- [docs] support renaming the helper application under macOS.

0.24.3 / 08-16-2017
===================
- Update Node.js to [v8.4.0](https://nodejs.org/en/blog/release/v8.4.0/)
- Update Chromium to [60.0.3112.101](https://chromereleases.googleblog.com/2017/08/stable-channel-update-for-desktop_14.html)
- Fix: input nwsaveas in mac missing extensionName [(#6091)](https://github.com/nwjs/nw.js/issues/6091)
- Fix: labels of menu items with F1 ~ F12 shortcuts missing in menu under Windows [(#6090)](https://github.com/nwjs/nw.js/issues/6090)

0.25.0-beta1 / 08-11-2017
=========================
- Update Chromium to [61.0.3163.39](https://chromereleases.googleblog.com/2017/08/beta-channel-update-for-desktop_9.html)

0.24.2 / 08-11-2017
===================
- Update Node.js to [v8.3.0](https://nodejs.org/en/blog/release/v8.3.0/)

0.24.1 / 08-05-2017
===================
- Update Chromium to [60.0.3112.90](https://chromereleases.googleblog.com/2017/08/stable-channel-update-for-desktop.html)
- Fix: [OSX] Transparency regression [(#6055)](https://github.com/nwjs/nw.js/issues/6055)
- Fix: [DevTools] Uncaught Error during filtering network requests [(#6060)](https://github.com/nwjs/nw.js/issues/6060)

0.24.0 / 07-26-2017
===================
- Update Chromium to [60.0.3112.78](https://chromereleases.googleblog.com/2017/07/stable-channel-update-for-desktop.html)
- Update Node.js to [v8.2.1](https://nodejs.org/en/blog/release/v8.2.1/)
- Fix: DevTools extension panel not working in webview [(#6004)](https://github.com/nwjs/nw.js/issues/6004)

0.23.7 / 07-21-2017
===================
- Fix: alert message is not visible in windows 7 [(#6019)](https://github.com/nwjs/nw.js/issues/6019)
- Fix: Changing css not reflecting in kiosk mode. Transparency [(#6021)](https://github.com/nwjs/nw.js/issues/6021)
- Fix: Notifications not clickable [(#6040)](https://github.com/nwjs/nw.js/issues/6040)
- [doc] transparency switch change

0.23.6 / 07-12-2017
===================
- Update Node.js to [v8.1.4](https://nodejs.org/en/blog/release/v8.1.4/)
- Fix: Crash on opening Devtools with compiled JS binary [(#6001)](https://github.com/nwjs/nw.js/issues/6001)
- Fix: flickering titlebar on transparent window [(#5989)](https://github.com/nwjs/nw.js/issues/5989)

0.23.5 / 06-27-2017
===================
- Update Chromium to [59.0.3071.115](https://chromereleases.googleblog.com/2017/06/stable-channel-update-for-desktop_26.html)
- Fix: [WIN] hang in application start in edge cases
- Fix: [WIN] window transparent not work on v0.23.2 [(#5973)](https://github.com/nwjs/nw.js/issues/5973)

0.23.4 / 06-22-2017
===================
- Update Chromium to [59.0.3071.109](https://chromereleases.googleblog.com/2017/06/stable-channel-update-for-desktop_20.html)
- Fix: macOS Notification settings link opens up a blank screen in a new window on 0.23.0 [(#5948)](https://github.com/nwjs/nw.js/issues/5948)

0.23.3 / 06-19-2017
===================
- Update Chromium to [59.0.3071.104](https://chromereleases.googleblog.com/2017/06/stable-channel-update-for-desktop_15.html)
- Update Node.js to [v8.1.2](https://nodejs.org/en/blog/release/v8.1.2/)
- Fix: crash when start with user data dir from higher version [(#5964)](https://github.com/nwjs/nw.js/issues/5964)

0.23.2 / 06-13-2017
===================
- Update Node.js to [v8.1.0](https://nodejs.org/en/blog/release/v8.1.0/)
- Fix: upstream issue - crashes when calling video.play() while devtools is open [(#5958)](https://github.com/nwjs/nw.js/issues/5958)
- Fix: incorrect commit hash in welcome screen [(#5955)](https://github.com/nwjs/nw.js/issues/5955)

0.23.1 / 06-08-2017
===================
- Fix: Crash in blink::V8StringToWebCoreString [(#5947)](https://github.com/nwjs/nw.js/issues/5947)
- Fix: Menu not working [(#5953)](https://github.com/nwjs/nw.js/issues/5953)

0.23.0 / 06-06-2017
===================
- Update Chromium to [59.0.3071.86](https://blog.chromium.org/2017/05/chrome-59-beta-headless-chromium-native.html)
- Update Node.js to [v8.0.0](https://nodejs.org/en/blog/release/v8.0.0/)
- Add support for chrome://extensions WebUI in SDK flavor
- Fix: Cannot require 'https' module in Web Worker [(#5918)](https://github.com/nwjs/nw.js/issues/5918)

0.22.3 / 05-10-2017
===================
- Update Chromium to [58.0.3029.110](https://chromereleases.googleblog.com/2017/05/stable-channel-update-for-desktop_9.html)

0.22.2 / 05-09-2017
===================
- Update Node.js to [v7.10.0](https://nodejs.org/en/blog/release/v7.10.0/)
- Add support for chrome://profiler [(#5901)](https://github.com/nwjs/nw.js/issues/5901)

0.22.1 / 05-03-2017
===================
- Update Chromium to [58.0.3029.96](https://chromereleases.googleblog.com/2017/05/stable-channel-update-for-desktop.html)
- Include MP3 decoder in the official build [(#4149)](https://github.com/nwjs/nw.js/issues/4149)
- Fix: [OSX] nothing rendered using --force-cpu-redraw on OSX [(#5866)](https://github.com/nwjs/nw.js/issues/5866)
- Fix: [OSX] crash after resuming from breakpoint in Node [(#5882)](https://github.com/nwjs/nw.js/issues/5882)
- Fix: crash when calling setAlwaysOnTop in created window with remote url [(#5738)](https://github.com/nwjs/nw.js/issues/5738)
- [docs] package.nw -> app.nw for macOS [(#5883)](https://github.com/nwjs/nw.js/issues/5883)
- [docs] not recommending nw-builder since it's not in active maintained state

0.22.0 / 04-20-2017
===================
- Update Chromium to [58.0.3029.81](https://chromereleases.googleblog.com/2017/04/stable-channel-update-for-desktop.html)
- Fix: New window created using window.open not honouring height and width [(#5517)](https://github.com/nwjs/nw.js/issues/5517)

0.21.6 / 04-13-2017
===================
- Update Node.js to [v7.9.0](https://nodejs.org/en/blog/release/v7.9.0/)
- Fix: Unable to open a file as argument with an NWjs app [(#5242)](https://github.com/nwjs/nw.js/issues/5242)
- [docs] command line argument handling

0.21.5 / 04-06-2017
===================
- support crash dump uploading
- Fix: spell check for windows [(#4954)](https://github.com/nwjs/nw.js/issues/4954)
- Fix: crash on remote pages
- Fix: [WIN] crash dump generated in the wrong location [(#5248)](https://github.com/nwjs/nw.js/issues/5248)
- [docs] replace nwjs-builder with nwjs-builder-phoenix
- [docs] add crash_report_url in manifest

0.21.4 / 03-30-2017
===================
- Update Chromium to [57.0.2987.133](https://chromereleases.googleblog.com/2017/03/stable-channel-update-for-desktop_29.html)
- Update Node.js to [v7.8.0](https://nodejs.org/en/blog/release/v7.8.0/)
- Fix: linux symbol files are almost empty
- Fix: Blank window is not closed after download [(#5734)](https://github.com/nwjs/nw.js/issues/5734)
- Fix: When flash was first used on Windows then console windows with "echo NOT SANDBOXED" flickered [(#5278)](https://github.com/nwjs/nw.js/issues/5278)
- Fix: crash when showing WebUSB chooser window [(#5808)](https://github.com/nwjs/nw.js/issues/5808)
- Fix: allow html fs quota by default [(#5177)](https://github.com/nwjs/nw.js/issues/5177)
- Fix: reload app breaks the "global" object with --load-extension [(#5483)](https://github.com/nwjs/nw.js/issues/5483)
- Fix: opening external url from iframe works only once [(#5680)](https://github.com/nwjs/nw.js/issues/5680)
- Fix: iframe should not have independent NWWindow object [(#5043)](https://github.com/nwjs/nw.js/issues/5043)
- Fix: use .sym filename extension for linux symbol files [(#5608)](https://github.com/nwjs/nw.js/issues/5608)
- [docs] Fix quote in documentation of Flash
- [docs] add nw-autoupdater
- [docs] add link to js src protection performance fix

0.21.3 / 03-17-2017
===================
- Update Chromium to [57.0.2987.110](https://chromereleases.googleblog.com/2017/03/stable-channel-update-for-desktop_16.html)
- Enable DOM mutation events in Chrome App and NW App [(#4778)](https://github.com/nwjs/nw.js/issues/4778)
- Fix: crash of opening new window without menu [(#5737)](https://github.com/nwjs/nw.js/issues/5737)

0.21.2 / 03-15-2017
===================
- Update Node.js to [v7.7.3](https://nodejs.org/en/blog/release/v7.7.3/)
- Support webview cookie API [(#5750)](https://github.com/nwjs/nw.js/issues/5750)
- Fix: Crash on opening new window with an icon [(#5767)](https://github.com/nwjs/nw.js/issues/5767)
- Fix: chrome.runtime.onInstalled is firing on each start [(#5704)](https://github.com/nwjs/nw.js/issues/5704)
- Fix: disable security warning for passwd autofill on NW.js pages [(#5766)](https://github.com/nwjs/nw.js/issues/5766)
- Fix: Warning about RapperRecorder in console [(#5671)](https://github.com/nwjs/nw.js/issues/5671)
- Fix: Support nw.Window.removeAllListeners() with no parameter [(#5649)](https://github.com/nwjs/nw.js/issues/5649)
- [docs] webview cookie API

0.21.1 / 03-10-2017
===================
- Fix: NW is not using the package.json *name* parm as part of folder *path* any longer [(#5762)](https://github.com/nwjs/nw.js/issues/5762)

0.21.0 / 03-10-2017
===================
- Update Chromium to [57.0.2987.98](https://chromereleases.googleblog.com/2017/03/stable-channel-update-for-desktop.html)
- Update Node.js to [v7.7.2](https://nodejs.org/en/blog/release/v7.7.2/)
- Fix: Print cancel close app (#5742)
- Fix: Helper renderer process hangs on opening and closing print window several times (#5020)
- Fix: Microtasks in Blink are executed in different cycles with Chrome (#5730)
- Fix: Debug component build on OSX
- [docs] Building NW: directory name change of third_party/node
- [docs] Added "Autoupdates.md" for node-webkit-updater and nwjs-autoupdater

0.20.3 / 02-23-2017
===================
- Update Node.js to [v7.6.0](https://nodejs.org/en/blog/release/v7.6.0/)
- Support passing Buffer or ArrayBuffer to evalNWBin [(#5220)](https://github.com/nwjs/nw.js/issues/5220)
- Support pac_url and auto proxy detection[ (#916)](https://github.com/nwjs/nw.js/issues/916)
- New print options: autoprint, headerString, footerString [(#5702)](https://github.com/nwjs/nw.js/issues/5702)
- Fix: nwuseragent not working after 0.13 [(#5397)](https://github.com/nwjs/nw.js/issues/5397)
- Fix: flash in embed or object tag not working using file protocol [(#5706)](https://github.com/nwjs/nw.js/issues/5706)
- Fix: multiple menu shortcuts issues [(#5657)](https://github.com/nwjs/nw.js/issues/5657)
- Disable resolving www.google.com in DNS predictor on startup
- [docs] upstream Windows build instructions has moved
- [docs] update for new features

0.20.2 / 02-13-2017
===================
- Fix: Crash on right click after opening devtools by F12 [(#5659)](https://github.com/nwjs/nw.js/issues/5659)
- Fix: nwsaveas attribute regression [(#5667)](https://github.com/nwjs/nw.js/issues/5667)
- Fix: local pdf in webview not loading, gives ERR_BLOCKED_BY_CLIENT [(#5682)](https://github.com/nwjs/nw.js/issues/5682)
- Fix: disable-crash-handler not working [(#5624)](https://github.com/nwjs/nw.js/issues/5624)
- Fix: Signal app does not get rendered [(#5528)](https://github.com/nwjs/nw.js/issues/5528)
- [doc] correct path of crash dumps in Windows [(#5669)](https://github.com/nwjs/nw.js/issues/5669)

0.20.1 / 02-02-2017
===================
- Update Chromium to [56.0.2924.87](https://chromereleases.googleblog.com/2017/02/stable-channel-update-for-desktop.html)
- Update Node.js to [v7.5.0](https://nodejs.org/en/blog/release/v7.5.0/)
- Support disabling crash handler for single process mode with `--disable-crash-handler=true' [(#5624)](https://github.com/nwjs/nw.js/issues/5624)
- [docs] Added documentation for `disable-crash-handler' switch

0.20.0 / 01-26-2017
===================
- Update Chromium to [56.0.2924.76](https://chromereleases.googleblog.com/2017/01/stable-channel-update-for-desktop.html)
- support disabling cookie encryption with '--disable-cookie-encryption'
- Added copies option to print API [(#5482)](https://github.com/nwjs/nw.js/issues/5482)
- Fix: session lost after open the develop tool and close it [(#5355)](https://github.com/nwjs/nw.js/issues/5355)
- Fix: DOM setTimeout callbacks not running from Node vm context in webview [(#5622)](https://github.com/nwjs/nw.js/issues/5622)
- [docs] Added documentation for the copies option in the printing API
- [docs] Added docs for "Support for Mac App Store"
- [docs] Added docs for '--disable-cookie-encryption'

0.19.5 / 01-10-2017
===================
- Update Node.js to [7.4.0](https://nodejs.org/en/blog/release/v7.4.0/)
- Node.js integration is supported in webview [(#4780)](https://github.com/nwjs/nw.js/issues/4780)
- Fix: Crash when accessing HTMLElement properties from node's vm module [(#4018)](https://github.com/nwjs/nw.js/issues/4018)
- Fix: Linux wrong file permissions [(#5611)](https://github.com/nwjs/nw.js/issues/5611)
- [docs] Node.js support in webview

0.19.4 / 12-30-2016
===================
- Fix: crash in ServiceWorker in webview

0.19.3 / 12-21-2016
===================
- Update Node.js to [7.3.0](https://nodejs.org/en/blog/release/v7.3.0/)
- Fix: Hang when https website with cert. error is loaded [(#5544)](https://github.com/nwjs/nw.js/issues/5544)
- Fix: Webview freezes on almost any invalid certificate [(#5564)](https://github.com/nwjs/nw.js/issues/5564)
- Fix: Flash needs manual intervention and control-click not working [(#5548)](https://github.com/nwjs/nw.js/issues/5548)
- Fix: permission warning and crash of chrome identity app [(#5552)](https://github.com/nwjs/nw.js/issues/5552)
- Fix: popup menu in background page crashes nw [(#5513)](https://github.com/nwjs/nw.js/issues/5513)
- Fix: nw.Window.isTransparent should be boolean [(#5561)](https://github.com/nwjs/nw.js/issues/5561)
- Fix: [test] race condition in chromedriver
- [docs] Add a recent tutorial

0.19.2 / 12-12-2016
===================
- Update Chromium to [55.0.2883.87](https://googlechromereleases.blogspot.com/2016/12/stable-channel-update-for-desktop_9.html)
- Fix crash when worker with Node support is closed

0.19.1 / 12-09-2016
===================
- Update Node.js to [7.2.1](https://nodejs.org/en/blog/release/v7.2.1/)
- Fix: Screen flickering in fullscreen [(#5477)](https://github.com/nwjs/nw.js/issues/5477)
- Fix: ship nacl_irt_x86_64.nexe and nacl64.exe for win-ia32 [(#5524)](https://github.com/nwjs/nw.js/issues/5524)
- Fix: nw-gyp build failure with UNICODE set [(nw-gyp#100)](https://github.com/nwjs/nw-gyp/issues/100)
- [docs] building Node in nw19

0.19.0 / 12-02-2016
===================
- Update Chromium to [55.0.2883.75](https://googlechromereleases.blogspot.com/2016/12/stable-channel-update-for-desktop.html)
- Fix: 'show_in_taskbar' option for 'nw.Window.open' (#4970)
- Fix: process.uptime() (#5481)

0.18.8 / 11-23-2016
===================
- Update Node.js to [7.2.0](https://nodejs.org/en/blog/release/v7.2.0/)
- Fix: closing devtools window clears session cookies
- Fix: disable symbolication of "adding an unknown subview" for frameless window [(#5435)](https://github.com/nwjs/nw.js/issues/5435)
- Fix: bugs of modifiers of nw.MenuItem [(#5451)](https://github.com/nwjs/nw.js/issues/5451)

0.19.0-rc1 / 11-17-2016
=======================
- Update Chromium to [55.0.2883.52](https://googlechromereleases.blogspot.com/2016/11/beta-channel-update-for-desktop_16.html)

0.18.7 / 11-16-2016
===================
- Fix: [OSX] Regression in Debugging [(#5469)](https://github.com/nwjs/nw.js/issues/5469)
- Fix: [OSX] Regression in --disable-raf-throttling feature [(5466)](https://github.com/nwjs/nw.js/issues/5466)
- [docs] adding Getting Started example to use JS as entrance [(#5471)](https://github.com/nwjs/nw.js/issues/5471)

0.18.6 / 11-10-2016
===================
- Update Chromium to [54.0.2840.99](https://googlechromereleases.blogspot.com/2016/11/stable-channel-update-for-desktop_9.html)
- Update Node.js to [7.1.0](https://nodejs.org/en/blog/release/v7.1.0/)
- Fix: registerGlobalHotKey and alert [(#4992)](https://github.com/nwjs/nw.js/issues/4992)
- Fix: `nwworkingdir` attribute for multiple file input [(#5327)](https://github.com/nwjs/nw.js/issues/5327)
- Fix: Take heap snapshot crash Mac OS X - nwjs 0.18.0 [(#5375)](https://github.com/nwjs/nw.js/issues/5375)
- Fix: WebUSB API not working [(#5418)](https://github.com/nwjs/nw.js/issues/5418)
- Fix: --single-process not working in manifest [(#5389)](https://github.com/nwjs/nw.js/issues/5389)
- Fix: window minimize event bug [(#5388)](https://github.com/nwjs/nw.js/issues/5388)
- Fix: Linux Fedora application menu says 'nwjs' instead of package.json name [(#4693)](https://github.com/nwjs/nw.js/issues/4693)
- Fix: PNaCl not working in OSX
- Fix: Devtools extension load throws error [(#5428)](https://github.com/nwjs/nw.js/issues/5428)
- Fix: Icon is broken when launched with relative path to parent folder [(#5402)](https://github.com/nwjs/nw.js/issues/5402)
- Fix: Node worker crash when being closed in some cases
- [doc] rebuilding native modules with npm

0.18.5 / 11-03-2016
===================
- Update Chromium to [54.0.2840.90](https://googlechromereleases.blogspot.com/2016/11/stable-channel-update-for-desktop.html)
- Fix: node worker Uncaught Error: No such module: url [(#5422)](https://github.com/nwjs/nw.js/issues/5422)
- Fix: Crashed when exiting nw without --enable-node-worker [(#5420)](https://github.com/nwjs/nw.js/issues/5420)
- Fix: Gamepad API not working [(#5377)](https://github.com/nwjs/nw.js/issues/5377)
- Fix: --single-process Seg Fault [(#5382)](https://github.com/nwjs/nw.js/issues/5382)

0.19.0-beta1 / 10-31-2016
=========================
- Update Chromium to [55.0.2883.28](https://googlechromereleases.blogspot.com/2016/10/beta-channel-update-for-desktop_26.html)

0.18.4 / 10-31-2016
===================
- Support Node.js in Web Workers [(#494)](https://github.com/nwjs/nw.js/issues/494)
- [docs] --enable-node-worker in Command Line Options

0.18.3 / 10-26-2016
===================
- Update Node.js to [7.0.0](https://nodejs.org/en/blog/release/v7.0.0/)

0.18.2 / 10-21-2016
===================
- Update Chromium to [54.0.2840.71](https://googlechromereleases.blogspot.com/2016/10/stable-channel-update-for-desktop_20.html)
- Fix: [WIN] Error in chrome_elf_main.cc after package with Enigma VB [(#5392)](https://github.com/nwjs/nw.js/issues/5392)

0.18.1 / 10-18-2016
===================
- Update Node.js to [6.8.1](https://nodejs.org/en/blog/release/v6.8.1/)
- Fix: [WIN] Cannot rename nw.exe with 0.18.0 [(#5362)](https://github.com/nwjs/nw.js/issues/5362)
- Support --single-process mode for Steam platform [(#4982)](https://github.com/nwjs/nw.js/issues/4982)
- Fix: Crash on close Mac OS X when using audio streams [(#5318)](https://github.com/nwjs/nw.js/issues/5318)
- Fix: nw.App.clearCache() freez apllication nwjs 0.18.0 [(#5368)](https://github.com/nwjs/nw.js/issues/5368)

0.18.0 / 10-13-2016
===================
- Update Chromium to [54.0.2840.59](https://googlechromereleases.blogspot.com/2016/10/stable-channel-update-for-desktop.html)
- Update Node.js to [6.8.0](https://nodejs.org/en/blog/release/v6.8.0/)
- Fix: [WIN] wrong icon used in windows opened by window.open()
- Fix: support sign-in UI for Chrome Apps (#5351)

0.17.6 / 09-30-2016
===================
- Update Chromium to [53.0.2785.143](https://googlechromereleases.blogspot.com/2016/09/stable-channel-update-for-desktop_29.html)
- Fix: [WIN] 2nd instance shows a popup warning if the first instance starts with higher privilege

0.17.5 / 09-28-2016
===================
- Update Node.js to [6.7.0](https://nodejs.org/en/blog/release/v6.7.0/)
- Fix: Segfault when using additional_trust_anchors [(#5279)](https://github.com/nwjs/nw.js/issues/5279)
- [doc] update known issues (removed fixed items)

0.18.0-rc1 / 09-22-2016
=========================
- Update Chromium to [54.0.2840.34](https://googlechromereleases.blogspot.com/2016/09/beta-channel-update-for-desktop_21.html)

0.17.4 / 09-20-2016
===================
- Update Node.js to [6.6.0](https://nodejs.org/en/blog/release/v6.6.0/)
- Fix: Remove dialog when running on read only volumes (.dmg) [(#5309)](https://github.com/nwjs/nw.js/issues/5309)
- Fix: window events on remote pages not firing after reloading [(#5312)](https://github.com/nwjs/nw.js/issues/5312)
- Fix: remove limitation of dom storage quota
- Fix: chrome.app.window reference lost after reload [(#5294)](https://github.com/nwjs/nw.js/issues/5294)
- [doc] fix a typo in FAQ for AngularJS

0.17.3 / 09-15-2016
===================
- Update Chromium to [53.0.2785.116](https://googlechromereleases.blogspot.com/2016/09/stable-channel-update-for-desktop_14.html)

0.17.2 / 09-14-2016
===================
- Update Chromium to [53.0.2785.113](https://googlechromereleases.blogspot.com/2016/09/stable-channel-update-for-desktop_13.html)
- Fix: chrome.app.window.getall() broken with remote pages windows [(#5181)](https://github.com/nwjs/nw.js/issues/5181)
- Fix: background page was destroyed when "main" points to remote pages [(#5282)](https://github.com/nwjs/nw.js/issues/5282)

0.18.0-beta1 / 09-13-2016
=========================
- Update Chromium to [54.0.2840.16](https://googlechromereleases.blogspot.com/2016/09/the-chrome-team-is-excited-to-announce.html)
- Fix: chrome.app.window.getall() broken with remote pages windows [(#5181)](https://github.com/nwjs/nw.js/issues/5181)

0.17.1 / 09-08-2016
===================
- Update Chromium to [53.0.2785.101](https://googlechromereleases.blogspot.com/2016/09/stable-channel-update-for-desktop.html)
- Fix: Failed to load node library (liblibnode.dylib) on child_process.fork [(#5269)](https://github.com/nwjs/nw.js/issues/5269)
- Fix: ia32 package file name [(#5258)](https://github.com/nwjs/nw.js/issues/5258)
- Fix: typos in common.gypi to prevent nw-gyp failure
- [docs] new ffmpeg dll build docs for GN [(#5263)](https://github.com/nwjs/nw.js/issues/5263)

0.17.0 / 08-31-2016
===================
- Update Chromium to [53.0.2785.89](http://googlechromereleases.blogspot.com/2016/08/stable-channel-update-for-desktop_31.html)
- Update Node.js to [6.5.0](https://nodejs.org/en/blog/release/v6.5.0/)
- Fix: new_instance does not create new renderer process [(#5163)](https://github.com/nwjs/nw.js/issues/5163)
- Fix: [OSX] crash on setting menubar in 0.17 beta [(#5229)](https://github.com/nwjs/nw.js/issues/5229)
- Fix: Tray Icon Menu not working [(#5202)](https://github.com/nwjs/nw.js/issues/5202)
- Fix: Crashing on link open in webview [(#5226)](https://github.com/nwjs/nw.js/issues/5226)
- [docs] new build documentation for GN

0.17.0-beta2 / 08-20-2016
=========================
- Update Chromium to [53.0.2785.70](http://blog.chromium.org/2016/08/chrome-53-beta-shadow-dom.html)
- Update Node.js to [6.4.0](https://nodejs.org/en/blog/release/v6.4.0/)
- Fix: nwjs 0.17 beta - crash at boot [(#5193)](https://github.com/nwjs/nw.js/issues/5193)
- Fix: multiple evalNWBin always execute just the first *.bin [(#5168)](https://github.com/nwjs/nw.js/issues/5168)
- Fix: regression - Shortcut not work in devtools while it placed in a webview [(#4709)](https://github.com/nwjs/nw.js/issues/4709)
- Fix: [GN] mac symbol generation

0.17.0-beta1 / 08-05-2016
=========================
- Update Chromium to [53.0.2785.46](http://blog.chromium.org/2016/08/chrome-53-beta-shadow-dom.html)

0.16.1 / 08-04-2016
=======================
- Update Chromium to [52.0.2743.116](http://googlechromereleases.blogspot.com/2016/08/stable-channel-update-for-desktop.html)
- Update Node to [6.3.1](https://nodejs.org/en/blog/release/v6.3.1/)
- Fix: [osx] update menuitems after setting main menu [(#2812)](https://github.com/nwjs/nw.js/issues/2812)
- Fix: Crashes if iframe has JavaScript error [(#5148)](https://github.com/nwjs/nw.js/issues/5148)
- [docs] update license information about enabling Proprietary Codecs

0.16.0 / 07-21-2016
=======================
- Update Chromium to [52.0.2743.82](http://googlechromereleases.blogspot.com/2016/07/stable-channel-update.html)
- [docs] updated Quick Start section for "Package and Distribute" [(#5054)](https://github.com/nwjs/nw.js/issues/5054)

0.16.0-rc2 / 07-15-2016
=======================
- Update Chromium to 52.0.2743.75
- Update Node.js to [6.3.0](https://nodejs.org/en/blog/release/v6.3.0/)
- Fix: nw is creating a file with the first command line argument it's opened with [(#5097)](https://github.com/nwjs/nw.js/issues/5097)
- [doc] fixed typos for docs [(#5091)](https://github.com/nwjs/nw.js/issues/5091)

0.16.0-rc1 / 07-13-2016
=======================
- Update Chromium to 52.0.2743.73
- Update Node to [6.3.0](https://nodejs.org/en/blog/release/v6.3.0/)
- Add App.clearAppCache(manifest_url) for clearing Application Cache [(#4991)](https://github.com/nwjs/nw.js/issues/4991)
- Enable password autosaving for http auth dialog [(#4966)](https://github.com/nwjs/nw.js/issues/4966)
- Margin option support for printer api [(#5033)](https://github.com/nwjs/nw.js/issues/5033)
- Fix: finding bundled pepper flash plugin [(#5068)](https://github.com/nwjs/nw.js/issues/5068)
- Fix: Silent printing with win.print() for pdfs [(#5002)](https://github.com/nwjs/nw.js/issues/5002)
- Fix: Permission Denied error when using chrome.bluetoothLowEnergy API [(#5010)](https://github.com/nwjs/nw.js/issues/5010)
- Fix: [OSX] Choose directory dialog shows "Upload" instead of "Select" [(#5058)](https://github.com/nwjs/nw.js/issues/5058)
- Fix: menu bar background [(#4851)](https://github.com/nwjs/nw.js/issues/4851)
- Fix: send positions arguments to click event handler of Tray [(#1874)](https://github.com/nwjs/nw.js/issues/1874)
- Fix: DesktopCaptureMonitor desktop stream does not get AudioTrack [(#5029)](https://github.com/nwjs/nw.js/issues/5029)
- Fix: crash in win.showDevTools() with string id [(#5061)](https://github.com/nwjs/nw.js/issues/5061)
- Fix: webview.clearData crash [(#5087)](https://github.com/nwjs/nw.js/issues/5087)
- Fix: Buffer encoding handling with input from DOM elements [(#5069)](https://github.com/nwjs/nw.js/issues/5069)
- Fix: Window resize event returns undefined for height [(#4993)](https://github.com/nwjs/nw.js/issues/4993)
- Fix: can not remove shortcut key at menuItem [(#5026)](https://github.com/nwjs/nw.js/issues/5026)
- Fix: win.showDevTools crash with empty string id [(#5061)](https://github.com/nwjs/nw.js/issues/5061)
-  Updated win_delay_load_hook.c to work with node-gyp@3.4.0 [(#5088)](https://github.com/nwjs/nw.js/issues/5088)
- [docs] margins for Window.print()
- [docs] Native Module building in nw15 and later

0.15.4 / 06-24-2016
===================
- Update Chromium to [51.0.2704.106](http://googlechromereleases.blogspot.com/2016/06/stable-channel-update_23.html)
- Include minidump_stackwalk in Linux and OSX sdk build [(#5023)](https://github.com/nwjs/nw.js/issues/5023)
- Added new APIs for read/write multiple types of data to clipboard [(#4929)](https://github.com/nwjs/nw.js/issues/4929)
- Fix: 'loaded' event shouldn't be fired when using history API [(#5001)](https://github.com/nwjs/nw.js/issues/5001)
- Fix: DeprecationWarning: 'root' is deprecated, use 'global' [(#4938)](https://github.com/nwjs/nw.js/issues/4938)
- Fix: Migrate websql data from 0.12 [(#5018)](https://github.com/nwjs/nw.js/issues/5018)
- Fix: content verficiation of package.json in Node modules
- Fix: [Linux] foreground colors for menubar [(#5007)](https://github.com/nwjs/nw.js/issues/5007)
- Make Window.eval() and Window.evalNWBin() return result

0.15.3 / 06-18-2016
===================
- Update Chromium to [51.0.2704.103](http://googlechromereleases.blogspot.com/2016/06/stable-channel-update_16.html)
- Update Node.js to [v6.2.2](https://github.com/nodejs/node/blob/v6.2.2/CHANGELOG.md)
- Fix: [WIN] user data migration from 0.12
- Fix: FileList object in dragover event callback is not set correctly
- Fix: crash on JSON.stringify(window) [(#4959)](https://github.com/nwjs/nw.js/issues/4959)
- Fix: open mini browser for links in PDF [(#4915)](https://github.com/nwjs/nw.js/issues/4915)
- Fix: possible race condition in content verfication
- Fix: protect package.json from hacking for content verification
- Fix: Disabled contextual menu plug-in items on mac
- [doc] changed link for FFmpeg precompiled binaries
- [doc] dded JS file as possibility for the "main" field

0.15.2 / 06-07-2016
===================
- Update Chromium to [51.0.2704.84](http://googlechromereleases.blogspot.com/2016/06/stable-channel-update_6.html)
- Update Node.js to [v6.2.1](https://github.com/nodejs/node/blob/v6.2.1/CHANGELOG.md)
- Fix: "Print to PDF" in the Privew dialog broken after using nw.Window.print() [(#4911)](https://github.com/nwjs/nw.js/issues/4911)

0.15.1 / 06-02-2016
===================
- Update Chromium to [51.0.2704.79](http://googlechromereleases.blogspot.com/2016/06/stable-channel-update.html)
- Custom Print API [(#4911)](https://github.com/nwjs/nw.js/issues/4911)
- Add: Support custom domain of nwjs app
- Add switch to disable the remote debugging server [(#3620)](https://github.com/nwjs/nw.js/issues/3620)
- Fix: make `this` in event listener reference to `nw.Window` [(#4901)](https://github.com/nwjs/nw.js/issues/4901)
- Fix: Don't migrate the data if new version is there to prevent data loss [(#4853)](https://github.com/nwjs/nw.js/issues/4853)
- Fix: Menu item key not working when key is 0 [(#4837)](https://github.com/nwjs/nw.js/issues/4837)
- Fix: cookies support of chrome-extension protocol
- [docs] usage of tray when page can be navigated [(#4816)](https://github.com/nwjs/nw.js/issues/4816)
- [docs] Custom Print API
- [docs] Manifest: Using custom domain

0.14.6 / 06-01-2016
===================
- Custom Print API [(#4911)](https://github.com/nwjs/nw.js/issues/4911)
- Add: '--disable-devtools' command line for SDK build [(#4839)](https://github.com/nwjs/nw.js/issues/4839)
- Add: Support custom domain of nwjs app
- Add switch to disable the remote debugging server [(#3620)](https://github.com/nwjs/nw.js/issues/3620)
- Fix: make `this` in event listener reference to `nw.Window` [(#4901)](https://github.com/nwjs/nw.js/issues/4901)
- Fix: Don't migrate the data if new version is there to prevent data loss [(#4853)](https://github.com/nwjs/nw.js/issues/4853)
- Fix: Menu item key not working when key is 0 [(#4837)](https://github.com/nwjs/nw.js/issues/4837)
- Fix: inject_js in webview and remote pages [(#4877)](https://github.com/nwjs/nw.js/issues/4877)
- Fix: Transparency + Click-through clicks through floated elements [(#4846)](https://github.com/nwjs/nw.js/issues/4846)
- Fix: disable tray icon "NW.js is using your microphone" [(#4852)](https://github.com/nwjs/nw.js/issues/4852)
- Fix: Memory leak in Menu API [(#4860)](https://github.com/nwjs/nw.js/issues/4860)
- Fix: Error in Tray in background context [(#4862)](https://github.com/nwjs/nw.js/issues/4862)
- Fix: cookies support of chrome-extension protocol
- [docs] disable-devtools switch
- [docs] using Menu API and navigation
- [docs] usage of tray when page can be navigated [(#4816)](https://github.com/nwjs/nw.js/issues/4816)
- [docs] Custom Print API
- [docs] Manifest: Using custom domain

0.15.0 / 05-25-2016
===================
- Update Chromium to [51.0.2704.63](http://googlechromereleases.blogspot.com/2016/05/stable-channel-update_25.html)
- Update Node.js to 6.2.0
- Add: '--disable-devtools' command line for SDK build [(#4839)](https://github.com/nwjs/nw.js/issues/4839)
- Fix: inject_js in webview and remote pages [(#4877)](https://github.com/nwjs/nw.js/issues/4877)
- Fix: Transparency + Click-through clicks through floated elements [(#4846)](https://github.com/nwjs/nw.js/issues/4846)
- Fix: disable tray icon "NW.js is using your microphone" [(#4852)](https://github.com/nwjs/nw.js/issues/4852)
- Fix: Memory leak in Menu API [(#4860)](https://github.com/nwjs/nw.js/issues/4860)
- Fix: Error in Tray in background context [(#4862)](https://github.com/nwjs/nw.js/issues/4862)
- [docs] disable-devtools switch
- [docs] using Menu API and navigation

0.14.5 / 05-12-2016
===================
- Update Chromium to [50.0.2661.102](http://googlechromereleases.blogspot.com/2016/05/stable-channel-update.html)
- Fix: process.nextTick() not working after uncaught exception [(#4822)](https://github.com/nwjs/nw.js/issues/4822)
- Fix: [OSX] Adobe flash support
- Fix: [OSX] nw.Screen.DesktopCaptureMonitor
- [docs] remove deprecated 'unmaximize' event
- [docs] verifying flash plugin

0.14.4 / 05-06-2016
===================
- Update Node.js to 5.11.1
- Extended `nw.Clipboard` to support multiple data types
- Fix: nw.Window.open returns undefined in callback when devtools is opened [(#4188)](https://github.com/nwjs/nw.js/issues/4188)
- Fix: ALT+ENTER causes NW.js to crash when using --in-process-gpu [(#4741)](https://github.com/nwjs/nw.js/issues/4741)
- Fix: 'document-end' and 'loaded' event not working with remote pages [(#4788)](https://github.com/nwjs/nw.js/issues/4788)
- Fix: inject_js_start not working with iframes in 0.14.3 [(#4791)](https://github.com/nwjs/nw.js/issues/4791)
- Fix: nw.Menu.popup works only once [(#4721)](https://github.com/nwjs/nw.js/issues/4721)
- [docs] document-start event and nw.Window.open()

0.15.0-beta2 / 05-03-2016
=========================
- Update Chromium to 51.0.2704.29
- Extended `nw.Clipboard` to support multiple data types
- Fix: nw.Window.open returns undefined in callback when devtools is opened [(#4188)](https://github.com/nwjs/nw.js/issues/4188)
- ALT+ENTER causes NW.js to crash when using --in-process-gpu [(#4741)](https://github.com/nwjs/nw.js/issues/4741)

0.14.3 / 04-29-2016
===================
- Update Chromium to 50.0.2661.94
- Update Node.js to 5.11.0
- Fix: failed to start after reload with package name starting with underline [(#4779)](https://github.com/nwjs/nw.js/issues/4779)
- Fix: [OSX] crashes when calling native module in inject_js_start script [(#4772)](https://github.com/nwjs/nw.js/issues/4772)
- Fix: Content of file included via "inject_js_start" is executing twice [(#4705)](https://github.com/nwjs/nw.js/issues/4705)
- Fix: undefined windows object returned from nw.Window.open in some cases
- Fix: window menubar click event handler [(#4777)](https://github.com/nwjs/nw.js/issues/4777)
- [docs] oncancel event of File dialog

0.14.2 / 04-21-2016
===================
- Update Chromium to 50.0.2661.86
- Update Node.js to 5.10.1
- Add: support 'oncancel' event of File dialog [(#4501)](https://github.com/nwjs/nw.js/issues/4501)
- Add: implemented `win.menu=null` to remove menubar [(#4725)](https://github.com/nwjs/nw.js/issues/4725)
- Fix: Shell.openItem() does not open directories [(#4713)](https://github.com/nwjs/nw.js/issues/4713)
- Fix: [WIN] Invisible eventless area around all sides of window [(#4749)](https://github.com/nwjs/nw.js/issues/4749)
- Fix: [WIN] Windows loading icon stays visible after launching app [(#4685)](https://github.com/nwjs/nw.js/issues/4685)
- Fix: [OSX] Support 10.8-10.7
- Fix: Font size issue in v0.14.0 [(#4717)](https://github.com/nwjs/nw.js/issues/4717)
- Fix: Weird popup in custom URL protocol [(#4746)](https://github.com/nwjs/nw.js/issues/4746)

0.14.1 / 04-18-2016
===================
- Fix: Menu not working in Windows and Linux [(#4727)](https://github.com/nwjs/nw.js/issues/4727)
- Fix: Crash with exceptions in some remote pages [(#4729)](https://github.com/nwjs/nw.js/issues/4729)
- Fix: [WIN] ship libexif.dll [(#4637)](https://github.com/nwjs/nw.js/issues/4637)
- Fix: Packaged application with command line parameter won't start; Reordered package path finding for backward compatible with nw12 [(#4681)](https://github.com/nwjs/nw.js/issues/4681)
- Fix: use icon in manifest for devtools window icon [(#4707)](https://github.com/nwjs/nw.js/issues/4707)
- [docs]  building proprietary codecs easier [(#4595)](https://github.com/nwjs/nw.js/issues/4595)
- [docs] Fix: window object in the Node context

0.14.0 / 04-14-2016
===================
- Update Chromium to 50.0.2661.75
- Fix: AlwaysOnTop does not work with Fullscreen or overlapping toolbar [(#4554)](https://github.com/nwjs/nw.js/issues/4554)
- Fix: Window object from `nw.Window.open` is `null` after an `new_instance` window was opened [(#4691)](https://github.com/nwjs/nw.js/issues/4691)
- Fix: Linux Fedora application menu says 'nwjs' instead of package.json name [(#4693)](https://github.com/nwjs/nw.js/issues/4693)
- Fix: Shortcut not work in devtools while it placed in a webview [(#4709)](https://github.com/nwjs/nw.js/issues/4709)
- Fix: nw.App.clearCache hang and it should be sync [(#4674)](https://github.com/nwjs/nw.js/issues/4674)
- Fix: Crashes sometimes when using menu.popup and GC happens [(#4697)](https://github.com/nwjs/nw.js/issues/4697)
- Fix: Win.isFullscreen and isAlwaysOnTop should not be function [(#4446)](https://github.com/nwjs/nw.js/issues/4446)

0.13.4 / 04-08-2016
===================
- Update Chromium to 49.0.2623.112
- Fix: webview local file issue [(#4668)](https://github.com/nwjs/nw.js/issues/4668)
- Fix: Custom app icon with incorrect size under volume mixer [(#4649)](https://github.com/nwjs/nw.js/issues/4649)
- Fix: process Event: 'uncaughtException' not working in 0.13 [(#4679)](https://github.com/nwjs/nw.js/issues/4679)
- [docs] data-path -> user-data-dir; --disable-raf-throttling
- [docs] Clarify files can be safely removed during packaging [(#4676)](https://github.com/nwjs/nw.js/issues/4676)

0.14.0-rc3 / 04-07-2016
=======================
- Update Chromium to 50.0.2661.66
- Update Node.js to 5.10.0
- Fix XP support again (32bit)
- Port all the fixes released in 0.13.3

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

0.14.0-rc2 / 03-31-2016
=======================
- Update Chromium to 50.0.2661.57
- Update Node.js to 5.9.1
- Fix XP support
- [WIN] fix restore size when aero glass is on [(#4323)](https://github.com/nwjs/nw.js/issues/4323)

0.14.0-rc1 / 03-29-2016
=======================
- Update Chromium to 50.0.2661.49

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
