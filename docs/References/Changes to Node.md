# Changes to Node {: .doctitle}
---

[TOC]

## console
Since NW.js supports GUI applications instead of console applications, the output of `console.log()` (and other similar methods such as `console.warn()` and `console.error()`) is redirected to the console of Chromium. You may see it in your "[Developer Tools](../For Users/Debugging with DevTools.md)" window (on its "Console" tab).

## process
A couple of new fields is added to the global `process` object:

* `process.versions['nwjs']` is set with NW.js's version.
* `process.versions['chromium']` is set with the chromium version which NW.js is based on.
* `process.mainModule` is set for the start page (such as `index.html`) as specified in the manifest's [`main`](Manifest Format.md#main) field. However, when the [`node-main`](Manifest Format.md#node-main) field is also specified in the manifest, `process.mainModule` points to the file specified in the `node-main` field.

## require
Behaviour of relative paths in Node's `require()` method depends on the [JavaScript context](../For Users/Advanced/JavaScript Contexts in NW.js.md) in which the parent file is running in. (where "the parent file" is the file in which the `require()` method is called):

* If the parent file is running in Node context, the child's relative path is treated as relative to its parent.
* If the parent file is running in browser context, the child's relative path is treated as relative to the application's root directory, i.e. the directory of manifest file.
