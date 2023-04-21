# Manifest Format {: .doctitle}
---

[TOC]

Every app package should contain a manifest file named `package.json` in the format of [JSON](http://www.json.org/). This document will help you understand the meaning of each field of the manifest.

## Quick Start

Following is a minimal manifest:
```json
{
  "main": "index.html",
  "name": "nw-demo"
}
```

You only need at least these two fields to start with an NW.js app. Here is the quick explanation of them:

* `main`: tell NW.js to open `index.html` in the same folder as `package.json` at start
* `name`: gives the application a unique name called `nw-demo`

## Required Fields

Each package must provide all the following fields in its package descriptor file.

### main

* `{String}` which HTML page should be opened or which JavaScript file should be executed when NW.js starts.

You can specify a URL here. You can also specify just a filename (such as `index.html` or `script.js`) or a path (relative to the directory where your `package.json` resides).

### name

* `{String}` the name of the package. This must be a unique, lowercase alpha-numeric name without spaces. It may include "." or "_" or "-" characters. It is otherwise opaque. 

`name` should be globally unique since NW.js will store app's data under the directory named `name`.

## Features Control Fields

Following fields control which features NW.js should provide and how NW.js should open the main window.

### product_string

* `{String}` Use it to [rename the Helper application under macOS](http://docs.nwjs.io/en/latest/For%20Users/Package%20and%20Distribute/#mac-os-x).

### nodejs

* `{Boolean}` Setting `nodejs` to false will disable Node and NW.js APIs.

### node-main

* `{String}` Specify the path to a node.js script file. And it will be executed on startup in Node context before the first DOM window load.

### domain

* `{String}` Specify the host in the chrome-extension:// protocol URL used for the application. The web engine will share the same cookies between your application and the website under the same domain.

### single-instance

!!! warning "Deprecated"
    This property is deprecated since 0.13.0. See [Migration Notes from 0.12 to 0.13](../For Users/Migration/From 0.12 to 0.13.md).

* `{Boolean}` Specify whether to start up single instance of your app. It's set to `true` by default.

By default NW.js only allows one instance of your app. If you want to allow multiple instances of your app running at the same time, set this to `false`.

### bg-script

* `{String}` background script. The script is executed in the background page at the start of application.

### window

* `{Object}` controls how the window looks, see [Window Subfields](#window-subfields) below.

### webkit

* `{Object}` controls what features of WebKit should be on/off, see [WebKit Subfields](#webkit-subfields) below.

### user-agent

* `{String}` Override the `User-Agent` header in HTTP requests made from the application.

The following placeholders are available to composite the user agent dynamically:

* `%name`: replaced by the `name` field in the manifest.
* `%ver`: replaced by the `version` field in the manifest, if available.
* `%nwver`: replaced by the version of NW.js.
* `%webkit_ver`: replaced by the version of Blink engine.
* `%chromium_ver`: replaced by the Chromium version
* `%osinfo`: replaced by the OS and CPU information you would see in browser's user agent string.

### node-remote

!!! warning "Behavior Changed"
    This feature is changed in 0.13.0. See [Migration Notes from 0.12 to 0.13](../For Users/Migration/From 0.12 to 0.13.md).

* `{Array}` or `{String}` Enable calling Node in remote pages. The value controls for which sites this feature should be turned on. Each item in the array follows the [match patterns](https://developer.chrome.com/extensions/match_patterns) used in Chrome extension.

A match pattern is essentially a URL that begins with a permitted scheme (`http`, `https`, `file`, or `ftp`, and that can contain `'*'` characters. The special pattern `<all_urls>` matches any URL that starts with a permitted scheme. Each match pattern has 3 parts:

* `scheme` — for example, `http` or `file` or `*`
* `host` — for example, `www.google.com` or `*.google.com` or `*`; if the scheme is file, there is no host part
* `path` — for example, `/*`, `/foo*`, or `/foo/bar`. The path must be present in a host permission, but is always treated as `/*`.

Here's the basic syntax:

```none
<url-pattern> := <scheme>://<host><path>
<scheme> := '*' | 'http' | 'https' | 'file' | 'ftp'
<host> := '*' | '*.' <any char except '/' and '*'>+
<path> := '/' <any chars>
```

### chromium-args

* `{String}` Specify chromium (content shell) command line arguments.

It will be useful if you want to distribute the app with some custom chromium args. For example, if you want to disable the GPU accelerated video display, just add `"chromium-args" : "--disable-accelerated-video"`. If you want to add multiple arguments, separate each two arguments by space. This field can take a number of flags in one argument as well, via enclosing them in single quotation marks.

The value of `NW_PRE_ARGS` environment variable is prepended to the value of this field.

See [Command Line Options](Command Line Options.md) for more information.

### crash_report_url

* `{String}` URL of the crash report server

Once the app crashed, the crash dump file and information about the runtime environment will be sent to the crash server. It's sent in the same way as in Chromium browser: a HTTP POST request with `multipart/form-data` as the content type. In theory, any breakpad/crashpad server could handle the request, since breakpad/crashpad work in the same way in NW as they do in Chromium. See [a very simple server](https://github.com/nwjs/nw.js/blob/nw21/test/sanity/crash-dump-report/crash_server.py) used in our test case, or [simple-breakpad-server](https://github.com/acrisci/simple-breakpad-server).

The request contains the following field at least:

* `prod` - the `name` field in the manifest of your application
* `ver` - the `version` field in the manifest of your application
* `upload_file_minidump` - the binary contents of the minidump file
* `switch-n` - the command line switches of the crashing process. There are multiple fields for each switch where `n` is a number starting from 1.

### js-flags

* `{String}` Specify the flags passed to JS engine (v8). e.g. turn on Harmony Proxies and Collections feature:

```json
{
  "name": "nw-demo",
  "main": "index.html",
  "js-flags": "--harmony_proxies --harmony_collections"
}
```

A list of supported V8 flags can be found here: https://chromium.googlesource.com/v8/v8/+/master/src/flag-definitions.h

### inject_js_start
### inject_js_end

* `{String}` a local filename, relative to the application path, used to specify a JavaScript file to inject to the window.

`inject_js_start`: The injecting JavaScript code is to be executed after any files from css, but before any other DOM is constructed or any other script is run.

`inject_js_end`: The injecting JavaScript code is to be executed after the document object is loaded, before `onload` event is fired. This is mainly to be used as an option of `Window.open()` to inject JS in a new window. 

### additional_trust_anchors

* `{Array}` Containing a list of PEM-encoded certificates (i.e. `"-----BEGIN CERTIFICATE-----\n...certificate data...\n-----END CERTIFICATE-----\n"`).

These certificates are used as additional root certificates for validation, to allow connecting to services using a self-signed certificate or certificates issued by custom CAs.

### dom_storage_quota

* `{Integer}` Number of mega bytes (MB) for the quota of the DOM storage. The suggestion is to put double the value you want.

### no-edit-menu (Mac)

!!! warning "Deprecated"
    This property is deprecated since 0.13.0. See [Migration Notes from 0.12 to 0.13](../For Users/Migration/From 0.12 to 0.13.md).

* `{Boolean}` whether the default `Edit` menu should be disabled on Mac OS X. The default value is `false`. Only effective on Mac OS X.

## Window Subfields

Most of window subfields are inherited by sub windows opened by `window.open()` or links (`<a target="_blank">`) by default. The exception list of non inherited subfields are as following. They will be set to default value for opened window:

* `fullscreen` -> `false`
* `kiosk` -> `false`
* `position` -> `null`
* `resizable` -> `true`
* `show` -> `true`

All of the window subfields can be overwritten by using [`new-win-policy` event](Window.md#event-new-win-policy-frame-url-policy).

### id

* `{String}` the `id` used to identify the window. This will be used to remember the size and position of the window and restore that geometry when a window with the same id is later opened. [See also the Chrome App documentation](https://developer.chrome.com/apps/app_window#type-CreateWindowOptions)

### title

* `{String}` the default title of window created by NW.js, it's very useful if you want to show your own title when the app is starting.

### width
### height

* `{Integer}` the initial inner width/height of the main window.

### toolbar

!!! warning "Deprecated"
    This property is deprecated since 0.13.0. See [Migration Notes from 0.12 to 0.13](../For Users/Migration/From 0.12 to 0.13.md).

* `{Boolean}` should the navigation toolbar be showed.

### icon

* `{String}` path to window's icon

### position

* `{String}` be `null` or `center` or `mouse`, controls where window will be put

### min_width
### min_height

* `{Integer}` minimum inner width/height of window

### max_width
### max_height

* `{Integer}` maximum inner width/height of window

### as_desktop (Linux)

* `{Boolean}` show as desktop background window under X11 environment

### resizable

* `{Boolean}` whether window is resizable

Beware, if resizable is set to false on OS X and frame is set to true, the user will still be able to make the window fullscreen. Set fullscreen to false to disable the fullscreen control.

### always_on_top

!!! warning "Behavior Changed"
    This feature is changed in 0.13.0. See [Migration Notes from 0.12 to 0.13](../For Users/Migration/From 0.12 to 0.13.md).

* `{Boolean}` whether the window should always stay on top of other windows.

### visible_on_all_workspaces (Mac & Linux)

!!! warning "Behavior Changed"
    This feature is changed in 0.13.0. See [Migration Notes from 0.12 to 0.13](../For Users/Migration/From 0.12 to 0.13.md).

* `{Boolean}` whether the window should be visible on all workspaces simultaneously (on platforms that support multiple workspaces, currently Mac OS X and Linux).

### fullscreen

* `{Boolean}` whether window is fullscreen

Beware, if frame is also set to false in fullscreen it will prevent the mouse from being captured on the very edges of the screen. You should avoid activate it if fullscreen is also set to true.

### show_in_taskbar

* `{Boolean}` whether the window is shown in taskbar or dock. The default is `true`. 

### frame

* `{Boolean}` specify it to `false` to make the window frameless 

Beware, if frame is set to false in fullscreen it will prevent the mouse from being captured on the very edges of the screen. You should avoid activating it if fullscreen is also set to true.

Frameless apps do not have a title bar for the user to click and drag the window. You can use CSS to designate DOM elements as draggable regions.

```css
.drag-enable {
  -webkit-app-region: drag;
}
.drag-disable {
  -webkit-app-region: no-drag;
}
```

### show

* `{Boolean}` specify it to `false` if you want your app to be hidden on startup 

### kiosk

* `{Boolean}` whether to use `Kiosk` mode. In `Kiosk` mode, the app will be fullscreen and try to prevent users from leaving the app, so you should remember to provide a way in app to leave `Kiosk` mode. This mode is mainly used for presentation on public displays

### transparent

* `{Boolean}` whether to turn on transparent window mode. The default is `false`.   

Control the transparency with rgba background value in CSS. Use command line option `--disable-transparency` to disable this feature completely.

There is experimental support for "click-through" on the transparent region: add `--disable-gpu` option to the command line.

## WebKit Subfields

### double_tap_to_zoom_enabled

* `{Boolean}` enable zooming with double tapping on mac with 2 fingers, default is false.

### plugin

* `{Boolean}` whether to load external browser plugins like Flash, default to true.

## Other Fields

The [Packages/1.0](http://wiki.commonjs.org/wiki/Packages/1.0) standard specifies many other fields `package.json` should provide. Currently we don't make use of them, but you provide them still.
