# Window {: .doctitle}
---

!!! important "Available"
    Since 0.3.0

[TOC]

`Window` is a wrapper of the DOM's `window` object. It has extended operations and can receive various window events.

Every `Window` is an instance of the EventEmitter class, and you're able to use `Window.on(...)` to respond to native window's events.

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
var new_win = nw.Window.open('https://github.com');

// And listen to new window's focus event
new_win.on('focus', function() {
  console.log('New window is focused');
});

```

## Window.get([window_object])

!!! important "Available"
    Since 0.3.0

* `window_object` `{DOM Window}` _Optional_ is the DOM window
* Returns `{Window}` the native `Window` object

If `window_object` is not specifed, then return current window's `Window` object, otherwise return `window_object`'s `Window` object.

```javascript
// Get the current window
var win = nw.Window.get();

// Create a new window and get it
var new_win = nw.Window.open('https://github.com/nwjs/nw.js');
```

## Window.open(url[, options])

!!! important "Available"
    Since 0.3.0

* `url` `{String}` URL to be loaded in the opened window
* `options` `{Object}` _Optional_ see [Window subfields](Manifest Format.md#windowsubfields) in manifest format.
* Returns `{Window}` the opened native `Window` object

Open a new window and load `url` in it, you can specify extra `options` with the window. All [Window subfields](Manifest Format.md#windowsubfields) in manifest format.

Since 0.4.0, a boolean field `new-instance` can be used to start a new Node instance.

Since 0.9.0 and 0.8.5, `inject-js-start` and `inject-js-end` field can be used to inject a javascript file, see [Manifest format](Manifest Format.md#inject-js-start).

!!! note
    You should wait for the Window's `loaded` event before interacting with any of its native components.

    ```javascript
    var win = nw.Window.open ('https://github.com/nwjs/nw.js', {
      position: 'center',
      width: 901,
      height: 127
    });
    win.on ('loaded', function(){
      // the native onload event has just occurred
      var document = win.window.document;
    });
    ```

Since 0.7.3 the opened window is not focused by default. It's a result of unifying behavior across platforms. If you want it to be focused by default, you can set `focus` to `true` in `options`.

## win.window

!!! important "Available"
    Since 0.3.0

Get the corresponding DOM window object of the native window.

## win.x
## win.y

!!! important "Available"
    Since 0.3.0

Get or set left/top offset from window to screen.

## win.width
## win.height

!!! important "Available"
    Since 0.3.0

Get or set window's size.

## win.title

!!! important "Available"
    Since 0.3.0

Get or set window's title.

## win.menu

!!! important "Available"
    Since 0.3.0

Get or set window's menubar. Set with a Menu with type `menubar`. See [Menu](Menu.md).

## win.isFullscreen

!!! important "Available"
    Since 0.3.5

Get whether we're in fullscreen mode.

## win.isTransparent 

!!! important "Available"
    Since 0.11.2

Get whether transparency is turned on

## win.isKioskMode

!!! important "Available"
    Since 0.3.5

Get whether we're in kiosk mode.

## win.zoomLevel

!!! important "Available"
    Since 0.4.1

Get or set the page zoom. `0` for normal size; positive value for zooming in; negative value for zooming out.

## win.cookies.*

!!! important "Available"
    Since 0.8.1

This includes multiple functions to manipulate the cookies. The API is defined in the same way as [Chrome Extensions'](http://developer.chrome.com/extensions/cookies.html). NW.js supports the `get`, `getAll`, `remove` and `set` methods; `onChanged` event (supporting both `addListener` and `removeListener` function on this event).

And anything related to `CookieStore` in the Chrome extension API is not supported, because there is only one global cookie store in NW.js apps.

## win.moveTo(x, y)

!!! important "Available"
    Since 0.3.0

* `x` `{Integer}` offset to the left of the screen
* `y` `{Integer}` offset to the top of the screen

Moves a window's left and top edge to the specified coordinates.

## win.moveBy(x, y)

!!! important "Available"
    Since 0.3.0

* `x` `{Integer}` horizontal offset
* `y` `{Integer}` vertical offset

Moves a window a specified number of pixels relative to its current coordinates.

## win.resizeTo(width, height)

!!! important "Available"
    Since 0.3.0

* `width` `{Integer}` the width of the window
* `height` `{Integer}` the height of the window

Resizes a window to the specified `width` and `height`.

## win.resizeBy(width, height)

!!! important "Available"
    Since 0.3.0

* `width` `{Integer}` the offset width of the window
* `height` `{Integer}` the offset height of the window

Resizes a window by the specified amount.

## win.focus()

!!! important "Available"
    Since 0.3.0

Focus on the window.

## win.blur()

!!! important "Available"
    Since 0.3.0

Move focus away. Usually it will move focus to other windows of your app, since on some platforms there is no concept of blur.

## win.show([is_show])

!!! important "Available"
    Since 0.3.0

* `is_show` `{Boolean}` _Optional_ specify whether the window should be shown or hidden. It's set to `true` by default.

Show the window if it's not shown.

`show(false)` has the same effect with `hide()`.

!!! note
    `show` will not focus on the window on some platforms, so you need to call `focus` if you want to.

## win.hide()

!!! important "Available"
    Since 0.3.0

Hide the window. User will not be able to find the window once it's hidden.

## win.close([force])

!!! important "Available"
    Since 0.3.0

* `force` `{Boolean}` specify whether to close the window forcely and bypass `close` event.

Close current window. And you can prevent the closing by listening to the `close` event. If `force` is specified and equals to `true`, then the `close` event will be ignored.

Usually you would like to listen to the `close` event and do some shutdown work and then do a `close(true)` to really close the window.

```javascript
win.on('close', function() {
  this.hide(); // Pretend to be closed already
  console.log("We're closing...");
  this.close(true); // then close it forcely
});

