# Manifest Format {: .doctitle}
---

[TOC]

Every app package should contain a manifest file named `package.json` in the format of [JSON](http://www.json.org/). This document will help you understand the meaning of each field of the manifest.

## Quick Start

Following is a minimal manifest:
```json
{
  "main": "index.html",
  "name": "nw-demo",
}
```

You only need at least these two fields to start with an NW.js app. Here is the quick explanation of them:
* `main`: tell NW.js to open `index.html` in the same folder as `package.json` at start
* `name`: gives the application a unique name called `nw-demo`

## Required Fields

Each package must provide all the following fields in its package descriptor file.

### main

* `{String}` which page should be opened when NW.js starts.

You can specify a URL here. You can also specify just a filename (such as `index.html`) or a path (relative to the directory where your `package.json` resides).

Note that you should not use parameters after a filename (such as `index.html?foo=bar`) because they'll be treated as parts of the file's path (for example, `index.html?foo=bar/baz` is a file `baz` in a directory named `index.html?foo=bar`) and that's probably not what you expect. When you need to pass some parameters to a local application, use a URL with the [App protocol](../For Users/Advanced/App Protocol.md).

### name

* `{String}` the name of the package. This must be a unique, lowercase alpha-numeric name without spaces. It may include "." or "_" or "-" characters. It is otherwise opaque. 

`name` should be globally unique since NW.js will store app's data under the directory named `name`.

## Features Control Fields

Following fields control which features NW.js should provide and how NW.js should open the main window.

### nodejs

* `{Boolean}` set `nodejs` to false will disable Node support in NW.js.

### node-main

!!! important "Available"
    Since 0.3.1

* `{String}` Specify the path to a node.js script file. And it will be executed on startup in Node context before the first DOM window load.

### single-instance

* `{Boolean}` Specify whether to start up single instance of your app. It's set to `true` by default.

By default NW.js only allows one instance of your app. If you want to allow multiple instances of your app running at the same time, set this to `false`.

!!! note
    Before v0.10.0, this field only effect packaged applications. See [Package and Distribute](../For Users/Package and Redistribute.md) for detailed information.

### window

* `{Object}` controls how the main window looks, see [Window Subfields](#windowsubfields) below.

### webkit

* `{Object}` controls what features of WebKit should be on/off, see [WebKit Subfields](#webkitsubfields) below.

### user-agent

!!! important "Available"
    Since 0.3.7

* `{String}` Override the `User-Agent` header in HTTP requests made from the application.

The following placeholders are available to composite the user agent dynamically:

* `%name`: replaced by the `name` field in the manifest.
* `%ver`: replaced by the `version` field in the manifest, if available.
* `%nwver`: replaced by the version of NW.js.
* `%webkit_ver`: replaced by the version of WebKit engine.
* `%osinfo`: replaced by the OS and CPU information you would see in browser's user agent string.

### node-remote

!!! important "Available"
    Since 0.3.7

* `{String}` Enable calling Node in remote pages. The value controls for which sites this feature should be turned on.

The format is the same with the "proxy bypass rules" of the browser. It's a rule set, which rules are separated by `,` or `;`. Each rule can be any of the following:

```none
  // (1) [ URL_SCHEME "://" ] HOSTNAME_PATTERN [ ":" <port> ]
  //
  //   Match all hostnames that match the pattern HOSTNAME_PATTERN.
  //
  //   Examples:
  //     "foobar.com", "*foobar.com", "*.foobar.com", "*foobar.com:99",
  //     "https://x.*.y.com:99"
  //
  // (2) "." HOSTNAME_SUFFIX_PATTERN [ ":" PORT ]
  //
  //   Match a particular domain suffix.
  //
  //   Examples:
  //     ".google.com", ".com", "http://.google.com"
  //
  // (3) [ SCHEME "://" ] IP_LITERAL [ ":" PORT ]
  //
  //   Match URLs which are IP address literals.
  //
  //   Conceptually this is the similar to (1), but with special cases
  //   to handle IP literal canonicalization. For example matching
  //   on "[0:0:0::1]" would be the same as matching on "[::1]" since
  //   the IPv6 canonicalization is done internally.
  //
  //   Examples:
  //     "127.0.1", "[0:0::1]", "[::1]", "http://[::1]:99"
  //
  // (4)  IP_LITERAL "/" PREFIX_LENGHT_IN_BITS
  //
  //   Match any URL that is to an IP literal that falls between the
  //   given range. IP range is specified using CIDR notation.
  //
  //   Examples:
  //     "192.168.1.1/16", "fefe:13::abc/33".
  //
  // (5)  "<local>"
  //
  //   Match local addresses. The meaning of "<local>" is whether the
  //   host matches one of: "127.0.0.1", "::1", "localhost".
```

### chromium-args

!!! important "Available"
    Since 0.4.0

* `{String}` Specify chromium (content shell) command line arguments.

It will be useful if you want to distribute the app with some custom chromium args. For example, if you want to disable the GPU accelerated video display, just add `"chromium-args" : "--disable-accelerated-video"`. If you want to add multiple arguments, separate each two arguments by space. This field can take a number of flags in one argument as well, via enclosing them in single quotation marks.

A list of Chromium's command line arguments is available at 
http://peter.sh/experiments/chromium-command-line-switches/

### js-flags

!!! important "Available"
    Since 0.4.1

* `{String}` Specify the flags passed to JS engine (v8). e.g. turn on Harmony Proxies and Collections feature:

```json
{
  "name": "nw-demo",
  "main": "index.html",
  "js-flags": "--harmony_proxies --harmony_collections"
}
```

### inject-js-start
### inject-js-end

!!! important "Available"
    Since 0.9.0

!!! important "Available"
    Since 0.8.5

* `{String}` a local filename, relative to the application path, used to specify a JavaScript file to inject to the window.

`inject-js-start`: The injecting JavaScript code is to be executed after any files from css, but before any other DOM is constructed or any other script is run.

`inject-js-end`: The injecting JavaScript code is to be executed after the document object is loaded, before `onload` event is fired. This is mainly to be used as an option of `Window.open()` to inject JS in a new window. 

### additional_trust_anchors

!!! important "Available"
    Since 0.11.1 

* `{String}` Containing a list of PEM-encoded certificates (i.e. `"-----BEGIN CERTIFICATE-----\n...certificate data...\n-----END CERTIFICATE-----\n"`).

These certificates are used as additional root certificates for validation, to allow connecting to services using a self-signed certificate or certificates issued by custom CAs.

### snapshot

!!! important "Available"
    Since 0.4.2

* `{String}` Specify the path to the snapshot file to be loaded with the application. The snapshot file contains compiled code of your application. See [Protect JavaScript Source Code with V8 Snapshot](../For Users/Advanced/Protect JavaScript Source Code with V8 Snapshot.md).

### dom_storage_quota

!!! important "Available"
    Since 0.6.1

* `{Integer}` Number of mega bytes (MB) for the quota of the DOM storage. The suggestion is to put double the value you want.

### no-edit-menu (Mac)

!!! warning "TBD"
    This feature need to be confirmed.

!!! important "Available"
    Since 0.7.3

* `{Boolean}` whether the default `Edit` menu should be disabled on Mac OS X. The default value is `false`. Only effective on Mac OS X. **This is a workaround for a feature request and is expected to be replaced by something else soon**

## Window Subfields

### title

* `{String}` the default title of window created by NW.js, it's very useful if you want to show your own title when the app is starting.

### width
### height

* `{Integer}` the initial width/height of the main window.

### toolbar

!!! warning "Deprecated"
    Since 0.13.0

* `{Boolean}` should the navigation toolbar be showed.

This field is **deprecated in 0.13.0**.

### icon

* `{String}` path to window's icon

### position

* `{String}` be `null` or `center` or `mouse`, controls where window will be put

### min_width
### min_height

* `{Integer}` minimum width/height of window

### max_width
### max_height

* `{Integer}` maximum width/height of window

### as_desktop (Linux)

* `{Boolean}` show as desktop background window under X11 environment

### resizable

!!! important "Available"
    Since 0.3.0

* `{Boolean}` whether window is resizable

Beware, if resizable is set to false on OS X and frame is set to true, the user will still be able to make the window fullscreen. Set fullscreen to false to disable the fullscreen control.

### always-on-top

!!! important "Available"
    Since 0.3.4

* `{Boolean}` whether the window should always stay on top of other windows.

### visible-on-all-workspaces (Mac & Linux)

!!! important "Available"
    Since 0.11.3

* `{Boolean}` whether the window should be visible on all workspaces simultaneously (on platforms that support multiple workspaces, currently Mac OS X and Linux).

### fullscreen

!!! important "Available"
    Since 0.3.0

* `{Boolean}` whether window is fullscreen

Beware, if frame is also set to false in fullscreen it will prevent the mouse from being captured on the very edges of the screen. You should avoid activate it if fullscreen is also set to true.

### show_in_taskbar

!!! important "Available"
    Since 0.9.2

* `{Boolean}` whether the window is shown in taskbar or dock. The default is `true`. 

### frame

!!! important "Available"
    Since 0.3.0

* `{Boolean}` specify it to `false` to make the window frameless 

Beware, if frame is set to false in fullscreen it will prevent the mouse from being captured on the very edges of the screen. You should avoid activate it if fullscreen is also set to true.

### show

!!! important "Available"
    Since 0.3.0

* `{Boolean}` specify it to `false` if you want your app to be hidden on startup 

### kiosk

!!! important "Available"
    Since 0.3.1

* `{Boolean}` whether to use `Kiosk` mode. In `Kiosk` mode, the app will be fullscreen and try to prevent users from leaving the app, so you should remember to provide a way in app to leave `Kiosk` mode. This mode is mainly used for presentation on public displays

### transparent

!!! important "Available"
    Since 0.11.2

* `{Boolean}` whether to turn on transparent window mode. The default is `false`.   

Control the transparency with rgba background value in CSS. Use command line argument `--disable-transparency` to disable this feature completely.

There is experimental support for "click-through" on the transparent region: add `--disable-gpu` argument to the command line. See the discussion here: https://github.com/nwjs/nw.js/issues/132

## WebKit Subfields

!!! important "Available"
    Since 0.3.0

### plugin

* `{Boolean}` whether to load external browser plugins like Flash, default to false.

### java

* `{Boolean}` whether to load Java applets, default to false.

### page-cache

* `{Boolean}` whether to enable page cache, default to false.

## Other Fields

The [Packages/1.0](http://wiki.commonjs.org/wiki/Packages/1.0) standard specifies many other fields `package.json` should provide. Currently we don't make use of them, but you provide them still.
