## node-webkit is renamed NW.js

[![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/nwjs/nw.js?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)  
Official site: http://nwjs.io  
[Announcement](https://groups.google.com/d/msg/nwjs-general/V1FhvfaFIzQ/720xKVd0jNkJ)  
## Introduction

NW.js is an app runtime based on `Chromium` and `node.js`. You can 
write native apps in HTML and JavaScript with NW.js. It also lets you
call Node.js modules directly from the DOM and enables a new way of writing
native applications with all Web technologies.

It was created in the Intel Open Source Technology Center.

[Introduction to node-webkit (slides)](https://speakerdeck.com/u/zcbenz/p/node-webkit-app-runtime-based-on-chromium-and-node-dot-js)   
[Creating Desktop Applications With node-webkit](http://strongloop.com/strongblog/creating-desktop-applications-with-node-webkit/)     
[WebApp to DesktopApp with node-webkit (slides)](http://oldgeeksguide.github.io/presentations/html5devconf2013/wtod.html)  
[Essay on the history and internals of the project](http://yedingding.com/2014/08/01/node-webkit-intro-en.html)

## Features

* Apps written in modern HTML5, CSS3, JS and WebGL.
* Complete support for [Node.js APIs](http://nodejs.org/api/) and all its [third party modules](https://npmjs.org).
* Good performance: Node and WebKit run in the same thread: Function calls are made straightforward; objects are in the same heap and can just reference each other;
* Easy to package and distribute apps.
* Available on Linux, Mac OS X and Windows

## Downloads
* **v0.12.3:** (Jul 31, 2015, based off of IO.js v1.2.0, Chromium 41.0.2272.76): [release notes](https://groups.google.com/d/msg/nwjs-general/hhXCS4aXGV0/TUQmcu5XDwAJ)  
 * Linux: [32bit](http://dl.nwjs.io/v0.12.3/nwjs-v0.12.3-linux-ia32.tar.gz) / [64bit](http://dl.nwjs.io/v0.12.3/nwjs-v0.12.3-linux-x64.tar.gz)
 * Windows: [32bit](http://dl.nwjs.io/v0.12.3/nwjs-v0.12.3-win-ia32.zip) / [64bit](http://dl.nwjs.io/v0.12.3/nwjs-v0.12.3-win-x64.zip)
 * Mac 10.7+: [32bit](http://dl.nwjs.io/v0.12.3/nwjs-v0.12.3-osx-ia32.zip) / [64bit](http://dl.nwjs.io/v0.12.3/nwjs-v0.12.3-osx-x64.zip)

* **v0.13.0-alpha1:** (Jun 11, 2015, based off of IO.js v1.5.1, Chromium 43.0.2357.45): [release notes](https://groups.google.com/d/msg/nwjs-general/c25l_jGMqj8/rsAtdSQuxeUJ)  
 **NOTE** You might want the **SDK build**. Please read the release notes  
 * Linux: [32bit](http://dl.nwjs.io/v0.13.0/alpha1/nwjs-v0.13.0-alpha1-linux-ia32.tar.gz) / [64bit](http://dl.nwjs.io/v0.13.0/alpha1/nwjs-v0.13.0-alpha1-linux-x64.tar.gz)
 * Windows: [32bit](http://dl.nwjs.io/v0.13.0/alpha1/nwjs-v0.13.0-alpha1-win-ia32.zip) / [64bit](http://dl.nwjs.io/v0.13.0/alpha1/nwjs-v0.13.0-alpha1-win-x64.zip)
 * Mac 10.7+: [32bit](http://dl.nwjs.io/v0.13.0/alpha1/nwjs-v0.13.0-alpha1-osx-ia32.zip) / [64bit](http://dl.nwjs.io/v0.13.0/alpha1/nwjs-v0.13.0-alpha1-osx-x64.zip)

* **0.8.6:** (Apr 18, 2014, based off of Node v0.10.22, Chrome 30.0.1599.66) **If your native Node module works only with Node v0.10, then you should use node-webkit v0.8.x, which is also a maintained branch. [More info](https://groups.google.com/d/msg/nwjs-general/2OJ1cEMPLlA/09BvpTagSA0J)**  
[release notes](https://groups.google.com/d/msg/nwjs-general/CLPkgfV-i7s/hwkkQuJ1kngJ)

 * Linux: [32bit](http://dl.node-webkit.org/v0.8.6/node-webkit-v0.8.6-linux-ia32.tar.gz) / [64bit](http://dl.node-webkit.org/v0.8.6/node-webkit-v0.8.6-linux-x64.tar.gz)
 * Windows: [win32](http://dl.node-webkit.org/v0.8.6/node-webkit-v0.8.6-win-ia32.zip)
 * Mac: [32bit, 10.7+](http://dl.node-webkit.org/v0.8.6/node-webkit-v0.8.6-osx-ia32.zip)

* **latest live build**: git tip version; build triggered from every git commit: http://dl.nwjs.io/live-build/

* [Previous versions](https://github.com/rogerwang/node-webkit/wiki/Downloads-of-old-versions)

###Demos and real apps
You may also be interested in [our demos repository](https://github.com/zcbenz/nw-sample-apps) and the [List of apps and companies using nw.js](https://github.com/nwjs/nw.js/wiki/List-of-apps-and-companies-using-nw.js).

## Quick Start

Create `index.html`:

```html
<!DOCTYPE html>
<html>
  <head>
    <title>Hello World!</title>
  </head>
  <body>
    <h1>Hello World!</h1>
    We are using node.js <script>document.write(process.version)</script>.
  </body>
</html>
```

Create `package.json`:

```json
{
  "name": "nw-demo",
  "version": "0.0.1",
  "main": "index.html"
}
```

Run:  
```bash
$ /path/to/nw .  (suppose the current directory contains 'package.json')
```

Note: on Windows, you can drag the folder containing `package.json` to `nw.exe` to open it.

Note: on OSX, the executable binary is in a hidden directory within the .app file. To run node-webkit on OSX, type:  
`/path/to/nwjs.app/Contents/MacOS/nwjs .` *(suppose the current directory contains 'package.json')*   

## Documents

For more information on how to write/package/run apps, see:

* [How to run apps](https://github.com/rogerwang/node-webkit/wiki/How-to-run-apps)
* [How to package and distribute your apps](https://github.com/rogerwang/node-webkit/wiki/How-to-package-and-distribute-your-apps)
* [How to use Node.js modules in node-webkit](https://github.com/rogerwang/node-webkit/wiki/Using-Node-modules)

And our [Wiki](https://github.com/rogerwang/node-webkit/wiki) for much more.

## Community

We use the [google group](https://groups.google.com/d/forum/nwjs-general) as
our mailing list (use English only). Subscribe via [nwjs-general+subscribe@googlegroups.com](mailto:nwjs-general+subscribe@googlegroups.com).

*NOTE*: Links to the old google group (e.g. `https://groups.google.com/forum/#!msg/node-webkit/doRWZ07LgWQ/4fheV8FF8zsJ`) that are no more working can be fixed by replacing `node-webkit` with `nwjs-general` (e.g `https://groups.google.com/forum/#!msg/nwjs-general/doRWZ07LgWQ/4fheV8FF8zsJ`).

Issues are being tracked here on GitHub.

## License

`node-webkit`'s code in this repo uses the MIT license, see our `LICENSE` file. To redistribute the binary, see [How to package and distribute your apps](https://github.com/rogerwang/node-webkit/wiki/How-to-package-and-distribute-your-apps)

## Sponsors

The work is being sponsored by:  
* [Intel](http://www.intel.com)
* [Gnor Tech](http://gnor.net)
