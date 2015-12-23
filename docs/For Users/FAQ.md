# FAQ {: .doctitle}
---

[TOC]

## 1. `var crypto = require('crypto')` gets a wrong object
Chromium has its own global `crypto` object which can't be overwritten. So you can't use the same variable name `crypto`. Changing your variable name to something else, like `nodeCrypto`, will work.


