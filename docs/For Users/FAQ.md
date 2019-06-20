# FAQ {: .doctitle}
---

[TOC]


## console.log doesn't output to Linux terminal
`--enable-logging=stderr` should be used in the command line; See more here: https://www.chromium.org/for-testers/enable-logging


## `var crypto = require('crypto')` gets a wrong object

Chromium has its own global `crypto` object which can't be overwritten. So you can't use the same variable name `crypto`. Changing your variable name to something else, like `nodeCrypto`, will work.


## Images are broken in AngularJS and receive `Failed to load resource XXX net::ERR_UNKNOWN_URL_SCHEME` in DevTools

AngularJS added `unsafe:` prefix for unknown scheme to prevent XSS attack. URLs in NW.js and Chrome apps are started with `chrome-extension:` scheme, which is unknown to AngularJS. The solution is to config the whitelist of known schemes with AngularJS by adding following lines:

```javascript
myApp.config(['$compileProvider',
  function($compileProvider) {
    $compileProvider.imgSrcSanitizationWhitelist(/^\s*((https?|ftp|file|blob|chrome-extension):|data:image\/)/);
    $compileProvider.aHrefSanitizationWhitelist(/^\s*(https?|ftp|mailto|tel|file|chrome-extension):/);
  }]);
```


## Can't see exception reporting in AngularJS 2+

AngularJS 2 tries to register exception handlers in global variable `global`. However, it's already existed in NW.js environment, which prevents exception reporting shown in DevTools. The workaround is to rename `global` to something else before loading any AngularJS libraries. For example,

```html
<script>
window.nw_global = window.global;
window.global = undefined;
</script>
<!-- Angular 2 Dependencies -->
```

* See also:
  * [Tutorial: Angular-CLI and NW.js](https://dev.to/thejaredwilcurt/angular-cli-and-nwjs-for-development-49gl)
  * [NW.js + Angular 8 demo repo](https://github.com/nwutils/nw-angular-cli-example)


## How to leave fullscreen mode with ESC key?

Usually users expect to use ESC key to quit fullscreen mode. By default, NW.js don't bind ESC shortcut for leaving fullscreen mode, but providing APIs for entering and leaving fullscreen mode. It will give developers more control on fullscreen mode.

To enable ESC key for leaving fullscreen mode, you can use [Shortcut API](../References/Shortcut.md):

```javascript
nw.App.registerGlobalHotKey(new nw.Shortcut({
  key: "Escape",
  active: function () {
    // decide whether to leave fullscreen mode
    // then ...
    nw.Window.get().leaveFullscreen();
  }
}));
```

## How do I get "intellisense" (auto-complete) in VSCode for `nw`?

![Screenshot of VSCode Intellisense for NW.js](https://user-images.githubusercontent.com/4629794/59388510-ea981b00-8d39-11e9-8e18-bf3425901410.png)

1. Install nwjs-types
   ```
   npm install --save-dev nwjs-types
   ```
1. Force VSCode's TypeScript engine to load.
   * Place `// @ts-check` at the top of your JS file.
   * You do not need to use TypeScript for this to work.
   * VSCode will load the TypeScript engine when it sees `// @ts-check`, then look for types in your `node_modules`, then find and load `nwjs-types`.
   * Then intellisense will know how to auto-complete for `nw.`.