win.close();
```

## win.reload()

!!! important "Available"
    Since 0.3.5

Reloads the current window.

## win.reloadDev()

!!! important "Available"
    Since 0.4.0

Reloads the current page by starting a new renderer process from scratch. This is the same as pressing the "reload" button in the right side of the toolbar.

## win.reloadIgnoringCache()

!!! important "Available"
    Since 0.3.5

Like `reload()`, but don't use caches (aka "shift-reload").

## win.maximize()

!!! important "Available"
    Since 0.3.0

Maximize the window on GTK and Windows, and zoom the window on Mac OS X.

## win.unmaximize()

!!! important "Available"
    Since 0.3.0

Unmaximize the window, i.e. the reverse of `maximize()`.

## win.minimize()

!!! important "Available"
    Since 0.3.0

Minimize the window to task bar on Windows, iconify the window on GTK, and miniaturize the window on Mac OS X.

## win.restore()

!!! important "Available"
    Since 0.3.0

Restore window to previous state after the window is minimized, i.e. the reverse of `minimize()`. It's not named `unminimize` since `restore` is used commonly.

## win.enterFullscreen()

!!! important "Available"
    Since 0.3.0

Make the window fullscreen.

!!! note
    This function is different with HTML5 FullScreen API, which can make part of the page fullscreen, `Window.enterFullscreen()` will only fullscreen the whole window.

## win.leaveFullscreen()

!!! important "Available"
    Since 0.3.0

Leave the fullscreen mode.

## win.toggleFullscreen()

!!! important "Available"
    Since 0.3.5

Toggle the fullscreen mode.

## win.enterKioskMode()

!!! important "Available"
    Since 0.3.1

Enter the Kiosk mode. In Kiosk mode, the app will be fullscreen and try to prevent users from leaving the app, so you should remember to provide a way in app to leave Kiosk mode. This mode is mainly used for presentation on public displays.

## win.leaveKioskMode()

!!! important "Available"
    Since 0.3.1

Leave the Kiosk mode.

## win.toggleKioskMode()

!!! important "Available"
    Since 0.3.5

Toggle the kiosk mode.

## win.setTransparent(transparent) 

!!! important "Available"
    Since 0.11.2

* `transparent` `{Boolean}` whether to set the window to be transparent

Turn on/off the transparency support. See more info on [Transparent Window](../For Users/Advanced/Transparent Window.md).

## win.showDevTools([iframe], [headless])

!!! important "Available"
    Since 0.3.0

!!! note
    This API is only available on SDK build flavor since 0.13.0.

* `iframe` `{String} or {HTMLIFrameElement}` _Optional_ the id or the element of the `<iframe>` to be jailed on. By default, the DevTools is shown for entire window.
* `headless` `{Boolean}` _Optional_ whether show DevTools in headless mode. If ignored, it's set to `false` by default.
* Returns `{Window}` the native window of the DevTools window when `headless` is false.

Open the devtools to inspect the window.

Since 0.6.0, the optional `iframe` as `String` is supported. It should be the value of `id` attribute of any `<iframe>` element in the window. It jails the DevTools to inspect the `<iframe>` only. If it is an empty string, this feature has no effect.

Since 0.6.0, the optional `headless` argument is supported. When it is `true`, the Devtools window will not be opened. Instead, a `devtools-opened` event will be emitted to the `Window` object after Devtools is ready.

Since 0.7.2, the optional `iframe` as `HTMLIFrameElement` is supported. It should be the iframe object. And it serves the same purpose with the `id` argument.

Since 0.8.1, this function returns a `Window` object when `headless` is `false`. You can use any properties and methods of `Window` except the events.

## win.closeDevTools()

!!! important "Available"
    Since 0.7.3

Close the devtools window.

## win.isDevToolsOpen()

!!! important "Available"
    Since 0.8.0

Query the status of devtools window.

!!! note
    This will always return false if the `headless` option was `true` when calling `showDevTools()`*

## win.setMaximumSize(width, height)

!!! important "Available"
    Since 0.3.0

* `width` `{Integer}` the maximum width of the window
* `height` `{Integer}` the maximum height of the window

Set window's maximum size.

## win.setMinimumSize(width, height)

!!! important "Available"
    Since 0.3.0

* `width` `{Integer}` the minimum width of the window
* `height` `{Integer}` the minimum height of the window

Set window's minimum size.

## win.setResizable(resizable)

!!! important "Available"
    Since 0.3.0

* `resizable` `{Boolean}` whether the window can be resized

Set whether window is resizable.

## win.setAlwaysOnTop(top)

!!! important "Available"
    Since 0.3.4

* `top` `{Boolean}` whether the window should always be on top

Sets the widget to be on top of all other windows in the window system.

## win.setVisibleOnAllWorkspaces(visible) (Mac and Linux)

!!! important "Available"
    Since 0.11.3

* `top` `{Boolean}` whether the window should be visible on all workspaces

For platforms that support multiple workspaces (currently Mac OS X and Linux), this allows NW.js windows to be visible on all workspaces simultaneously.

## win.canSetVisibleOnAllWorkspaces() (Mac and Linux)

!!! important "Available"
    Since 0.11.3

Returns a a boolean indicating if the platform (currently Mac OS X and Linux) support Window API method `setVisibleOnAllWorkspace(Boolean)`.

## win.setPosition(position)

!!! important "Available"
    Since 0.3.0

* `position` `{String}` the position of the window. There are three valid positions: `null` or `center` or `mouse`

Move window to specified position. Currently only `center` is supported on all platforms, which will put window in the middle of the screen.

## win.setShowInTaskbar(show)

!!! important "Available"
    Since 0.9.2

* `show` `{Boolean}` whether show in task bar

Control whether to show window in taskbar or dock. See also `show_in_taskbar` in [Manifest-format](Manifest Format.md#show_in_taskbar).

## win.requestAttention(attension)

!!! important "Available"
    Since 0.3.0

* `attension` `{Boolean} or {Integer}` If a Boolean, it indicates to request or cancel user's attension. If an Integer, it indicates the number of times the window flashes.

Request the user's attension by making the window flashes in the task bar.

Since 0.10.2, `attension` as Integer is supported on Windows and Mac. Additionally, on Mac, value < 0 will trigger `NSInformationalRequest`, while value > 0 will trigger `NSCriticalRequest`.

## win.capturePage(callback [, config ])

!!! important "Available"
    Since 0.4.2

* `callback` `{Function}` the callback when finished capturing the window
* `config` `{String} or {Object}` _Optional_ if a String, see `config.format` for valid values.
    - `format` `{String}` _optional_ the image format used to generate the image. It supports two formats: `"png"` and `"jpeg"`. If ignored, it's `"jpeg"` by default.
    - `datatype` `{String}` it supports three types: `"raw"`, `"buffer"` and `"datauri"`. If ignored, it's `"datauri"` by default.

Captures the visible area of the window.

!!! note "`raw` or `datauri`"
    The `"raw"` only contains the Base64 encoded image. But `"datauri"` contains the mime type headers as well, and it can be directly assigned to `src` of `Image` to load the image.

Since 0.9.3, `config` as `Object` is supported.

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

## win.setProgressBar(progress)

!!! important "Available"
    Since 0.10.2

* `progress` `{Float}` valid values within [0, 1]. Setting to negative value (<0) removes the progress bar.

!!! note Linux
    Only Ubuntu is supported, and you'll need to specify the application `.desktop` file through `NW_DESKTOP` env. If `NW_DESKTOP` env variable is not found, it uses `nw.desktop` by default.

## win.setBadgeLabel(label)

!!! important "Available"
    Since 0.10.0

Set the badge label on the window icon in taskbar or dock.

Since 0.10.2, Ubuntu is supported. However the label is restricted to a string number only. You'll also need to specify the `.desktop` file for your application (see the note on [`setProgressBar`](#winsetprogressbar))

## win.eval(frame, script)

!!! important "Available"
    Since 0.9.0 and 0.8.5

* `frame` `{HTMLIFrameElement}` the frame to execute in. If `iframe` is `null`, it assumes in current window / frame.
* `script` `{String}` the source code of the script to be executed

Execute a piece of JavaScript in the frame.

## win.evalNWBin(frame, path)

!!! important "Available"
    Since 0.12.0

* `frame` `{HTMLIFrameElement}` the frame to execute in. If `iframe` is `null`, it assumes in current window / frame.
* `path` `{String}` the path of the snapshot file generated by `nwjc`

Load and execute the compiled snapshot in the frame. See [Protect JavaScript Source Code with V8 Snapshot](../For Users/Advanced/Protect JavaScript Source Code with V8 Snapshot.md).

## Event: close

!!! important "Available"
    Since 0.3.0

The `close` event is a special event that will affect the result of the `Window.close()` function. If developer is listening to the `close` event of a window, the `Window.close()` emit the `close` event without closing the window.

Usually you would do some shutdown work in the callback of `close` event, and then call `this.close(true)` to really close the window, which will not be caught again. Forgetting to add `true` when calling `this.close()` in the callback will result in infinite loop.

And if the shutdown work takes some time, users may feel that the app is exiting slowly, which is bad experience, so you could just hide the window in the `close` event before really closing it to make a smooth user experience.

See example code of [`win.close(true)` above](#wincloseforce) for the usage of `close` event.

Since v0.8.4 on Mac, there is an argument passed to the callback indicating whether it's being closed by <kbd>&#8984;</kbd>+<kbd>Q</kbd>.

## Event: closed

!!! important "Available"
    Since 0.3.0

The `closed` event is emitted after corresponding window is closed. Normally you'll not be able to get this event since after the window is closed all js objects will be released. But it's useful if you're listening this window's events in another window, whose objects will not be released.

```javascript
// Open a new window.
var win = nw.Window.open('popup.html');

