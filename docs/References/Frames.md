# Frames {: .doctitle}
---

[TOC]

NW.js extended Chromium for easier developing native apps. Thoses features enables apps to bypass the restrictions of Sandbox and Same Origin Policy. However in some cases, you want to embed the 3rd-party web page within your app, but do not want the 3rd-party web page to bypass these security settings. NW.js provided two ways to achieve this: using `<webview>` tag provided by Chrome App or `<iframe>` with additional attributes.

## `<webview>`

!!! important "Available"
    Since 0.12.0

Use the `<webview>` tag to embed 'guest' content (such as web pages) in your app. Different from the `<iframe>`, the `<webview>` runs in a separate process than your app; it doesn't have the same permissions as your app and all interactions between your app and embedded content will be asynchronous. This keeps your app safe from the embedded content.

### Synopsis

To embed a web page in your app, add the webview tag to your app's embedder page (this is the app page that will display the guest content). In its simplest form, the webview tag includes the src of the web page and css styles that control the appearance of the webview container:

```html
<webview id="foo" src="http://www.google.com/" style="width:640px; height:480px"></webview>
```

### References

See [Chrome document of `<webview>` tag](https://developer.chrome.com/apps/tags/webview) for detailed API references.

## `<iframe>`

!!! important "Available"
    Since 0.5.0

There are two kinds of frames in NW.js: node frame and normal frame.

Node frames are able to access Node.js and NW.js APIs and can bypass all security restrictions, such as sandboxing, same origin policy etc.; normal frames do not have these features and web pages are running as in a browser.

In NW.js, frames matching following criterias will be a normal frame. Otherwise it's a node frame:

* `nodejs` set to `false` in [manifest file](Manifest Format.md#nodejs) (default)
* `nodejs` set to `false` when window is opened with NW.js API [`Window.open()`](Window.md#windowopen) (default)
* Frames or parent frames has `nwdisable` attribute. See [`nwdisable` attribute](#attribute-nwdisable) below.
* The URL of the loaded web page is **NOT** under [`app://` protocol](../For Users/Advanced/App Protocol.md), or `nw:` protocol
* The URL of the loaded web page does **NOT** match `node-remote` defined in [manifest file](Manifest Format.md#node-remote)

### Attribute: nwdisable

!!! important "Available"
    Since 0.5.0

Make the frame and subframes normal frames.

!!! note
    This attribute doesn't prevent the page in normal frames accessing its parent and top frames. Then they can still get access to Node.js APIs. So usually you may want to use this attribute with `nwfaketop` togather.

### Attribute: nwfaketop

!!! important "Available"
    Since 0.5.1

Prevent web page in the frame accessing the real `window.parent` or `window.top`. They will get the faked parent and top, which is its own `window` object of the frame. Subframes will also be affected.

This attribute is usually used with `nwdisable`.

### Attribute: nwUserAgent

!!! important "Available"
    Since 0.9.0

Override the default user-agent when loading the web page in the frame and sub frames. See [`user-agent` in manifest](Manifest Format.md#user-agent) for details.

