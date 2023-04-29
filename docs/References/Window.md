# Window {: .doctitle}
---

[TOC]

`Window` is a wrapper of the DOM's topmost `window` object. It has extended operations and can receive various window events.

Every `Window` is an instance of the EventEmitter class, and you're able to use `Window.on(...)` to respond to native window's events.

!!! warning "Behavior Changed"
    There are some changes of `Window` since 0.13.0. Please see [Migration Notes from 0.12 to 0.13](../For Users/Migration/From 0.12 to 0.13.md).

## Synopsis

```javascript
// Get the current window
var win = nw.Window.get();

// Listen to the minimize event
win.on('minimize', function() {
  console.log('Window is minimized');
});

// Minimize the window
win.minimize();

// Unlisten the minimize event
win.removeAllListeners('minimize');

// Create a new window and get it
nw.Window.open('https://github.com', {}, function(new_win) {
  // And listen to new window's focus event
  new_win.on('focus', function() {
    console.log('New window is focused');
  });

});
```

## Window.get([window_object])

* `window_object` `{DOM Window}` _Optional_ is the DOM window
* Returns `{Window}` the native `Window` object

If `window_object` is not specifed, then return current window's `Window` object, otherwise return `window_object`'s `Window` object.

!!! note
    If `window_object` is `iframe`'s, the function will still return topmost window's `Window` object.

```javascript
// Get the current window
var win = nw.Window.get();

// Get iframe's window
var iframeWin = nw.Window.get(iframe.contentWindow);

//This will return true
console.log(iframeWin === win);

// Create a new window and get it
nw.Window.open('https://github.com/nwjs/nw.js', {}, function(new_win) {
  // do something with the newly created window
});
```
## Window.getAll(callback)

Get all windows with a callback function whose parameter is an array of nw.Window object. This function is supported since 0.42.6.

## Window.open(url, [options], [callback])

!!! warning "Behavior Changed"
    The behavior of the function is changed since 0.13.0. Please see [Migration Notes from 0.12 to 0.13](../For Users/Migration/From 0.12 to 0.13.md).

