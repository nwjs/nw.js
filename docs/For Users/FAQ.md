# FAQ {: .doctitle}
---

[TOC]

## console.log doesn't output to Linux terminal
`--enable-logging=stderr` should be used in the command line; See more here: https://www.chromium.org/for-testers/enable-logging

## `var crypto = require('crypto')` gets a wrong object
Chromium has its own global `crypto` object which can't be overwritten. So you can't use the same variable name `crypto`. Changing your variable name to something else, like `nodeCrypto`, will work.

## Images are broken in AnugarJS and receive `Failed to load resource XXX net::ERR_UNKNOWN_URL_SCHEME` in DevTools
AngularJS added `unsafe:` prefix for unknown scheme to prevent XSS attack. URLs in NW.js and Chrome apps are started with `chrome-extension:` scheme, which is unknown to AnuglarJS. The solution is to config the whitelist of known schemes with AngularJS by adding following lines:

```javascript
myApp.config(['$compileProvider',
  function($compileProvider) {
    $compileProvider.imgSrcSanitizationWhitelist(/^\s*((https?|ftp|file|blob|chrome-extension):|data:image\/)/);
    $compileProvider.aHrefSanitizationWhitelist(/^\s*(https?|ftp|mailto|tel|file:chrome-extension):/);
  }]);
```

## Can't see exception reporting in AngularJS 2
AngularJS 2 tries to register exception handlers in global variable `global`. However it's already existed in NW.js environment, which prevents exception reporting shown in DevTools. The workaround is to rename `global` to something else before loading any AngularJS libraries. For example,

```html
<script>
window.nw_global = window.global;
window.global = undefined;
</script>
<!-- Angular 2 Dependencies -->
```

## How to leave fullscreen mode with ESC key?
Usually users expect to use ESC key to quit fullscreen mode. By default, NW.js don't bind ESC shortcut for leaving fullscreen mode, but providing APIs for entering and leaving fullscreen mode. It will give developers more control on fullscreen mode.

To enable ESC key for leaving fullscreen mode, you can use [Shortcut API](../References/Shortcut.md):

```javascript
nw.App.registerGlobalHotKey(new nw.Shortcut({
  key: "Esc",
  active: function () {
    // decide whether to leave fullscreen mode
    // then ...
    nw.Window.get().leaveFullscreen();
  })
}));
```

