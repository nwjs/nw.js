# Security in NW.js
---

[TOC]

## Node and Normal Frames
There are two kinds of frames in NW.js: node frame and normal frame.

**Node frames** have following extra capabilities than **normal frames**:

* Access Node.js / NW.js APIs
* Access extended DOM features, such as [save as dialog](../../References/Changes to DOM.md#attribute-nwsaveas), [nwUserAgent attribute](../../References/Changes to DOM.md#attribute-nwuseragent) etc.
* Bypass all security restrictions, such as sandboxing, same origin policy etc. For example, you can make cross origin XHR to any remote sites, or access to `<iframe>` element whose `src` points to remote sites in node frames.

In NW.js, frames matching **ALL** of the following criterias will be a **node frame**:

* `nodejs` set to `true` in [manifest file](../../References/Manifest Format.md#nodejs)
* URL of windows and frames matching `node-remote` patterns in [manifest file](../../References/Manifest Format.md#node-remote) or `chrome-extension://` protocol.
* Frames or parent frames **NOT** having [`nwdisable` attribute](../../References/Changes to DOM.md#attribute-nwdisable).
* Frames or parent frames **NOT** in [`<webview>` tag](../../References/webview Tag.md)