* `url` `{String}` URL to be loaded in the opened window
* `options` `{Object}` _Optional_ see [Window subfields](Manifest Format.md#window-subfields) in manifest format. And following extra fields can also be used in options.
    - `new_instance` `{Boolean}` _Optional_ whether to open a new window in a separate render process.
    - `mixed_context` `{Boolean}` _Optional_ If true, the Node context and DOM context are merged in the new window's process. Use only when `new_instance` is true.
    - `inject_js_start` `{String}` _Optional_ the script to be injected before any DOM is constructed and any script is run. See [Manifest format](Manifest Format.md#inject_js_start)
    - `inject_js_end` `{String}` _Optional_ the script to be injected after the document object is loaded, before onload event is fired. See [Manifest format](Manifest Format.md#inject_js_end)
    - `id` `{String}` _Optional_ the `id` used to identify the window. This will be used to remember the size and position of the window and restore that geometry when a window with the same id is later opened. [See also the Chrome App documentation](https://developer.chrome.com/apps/app_window#type-CreateWindowOptions)
* `callback(win)` `{Function}` _Optional_ callback when with the opened native `Window` object

Open a new window and load `url` in it.

!!! note
    You should wait for the Window's `loaded` event before interacting with any of its components.

!!! note "Focus"
    The opened window is not focused by default. If you want it to be focused by default, you can set `focus` to `true` in `options`.

## win.window

Get the corresponding DOM window object of the native window.

## win.x
## win.y

Get or set left/top offset from window frame to screen.

## win.width
## win.height

Get or set window's size, including the window's frame.

## win.title

Get or set window's title.

## win.menu

Get or set window's menubar. Set with a Menu with type `menubar`. When `win.menu` is set to `null`, the menubar is completely removed for Windows and Linux, and the menubar is cleared out on Mac.

See [Customize Menubar](../For Users/Advanced/Customize Menubar.md) for the usage of menubars. And see [Menu](Menu.md) and [MenuItem](MenuItem.md) for detailed APIs.

## win.isAlwaysOnTop

Get whether the window is always on top of other windows.

## win.isFullscreen

Get whether we're in fullscreen mode.

## win.isTransparent 

Get whether transparency is turned on

## win.isKioskMode

Get whether we're in kiosk mode.

## win.zoomLevel

Get or set the page zoom. `0` for normal size; positive value for zooming in; negative value for zooming out.

## win.cookies.*

This includes multiple functions to manipulate the cookies. The API is defined in the same way as [Chrome Extensions](http://developer.chrome.com/extensions/cookies.html). NW.js supports the `get`, `getAll`, `remove` and `set` methods; `onChanged` event (supporting both `addListener` and `removeListener` function on this event).

And anything related to `CookieStore` in the Chrome extension API is not supported, because there is only one global cookie store in NW.js apps.

## win.moveTo(x, y)

* `x` `{Integer}` offset to the left of the screen
* `y` `{Integer}` offset to the top of the screen

Moves a window's left and top edge to the specified coordinates.

## win.moveBy(x, y)

* `x` `{Integer}` horizontal offset
* `y` `{Integer}` vertical offset

Moves a window a specified number of pixels relative to its current coordinates.

## win.resizeTo(width, height)

* `width` `{Integer}` the inner width of the window
* `height` `{Integer}` the inner height of the window

Resizes a window to the specified `width` and `height`.

## win.setInnerWidth(width)

* `width` `{Integer}` the inner width of the window

## win.setInnerHeight(height)

* `height` `{Integer}` the inner height of the window

## win.resizeBy(width, height)

* `width` `{Integer}` the offset width of the window
* `height` `{Integer}` the offset height of the window

Resizes a window by the specified amount.

## win.focus()

Focus on the window.

## win.blur()

Move focus away. Usually it will move focus to other windows of your app, since on some platforms there is no concept of blur.

## win.show([is_show])

* `is_show` `{Boolean}` _Optional_ specify whether the window should be shown or hidden. It's set to `true` by default.

Show the window if it's not shown.

`show(false)` has the same effect with `hide()`.

!!! note "Focus"
    `show` will not focus on the window on some platforms, so you need to call `focus` if you want to.

## win.hide()

Hide the window. User will not be able to find the window once it's hidden.

## win.close([force])

* `force` `{Boolean}` specify whether to close the window forcefully and bypass the `close` event.

Closes the current window. You can prevent the closing by listening to the `close` event. If `force` is specified and equals `true`, then the `close` event will be ignored.

Usually you would like to listen to the `close` event and do some shutdown work and then do a `close(true)` to really close the window.

```javascript
win.on('close', function () {
  this.hide(); // Pretend to be closed already
  console.log("We're closing...");
  this.close(true); // then close it forcefully
});

win.close(); // Would be detected by the above close event. To skip the above close event use win.close(true);
```

## win.reload()

Reloads the current window.

## win.reloadDev()

Reloads the current page by starting a new renderer process from scratch.

## win.reloadIgnoringCache()

Like `reload()`, but don't use caches (aka "shift-reload").

## win.maximize()

Maximize the window on GTK and Windows, and zoom the window on Mac OS X.

## win.unmaximize()

!!! warning "Deprecated"
    This feature is deprecated since 0.13.0. It's now replaced by [`restore` event](#event-restore). See [Migration Notes from 0.12 to 0.13](../For Users/Migration/From 0.12 to 0.13.md).

Unmaximize the window, i.e. the reverse of `maximize()`.

## win.minimize()

Minimize the window to task bar on Windows, iconify the window on GTK, and miniaturize the window on Mac OS X.

## win.restore()

Restore window to previous state after the window is minimized, i.e. the reverse of `minimize()`. It's not named `unminimize` since `restore` is used commonly.

## win.enterFullscreen()

Make the window fullscreen.

!!! note
    This function is different with HTML5 FullScreen API, which can make part of the page fullscreen, `Window.enterFullscreen()` will only fullscreen the whole window.

## win.leaveFullscreen()

Leave the fullscreen mode.

## win.toggleFullscreen()

Toggle the fullscreen mode.

## win.enterKioskMode()

Enter the Kiosk mode. In Kiosk mode, the app will be fullscreen and try to prevent users from leaving the app, so you should remember to provide a way in app to leave Kiosk mode. This mode is mainly used for presentation on public displays.

## win.leaveKioskMode()

Leave the Kiosk mode.

## win.toggleKioskMode()

Toggle the kiosk mode.

## win.setTransparent(transparent)

!!! warning "Deprecated"
    This feature is deprecated since 0.13.0. See [Migration Notes from 0.12 to 0.13](../For Users/Migration/From 0.12 to 0.13.md).

* `transparent` `{Boolean}` whether to set the window to be transparent

Turn on/off the transparency support. See more info on [Transparent Window](../For Users/Advanced/Transparent Window.md).

## win.setShadow(shadow) (Mac)

* `shadow` `{Boolean}` whether the window has a shadow

Turn the window's native shadow on/off. Useful for frameless, transparent windows.

## win.showDevTools([iframe], [callback])

!!! note
    This API is only available on SDK build flavor.

!!! warning "Behavior Changed"
    The behavior of the function is changed since 0.13.0. Please see [Migration Notes from 0.12 to 0.13](../For Users/Migration/From 0.12 to 0.13.md).

!!! note
    Since 0.75, the function throws an error if the build flavor is not SDK. This error can be caught in a try catch block.

* `iframe` `{String} or {HTMLIFrameElement}` _Optional_ the id or the element of the `<iframe>` to be jailed on. By default, the DevTools is shown for entire window.
* `callback(dev_win)` `{Function}` callback with the native window of the DevTools window.

Open the devtools to inspect the window.

The optional `iframe` as `String` should be the value of `id` attribute of any `<iframe>` element in the window. It jails the DevTools to inspect the `<iframe>` only. If it is an empty string, this feature has no effect.

The optional `iframe` as `HTMLIFrameElement` should be the iframe object. And it serves the same purpose with the `id` argument.

This function returns a `Window` object via the callback. You can use any properties and methods of `Window` except the events.

See also in [webview reference](webview Tag.md) on how to open DevTools for webview or open DevTools in a webview.

## win.closeDevTools()

!!! note
    This API is only available on SDK build flavor.

Close the devtools window.

## win.getPrinters(callback)

Enumerate the printers in the system. The callback function will receive an array of JSON objects for the printer information. The device name of the JSON object can be used as parameter in `nw.Window.print()`.

## win.isDevToolsOpen()

!!! note
    This API is only available on SDK build flavor.

Query the status of devtools window.

See also [`win.showDevTools()`](#winshowdevtoolsiframe-callback).

## win.print(options)

Print the web contents in the window with or without the need for user's interaction. `options` is a JSON object with the following fields:

* `autoprint` `{Boolean}` whether to print without the need for user's interaction; optional, true by default
* `silent` `{Boolean}` hide the flashing print preview dialog; optional, false by default
* `printer` `{String}` the device name of the printer returned by `nw.Window.getPrinters()`; No need to set this when printing to PDF
* `pdf_path` `{String}` the path of the output PDF when printing to PDF
* `headerFooterEnabled` `{Boolean}` whether to enable header and footer
* `landscape` `{Boolean}` whether to use landscape or portrait
* `mediaSize` `{JSON Object}` the paper size spec
* `shouldPrintBackgrounds` `{Boolean}` whether to print CSS backgrounds
* `marginsType` `{Integer}` 0 - Default; 1 - No margins; 2 - minimum; 3 - Custom, see `marginsCustom`.
* `marginsCustom` `{JSON Object}` the custom margin setting; units are points.
* `copies` `{Integer}` the number of copies to print.
* `scaleFactor` `{Integer}` the scale factor; `100` is the default.
* `headerString` `{String}` string to replace the URL in the header.
* `footerString` `{String}` string to replace the URL in the footer.

`marginsCustom` example: `"marginsCustom":{"marginBottom":54,"marginLeft":70,"marginRight":28,"marginTop":32}`  
`mediaSize` example: `'mediaSize':{'name': 'CUSTOM', 'width_microns': 279400, 'height_microns': 215900, 'custom_display_name':'Letter', 'is_default': true}`

*NOTE: If no options are being passed, `win.print({})` is what should be called.*

## win.setMaximumSize(width, height)

* `width` `{Integer}` the maximum width of the window
* `height` `{Integer}` the maximum height of the window

Set window's maximum size.

## win.setMinimumSize(width, height)

* `width` `{Integer}` the minimum width of the window
* `height` `{Integer}` the minimum height of the window

Set window's minimum size.

## win.setResizable(resizable)

* `resizable` `{Boolean}` whether the window can be resized

Set whether window is resizable.

## win.setAlwaysOnTop(top)

* `top` `{Boolean}` whether the window should always be on top

Sets the widget to be on top of all other windows in the window system.

## win.setVisibleOnAllWorkspaces(visible) (Mac and Linux)

* `visible` `{Boolean}` whether the window should be visible on all workspaces

For platforms that support multiple workspaces (currently Mac OS X and Linux), this allows NW.js windows to be visible on all workspaces simultaneously.

## win.canSetVisibleOnAllWorkspaces() (Mac and Linux)

Returns a a boolean indicating if the platform (currently Mac OS X and Linux) support Window API method `setVisibleOnAllWorkspace(Boolean)`.

## win.setPosition(position)

* `position` `{String}` the position of the window. There are three valid positions: `null` or `center` or `mouse`

Move window to specified position. Currently only `center` is supported on all platforms, which will put window in the middle of the screen.

## win.setShowInTaskbar(show)

* `show` `{Boolean}` whether show in task bar

Control whether to show window in taskbar or dock. See also `show_in_taskbar` in [Manifest-format](Manifest Format.md#show_in_taskbar).

## win.requestAttention(attension)

* `attension` `{Boolean} or {Integer}` If a Boolean, it indicates to request or cancel user's attension. If an Integer, it indicates the number of times the window flashes.

Request the user's attension by making the window flashes in the task bar.

!!! note "Mac"
    On Mac, value < 0 will trigger `NSInformationalRequest`, while value > 0 will trigger `NSCriticalRequest`.

## win.capturePage(callback [, config ])

* `callback` `{Function}` the callback when finished capturing the window
* `config` `{String} or {Object}` _Optional_ if a String, see `config.format` for valid values.
    - `format` `{String}` _optional_ the image format used to generate the image. It supports two formats: `"png"` and `"jpeg"`. If ignored, it's `"jpeg"` by default.
    - `datatype` `{String}` it supports three types: `"raw"`, `"buffer"` and `"datauri"`. If ignored, it's `"datauri"` by default.

Captures the visible area of the window. To capture the full page, see `win.captureScreenshot`.

!!! note "`raw` or `datauri`"
    The `"raw"` only contains the Base64 encoded image. But `"datauri"` contains the mime type headers as well, and it can be directly assigned to `src` of `Image` to load the image.

Example usage:
```javascript
// png as base64string
win.capturePage(function(base64string){
 // do something with the base64string
}, { format : 'png', datatype : 'raw'} );

// png as node buffer
win.capturePage(function(buffer){
 // do something with the buffer
}, { format : 'png', datatype : 'buffer'} );
```

## win.captureScreenshot(options [, callback])

When `callback` is omitted, a Promise is returned and it will resolve with `data` argument of the callback.

* `callback` `{Function}` _optional_ the callback when finished capturing the window. It's called with:
    - `err` `null` for success; `{String}` with the error message for failure.
    - `data` `{String}` base64 encoded image
* `options` `{Object}`
    - `fullSize` `{Boolean}` _optional_ Capture the whole page beyond the visible area. Currently the height of captured image is capped at 16384 pixels by Chromium.
    - `format` `{String}` _optional_ the image format used to generate the image. It supports two formats: `"png"` and `"jpeg"`. `"png"` is the default.
    - `quality` `{Integer}` _optional_ Compression quality from range [0..100] (jpeg only).
    - `clip` `{Object}` _optional_ Capture the screenshot of a given region only. The object's properties are:
      - `x`: `{Number}` X offset in device independent pixels (dip).
      - `y`: `{Number}` Y offset in device independent pixels (dip).
      - `width`: `{Number}` Rectangle width in device independent pixels (dip).
      - `height`: `{Number}` Rectangle height in device independent pixels (dip).
      - `scale`: `{Number}` Page scale factor.

Captures the the window. It can be used to capture the full page beyond the visible area.

!!! note Experimental
    This API is experimental and subject to change in the future.

Example usage:
```javascript
          win.captureScreenshot({fullSize: true}, (err, data) => {
            if (err) {
              alert(err.message);
              return;
            }
            var image = new Image();
            image.src = 'data:image/jpg;base64,' + data;
            document.body.appendChild(image);
          });
```
## win.setProgressBar(progress)

* `progress` `{Float}` valid values within [0, 1]. Setting to negative value (<0) removes the progress bar.

!!! note "Linux"
    Only Ubuntu is supported, and you'll need to specify the application `.desktop` file through `NW_DESKTOP` env. If `NW_DESKTOP` env variable is not found, it uses `nw.desktop` by default.

## win.setBadgeLabel(label)

Set the badge label on the window icon in taskbar or dock.

!!! note "Linux"
    This API is only supported on Ubuntu and the label is restricted to a string number only. You'll also need to specify the `.desktop` file for your application (see the note on [`setProgressBar`](#winsetprogressbar))

## win.eval(frame, script)

* `frame` `{HTMLIFrameElement}` the frame to execute in. If `iframe` is `null`, it assumes in current window / frame.
* `script` `{String}` the source code of the script to be executed

Execute a piece of JavaScript in the frame.

## win.evalNWBin(frame, path)

* `frame` `{HTMLIFrameElement}` the frame to execute in. If `iframe` is `null`, it assumes in current window / frame.
* `path` `{String|ArrayBuffer|Buffer}` the path or `Buffer` or `ArrayBuffer` of the binary file generated by `nwjc`

Load and execute the compiled binary in the frame. See [Protect JavaScript Source Code](../For Users/Advanced/Protect JavaScript Source Code.md).

## win.evalNWBinModule(frame, path, module_path)

!!! warning "experimental"
    This API is subject to change in future versions as we're exploring ways to support this feature better. [Discuss here](https://github.com/nwjs/nw.js/issues/6303).

* `frame` `{HTMLIFrameElement}` the frame to execute in. If `iframe` is `null`, it assumes in current window / frame.
* `path` `{String|ArrayBuffer|Buffer}` the path or `Buffer` or `ArrayBuffer` of the binary file generated by `nwjc`
* `module_path` `{String}` the module URL related to the current document. It will be used to [resolve the module specifier](https://html.spec.whatwg.org/multipage/webappapis.html#resolve-a-module-specifier).

Load and execute the compiled binary for Modules in the frame. The binary should be compiled with `nwjc --nw-module`. The following code will load lib.bin as module and other modules can refer to it with something like `import * from './lib.js'`:
```javascript
nw.Window.get().evalNWBinModule(null, 'lib.bin', 'lib.js');
```

## win.removeAllListeners([eventName])

Removes all listeners, or those of the specified `eventName`.

## Event: close

The `close` event is a special event that will affect the result of the `Window.close()` function. If developer is listening to the `close` event of a window, the `Window.close()` emit the `close` event without closing the window.

Usually you would do some shutdown work in the callback of `close` event, and then call `this.close(true)` to really close the window, which will not be caught again. Forgetting to add `true` when calling `this.close()` in the callback will result in infinite loop.

And if the shutdown work takes some time, users may feel that the app is exiting slowly, which is bad experience, so you could just hide the window in the `close` event before really closing it to make a smooth user experience.

See example code of [`win.close(true)` above](#wincloseforce) for the usage of `close` event.

!!! note "Mac"
    On Mac, there is an argument passed to the callback indicating whether it's being closed by <kbd>&#8984;</kbd>+<kbd>Q</kbd>. It will be set to string `quit` if that's true, otherwise `undefined`.

## Event: closed

The `closed` event is emitted after the corresponding window is closed. Normally you will not be able to get this event since after the window is closed all js objects will be released. But it is useful when listening to the window's events in another window, whose objects will not be released.

```javascript
// Open a new window.
nw.Window.open('popup.html', {}, function (win) {
  // Release the 'win' object here after the new window is closed.
  win.on('closed', function () {
    win = null;
  });

  // Listen to main window's close event
  nw.Window.get().on('close', function () {
    // Hide the window to give user the feeling of closing immediately
    this.hide();

    // If the new window is still open then close it.
    if (win !== null) {
      win.close(true);
    }
 
    // After closing the new window, close the main window.
    this.close(true);
  });
});

```

## Event: loading

Emitted when the window starts to reload, normally you cannot catch this event because usually it's emitted before you actually setup the callback.

The only situation that you can catch this event is when you refresh the window and listen to this event in another window.

## Event: loaded

Emitted when the window is fully loaded, this event behaves the same with `window.onload`, but doesn't rely on the DOM.

## Event: document-start(frame)

* `frame` `{HTMLIFrameElement}` is the iframe object, or `null` if the event is for the window. 

Emitted when the document object in this window or a child iframe is available, after all files are loaded, but before DOM is constructed or any script is run.
It will not be fired on the new window being created with nw.Window.open(): the callback of that function will be fired at the same point of this event.

See `inject_js_start` in [Manifest-format](Manifest Format.md#inject_js_start).

## Event: document-end(frame)

* `frame` `{HTMLIFrameElement}` is the iframe object, or `null` if the event is for the window. 
 
Emitted when the document object in this window or a child iframe is unloaded, but before the `onunload` event is emitted.

See `inject_js_end` in [Manifest-format](Manifest Format.md#inject_js_end)

## Event: focus

Emitted when window gets focus.

## Event: blur

Emitted when window loses focus.

## Event: minimize

Emitted when window is minimized.

## Event: restore

!!! warning "Behavior Changed"
    The behavior of the function is changed since 0.13.0. Please see [Migration Notes from 0.12 to 0.13](../For Users/Migration/From 0.12 to 0.13.md).

Emitted when window is restored from minimize, maximize and fullscreen state.

## Event: maximize

Emitted when window is maximized.

## Event: move(x, y)

Emitted after window is moved. The callback is called with 2 arguments: `(x, y)` for the new location of the left / top corner of the window.

## Event: resize(width, height)

Emitted after window is resized. The callback is called with 2 arguments: `(width, height)` for the new size of the window.

## Event: enter-fullscreen

Emitted when window enters fullscreen state.

## Event: leave-fullscreen

!!! warning "Deprecated"
    This feature is deprecated since 0.13.0. It's now replaced by [`restore` event](#event-restore). See [Migration Notes from 0.12 to 0.13](../For Users/Migration/From 0.12 to 0.13.md).

Emitted when window leaves fullscreen state.

## Event: zoom

Emitted when window zooming changed. It has a parameter indicating the new zoom level. See [`win.zoom()` method](#winzoomlevel) for the parameter's value definition.

## Event: capturepagedone

!!! warning "Deprecated"
    This feature is deprecated since 0.13.0. Use the callback with `win.capturePage()` instead. See [Migration Notes from 0.12 to 0.13](../For Users/Migration/From 0.12 to 0.13.md).

Emitted after the capturePage method is called and image data is ready. See `win.capturePage()` callback function for the parameter's value definition.

## Event: devtools-opened(url)

!!! warning "Deprecated"
    This feature is deprecated since 0.13.0. Use the `callback` passed to [`win.showDevtools`](#winshowdevtoolsiframe-callback) instead. See [Migration Notes from 0.12 to 0.13](../For Users/Migration/From 0.12 to 0.13.md).

See [`win.showDevTools()` method](#winshowdevtoolsiframe-callback) for more details.

## Event: devtools-closed

Emitted after Devtools is closed.

See [`win.closeDevTools()` method](#winclosedevtools) for more details.

## Event: new-win-policy (frame, url, policy)

* `frame` `{HTMLIFrameElement}` is the object of the child iframe where the request is from, or `null` if it's from the top window.
* `url` `{String}` is the address of the requested link
* `policy` `{Object}` is an object with the following methods:
    * `ignore()` : ignore the request, navigation won't happen.
    * `forceCurrent()` : force the link to be opened in the same frame
    * `forceDownload()` : force the link to be a downloadable, or open by external program
    * `forceNewWindow()` : force the link to be opened in a new window
    * `forceNewPopup()` : force the link to be opened in a new popup window
    * `setNewWindowManifest(m)` : control the options for the new popup window. The object `m` is in the same format as the [Window subfields](Manifest Format.md#window-subfields) in manifest format.

Emitted when a new window is requested from this window or a child iframe. You can call `policy.*` methods in the callback to change the default behavior of opening new windows.

For example, you can open the URL in system brower when user tries to open in a new window:
```javascript

nw.Window.get().on('new-win-policy', function(frame, url, policy) {
  // do not open the window
  policy.ignore();
  // and open it in external browser
  nw.Shell.openExternal(url);
});

```

## Event: navigation (frame, url, policy)

* `frame` `{HTMLIFrameElement}` is the object of the child iframe where the request is from, or `null` if it's from the top window.
* `url` `{String}` is the address of the requested link
* `policy` `{Object}` is an object with the following methods:
    * `ignore()` : ignore the request, navigation won't happen.

Emitted when navigating to another page. Similar to [`new-win-policy`](#event-new-win-policy-frame-url-policy), you can call `policy.ignore()` within the callback to ignore the navigation.


