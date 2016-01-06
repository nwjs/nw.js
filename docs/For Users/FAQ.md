# FAQ {: .doctitle}
---

[TOC]

## 1. `var crypto = require('crypto')` gets a wrong object
Chromium has its own global `crypto` object which can't be overwritten. So you can't use the same variable name `crypto`. Changing your variable name to something else, like `nodeCrypto`, will work.

## 2. Images are broken in AnugarJS and receive `Failed to load resource XXX net::ERR_UNKNOWN_URL_SCHEME` in DevTools
AngularJS added `unsafe:` prefix for unknown scheme to prevent XSS attack. URLs in NW.js and Chrome apps are started with `chrome-extension:` scheme, which is unknown to AnuglarJS. The solution is to config the whitelist of known schemes with AngularJS by adding following lines:

```javascript
myApp.config(['$compileProvider',
  function($compileProvider) {
    $compileProvider.imgSrcSanitizationWhitelist(/^\s*((https?|ftp|file|blob|chrome-extension):|data:image\/)/);
    $compileProvider.aHrefSanitizationWhitelist(/^\s*(https?|ftp|mailto|tel|file:chrome-extension):/);
  }]);
```