// Release the 'win' object here after the new window is closed.
win.on('closed', function() {
  win = null;
});

// Listen to main window's close event
nw.Window.get().on('close', function() {
  // Hide the window to give user the feeling of closing immediately
  this.hide();

  // If the new window is still open then close it.
  if (win != null)
    win.close(true);

  // After closing the new window, close the main window.
  this.close(true);
});
```

## Event: loading

!!! important "Available"
    Since 0.3.5

Emitted when the window starts to reload, normally you cannot catch this event because usually it's emitted before you actually setup the callback.

The only situation that you can catch this event is when you refresh the window and listen to this event in another window.

## Event: loaded

!!! important "Available"
    Since 0.3.5

Emitted when the window is fully loaded, this event behaves the same with `window.onload`, but doesn't rely on the DOM.

## Event: document-start(frame)

!!! important "Available"
    Since 0.9.0 and 0.8.5

* `frame` `{HTMLIFrameElement}` is the iframe object, or `null` if the event is for the window. 

Emitted when the document object in this window or a child iframe is available, after all files are loaded, but before DOM is constructed or any script is run.

See `inject-js-start` in [Manifest-format](Manfest Format.md#inject-js-start).

## Event: document-end(frame)

!!! important "Available"
    Since 0.9.0 and 0.8.5

* `frame` `{HTMLIFrameElement}` is the iframe object, or `null` if the event is for the window. 
 
Emitted when the document object in this window or a child iframe is unloaded, but before the `onunload` event is emitted.

See `inject-js-end` in [[Manifest-format]]

## Event: focus

!!! important "Available"
    Since 0.3.0

Emitted when window gets focus.

## Event: blur

!!! important "Available"
    Since 0.3.0

Emitted when window loses focus.

## Event: minimize

!!! important "Available"
    Since 0.3.0

Emitted when window is minimized.

## Event: restore

!!! important "Available"
    Since 0.3.0

Emitted when window is restored from minimize state.

## Event: maximize

!!! important "Available"
    Since 0.3.0

Emitted when window is maximized.

## Event: unmaximize

!!! important "Available"
    Since 0.3.0

Emitted when window is restored from maximize state.

!!! note
    On some platforms window can be resized even when maximized. The `unmaximize` may not be emitted when a maximized window is resized instead of being unmaximized

## Event: move(x, y)

!!! important "Available"
    Since 0.8.2

Emitted after window is moved. The callback is called with 2 arguments: `(x, y)` for the new location of the left / top corner of the window.

## Event: resize(width, height)

!!! important "Available"
    Since 0.8.2

Emitted after window is resized. The callback is called with 2 arguments: `(width, height)` for the new size of the window.

## Event: enter-fullscreen

!!! important "Available"
    Since 0.3.0

Emitted when window enters fullscreen state.

## Event: leave-fullscreen

!!! important "Available"
    Since 0.3.0

Emitted when window leaves fullscreen state.

## Event: zoom

!!! important "Available"
    Since 0.3.0

Emitted when window zooming changed. It has a parameter indicating the new zoom level. See [`win.zoom()` method](#winzoom) for the parameter's value definition.

## Event: capturepagedone

!!! warning "Deprecated"
    Since 0.13.0

Emitted after the capturePage method is called and image data is ready. See `win.capturePage()` callback function for the parameter's value definition.

This event is **deprecated** since 0.13.0. Use the callback with `win.capturePage()` instead.

## Event: devtools-opened(url)

!!! important "Available"
    Since 0.8.0

Emitted after Devtools is opened by any means, or ready after calling `win.showDevTools(id, headless)` with `headless` is `true`. The event callback has an `url` argument, which is the URL to load Devtools UI.

See [`win.showDevTools()` method](#winshowdevtools) for more details.

## Event: devtools-closed

!!! important "Available"
    Since 0.8.0

Emitted after Devtools is closed.

See [`win.closeDevTools()` method](#winclosedevtools) for more details.

## Event: new-win-policy (frame, url, policy)

!!! important "Available"
    Since 0.9.0 and 0.8.5

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

Since 0.11.3, `setNewWindowManifest(m)` is supported.

For example, you can open the URL in system brower when user tries to open in a new window:
```javascript

nw.Window.get().on('new-win-policy', function(frame, url, policy) {
  // do not open the window
  policy.ignore();
  // and open it in external browser
  nw.Shell.openExternal(url);
});

```
