# Introduction

`node-webkit` is a web runtime based on `Chromium` and `node.js`. Though its
name suggests that is's a module of `node.js`, it is in fact a standalone 
runtime that runs apps written in HTML, CSS and node.js.

# Features

* Write apps in modern HTML, CSS, JS and WebGL
* Strong network and native APIs from node.js
* Support modules written in JS and C++ 
* Easy to package and distribute apps

# Prebuilt binaries

_Binaries built for all platforms will come soon._

* Linux: 32bit / 64bit
* Windows: win32
* Mac: 32bit

# Quick Start

Create `index.html`:

````html
<html>
<head>
<title>Hello World!</title>
</head>
<body>
<h1>Hello World!</h1>
We are using node.js <script>document.write(process.version)</script>
</body>
</html>
````

Create `package.json`:

````json
{
  "main": "index.html"
}
````

Compress `index.html` and `package.json` into a zip archive, and rename
it to `app.nw`:

    app.nw
    |-- package.json
    `-- index.html

Download the prebuilt binary for your platform and use it to open the
`app.nw` file:

````bash
$ ./nw app.nw
````

Note: on Windows, you can drag the `app.nw` to `nw.exe` to open it.

For more information on how to write/package/run apps, see:

* [How to run apps](node-webkit/wiki/How-to-run-apps)
* [How to package and distribute your apps](node-webkit/wiki/How-to-package-and-distribute-your-apps)
* [Manifest format](node-webkit/wiki/Manifest-format) - format of `package.json`
* [Node.js Manual](http://nodejs.org/api/)

# For developers

* [Building node-webkit](node-webkit/wiki/Building-node-webkit)
* [How node.js is integrated with Chromium](node-webkit/wiki/How-node.js-is-integrated-with-Chromium)

# Community

We use [node-webkit | Google Groups](groups.google.com/group/node-webkit) as
our mailing list, subscribe via [node-webkit+subscribe@googlegroups.com](mailto:node-webkit+subscribe@googlegroups.com)

# Some history

The first release of `node-webkit` is a C++ module of `node.js`, it created
a WebView as the main window, and modified the WebView's WebKit engine to
allow `node.js` code to execute in normal web pages. See the `master` branch
of [rogerwang/node-webkit](https://github.com/rogerwang/node-webkit).

Later `node-webkit` instead hacked on `Chromium` and `CEF`, and showed pages
using `CEF`'s `cefclient`. These works were done by
[rogerwang](https://github.com/rogerwang), see the `cef` branch
of [rogerwang/node-webkit](https://github.com/rogerwang/node-webkit/tree/cef).

Since then `node-webkit` had slept for a while, now I have taken the
development of `node-webkit` and will try to make it a usable web runtime.

# License

Currently most of our codes are based on `Chromium` and `CEF`, and the code
of `nw` resides under `CEF`. In future we will move `nw` out of `CEF` into
this repository, before that all our code obeys `Chromium` and `CEF`'s 
BSD-like license, for more see `LICENSE.txt` under `src/cef`.
