## Introduction

node-webkit is an app runtime based on `Chromium` and `node.js`. You can 
write native apps in HTML and JavaScript with node-webkit. It also lets you
call Node.js modules directly from the DOM and enables a new way of writing
native applications with all Web technologies.

It's created and developed in the Intel Open Source Technology Center.

[Introduction to node-webkit (slides)](https://speakerdeck.com/u/zcbenz/p/node-webkit-app-runtime-based-on-chromium-and-node-dot-js)   
[Creating Desktop Applications With node-webkit](http://strongloop.com/strongblog/creating-desktop-applications-with-node-webkit/)     
[WebApp to DesktopApp with node-webkit (slides)](http://oldgeeksguide.github.io/presentations/html5devconf2013/wtod.html)  
[Essay on the history and internals of the project](http://yedingding.com/2014/08/01/node-webkit-intro-en.html)

## Features

* Apps written in modern HTML5, CSS3, JS and WebGL.
* Complete support for [Node.js APIs](http://nodejs.org/api/) and all its [third party modules](https://npmjs.org).
* Good performance: Node and WebKit runs in the same thread: Function calls are made straightforward; objects are in the same heap and can just reference each other;
* Easy to package and distribute apps.
* Available on Linux, Mac OS X and Windows

## Downloads
* **v0.10.5:** (Sep 16, 2014, based off of Node v0.11.13, Chromium 35.0.1916.157): [release notes](https://groups.google.com/d/msg/node-webkit/l2PsW-0G5Oc/Fx19-UrS3ZoJ)

 * Linux: [32bit](http://dl.node-webkit.org/v0.10.5/node-webkit-v0.10.5-linux-ia32.tar.gz) / [64bit](http://dl.node-webkit.org/v0.10.5/node-webkit-v0.10.5-linux-x64.tar.gz)
 * Windows: [win32](http://dl.node-webkit.org/v0.10.5/node-webkit-v0.10.5-win-ia32.zip)
 * Mac 10.7+: [32bit](http://dl.node-webkit.org/v0.10.5/node-webkit-v0.10.5-osx-ia32.zip) / [64bit](http://dl.node-webkit.org/v0.10.5/node-webkit-v0.10.5-osx-x64.zip)

* **0.8.6:** (Apr 18, 2014, based off of Node v0.10.22, Chrome 30.0.1599.66) **If your native Node module works only with Node v0.10, then you should use node-webkit v0.8.x, which is also a maintained branch. [More info](https://groups.google.com/d/msg/node-webkit/2OJ1cEMPLlA/09BvpTagSA0J)**  
[release notes](https://groups.google.com/d/msg/node-webkit/CLPkgfV-i7s/hwkkQuJ1kngJ)

 * Linux: [32bit](http://dl.node-webkit.org/v0.8.6/node-webkit-v0.8.6-linux-ia32.tar.gz) / [64bit](http://dl.node-webkit.org/v0.8.6/node-webkit-v0.8.6-linux-x64.tar.gz)
 * Windows: [win32](http://dl.node-webkit.org/v0.8.6/node-webkit-v0.8.6-win-ia32.zip)
 * Mac: [32bit, 10.7+](http://dl.node-webkit.org/v0.8.6/node-webkit-v0.8.6-osx-ia32.zip)

* **latest live build**: git tip version; build triggered from every git commit: http://dl.node-webkit.org/live-build/

* [Previous versions](https://github.com/rogerwang/node-webkit/wiki/Downloads-of-old-versions)

###Demos and real apps
You may also be interested in [our demos repository](https://github.com/zcbenz/nw-sample-apps) and the [List of apps and companies using node-webkit](https://github.com/rogerwang/node-webkit/wiki/List-of-apps-and-companies-using-node-webkit).

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
  "main": "index.html"
}
```

Run:  
```bash
$ /path/to/nw .  (suppose the current directory contains 'package.json')
```

Note: on Windows, you can drag the folder containing `package.json` to `nw.exe` to open it.

## Documents

For more information on how to write/package/run apps, see:

* [How to run apps](https://github.com/rogerwang/node-webkit/wiki/How-to-run-apps)
* [How to package and distribute your apps](https://github.com/rogerwang/node-webkit/wiki/How-to-package-and-distribute-your-apps)
* [How to use Node.js modules in node-webkit](https://github.com/rogerwang/node-webkit/wiki/Using-Node-modules)

And our [Wiki](https://github.com/rogerwang/node-webkit/wiki) for much more.

## Community

We use the [node-webkit group](http://groups.google.com/group/node-webkit) as
our mailing list (use English only). Subscribe via [node-webkit+subscribe@googlegroups.com](mailto:node-webkit+subscribe@googlegroups.com).
Issues are being tracked here on GitHub.

You can chat with us on IRC in the ##node-webkit channel on irc.freenode.net  

## License

`node-webkit`'s code in this repo uses the MIT license, see our `LICENSE` file. To redistribute the binary, see [How to package and distribute your apps](https://github.com/rogerwang/node-webkit/wiki/How-to-package-and-distribute-your-apps)

## Sponsors

The work is being sponsored by:  
* [Intel](http://www.intel.com)
* [Gnor Tech](http://gnor.net)
* [eFounders](http://efounders.co)
