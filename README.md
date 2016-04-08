## node-webkit is renamed NW.js

[![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/nwjs/nw.js?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)  
Official site: http://nwjs.io  
Official documentation: http://docs.nwjs.io/  
[Announcement](https://groups.google.com/d/msg/nwjs-general/V1FhvfaFIzQ/720xKVd0jNkJ)  
## Introduction

NW.js is an app runtime based on `Chromium` and `node.js`. You can 
write native apps in HTML and JavaScript with NW.js. It also lets you
call Node.js modules directly from the DOM and enables a new way of writing
native applications with all Web technologies.

It was created in the Intel Open Source Technology Center.

[Introduction to node-webkit (slides)](https://speakerdeck.com/zcbenz/node-webkit-app-runtime-based-on-chromium-and-node-dot-js)   
[Creating Desktop Applications With node-webkit](https://strongloop.com/strongblog/creating-desktop-applications-with-node-webkit/)     
[WebApp to DesktopApp with node-webkit (slides)](http://oldgeeksguide.github.io/presentations/html5devconf2013/wtod.html)  
[Essay on the history and internals of the project](http://yedingding.com/2014/08/01/node-webkit-intro-en.html)

## Features

* Apps written in modern HTML5, CSS3, JS and WebGL.
* Complete support for [Node.js APIs](https://nodejs.org/api/) and all its [third party modules](https://www.npmjs.com/).
* Good performance: Node and WebKit run in the same thread: Function calls are made straightforward; objects are in the same heap and can just reference each other;
* Easy to package and distribute apps.
* Available on Linux, Mac OS X and Windows

## Downloads
* **v0.13.4:** (Apr 8, 2016, based off of Node.js v5.10.0, Chromium 49.0.2623.112): [release notes](http://nwjs.io/blog/v0.13.4/)  
 **NOTE** You might want the **SDK build**. Please read the release notes  
 * Linux: [32bit](http://dl.nwjs.io/v0.13.4/nwjs-v0.13.4-linux-ia32.tar.gz) / [64bit](http://dl.nwjs.io/v0.13.4/nwjs-v0.13.4-linux-x64.tar.gz)
 * Windows: [32bit](http://dl.nwjs.io/v0.13.4/nwjs-v0.13.4-win-ia32.zip) / [64bit](http://dl.nwjs.io/v0.13.4/nwjs-v0.13.4-win-x64.zip)
 * Mac 10.7+: [64bit](http://dl.nwjs.io/v0.13.4/nwjs-v0.13.4-osx-x64.zip)

* **v0.14.0-rc3:** (Apr 7, 2016, based off of Node.js v5.10.0, Chromium 50.0.2661.66): [release notes](http://nwjs.io/blog/v0.14.0-rc3/)  
 **NOTE** You might want the **SDK build**. Please read the release notes  
 * Linux: [32bit](http://dl.nwjs.io/v0.14.0-rc3/nwjs-v0.14.0-rc3-linux-ia32.tar.gz) / [64bit](http://dl.nwjs.io/v0.14.0-rc3/nwjs-v0.14.0-rc3-linux-x64.tar.gz)
 * Windows: [32bit](http://dl.nwjs.io/v0.14.0-rc3/nwjs-v0.14.0-rc3-win-ia32.zip) / [64bit](http://dl.nwjs.io/v0.14.0-rc3/nwjs-v0.14.0-rc3-win-x64.zip)
 * Mac 10.7+: [64bit](http://dl.nwjs.io/v0.14.0-rc3/nwjs-v0.14.0-rc3-osx-x64.zip)

* **latest live build**: git tip version; build triggered from every git commit: http://dl.nwjs.io/live-build/

* [Previous versions](https://github.com/nwjs/nw.js/wiki/Downloads-of-old-versions)

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

* [How to run apps](https://github.com/nwjs/nw.js/wiki/How-to-run-apps)
* [How to package and distribute your apps](https://github.com/nwjs/nw.js/wiki/How-to-package-and-distribute-your-apps)
* [How to use Node.js modules in node-webkit](https://github.com/nwjs/nw.js/wiki/Using-Node-modules)

And our [Wiki](https://github.com/nwjs/nw.js/wiki) for much more.

## Community

We use the [google group](https://groups.google.com/d/forum/nwjs-general) as
our mailing list (use English only). Subscribe via [nwjs-general+subscribe@googlegroups.com](mailto:nwjs-general+subscribe@googlegroups.com).

*NOTE*: Links to the old google group (e.g. `https://groups.google.com/forum/#!msg/node-webkit/doRWZ07LgWQ/4fheV8FF8zsJ`) that are no more working can be fixed by replacing `node-webkit` with `nwjs-general` (e.g `https://groups.google.com/forum/#!msg/nwjs-general/doRWZ07LgWQ/4fheV8FF8zsJ`).

Issues are being tracked here on GitHub.

## License

`node-webkit`'s code in this repo uses the MIT license, see our `LICENSE` file. To redistribute the binary, see [How to package and distribute your apps](https://github.com/nwjs/nw.js/wiki/How-to-package-and-distribute-your-apps)

## Sponsors

The work is being sponsored by:  
* [Intel](http://www.intel.com/content/www/us/en/homepage.html)
* [Gnor Tech](http://gnor.net)
