# Build Flavors {: .doctitle}
---

[TOC]

NW.js supports various build flavors for reducing the application size. Currently NW.js supports following build flavors:

* SDK flavor: has builtin support for DevTools and NaCl plugins. SDK flavor has the same capabilities as the builds before 0.13.0
* Normal flavor: is a minimum build without DevTools and NaCl plugin support.

In your code, you can use `process.versions['nw-flavor']` to see in which flavor your application is running.

See [Build Flavors section in Building NW.js](../../For Developers/Building NW.js.md#build-flavors) for how to build them from source code.
