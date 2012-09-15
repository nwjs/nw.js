# Introduction

`node-webkit` is a web runtime based on `Chromium` and `node.js`. It lets you
to call Node.js modules directly from DOM and enables a new way of writing
native applications with all Web technologies. 

It's created and developed in Intel Open Source Technology Center.

[Introduction to node-webkit (slides)](https://speakerdeck.com/u/zcbenz/p/node-webkit-app-runtime-based-on-chromium-and-node-dot-js)

# Features

* Write apps in modern HTML, CSS, JS and WebGL
* Strong network and native APIs from node.js
* Support modules written in JS and C++ 
* Node and WebKit is in same thread so that the function call is very efficient.
* Easy to package and distribute apps

# Downloads

[v0.2.5 release note](https://groups.google.com/forum/?fromgroups=#!topic/node-webkit/PExOCHgcmKs)

Prebuilt binaries (v0.2.5):

* Linux: [32bit](http://s3.amazonaws.com/node-webkit/nw_release_linux_x32.tar.gz) / [64bit](https://s3.amazonaws.com/node-webkit/nw_release_linux_x64.tar.gz)
* Windows: [win32](http://s3.amazonaws.com/node-webkit/nw_release_win32.zip)
* Mac: [32bit](http://s3.amazonaws.com/node-webkit/nw_release_mac.zip)

Demos:

* [Hello World](http://s3.amazonaws.com/node-webkit/hello_world2.nw)
* [File Explorer](https://github.com/zcbenz/nw-file-explorer)

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
  "name": "nw-demo",
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

* [Node.js Manual](http://nodejs.org/api/)
* [How to run apps](https://github.com/rogerwang/node-webkit/wiki/How-to-run-apps)
* [How to package and distribute your apps](https://github.com/rogerwang/node-webkit/wiki/How-to-package-and-distribute-your-apps)

And also our [Wiki](https://github.com/rogerwang/node-webkit/wiki).

# For developers

* [Building node-webkit](https://github.com/rogerwang/node-webkit/wiki/Building-node-webkit)
* [How node.js is integrated with Chromium](https://github.com/rogerwang/node-webkit/wiki/How-node.js-is-integrated-with-Chromium)

# Community

We use [node-webkit | Google Groups](http://groups.google.com/group/node-webkit) as
our mailing list, subscribe via [node-webkit+subscribe@googlegroups.com](mailto:node-webkit+subscribe@googlegroups.com).

# Old version based on WebKit and GTK+

`node-webkit` previously was a `node.js` module which created a GTK+ WebView
based on a custom WebKit, see tag [webkitgtk](https://github.com/rogerwang/node-webkit/tree/webkitgtk).

# License

`node-webkit`'s code uses the MIT license, `Chromium` and `CEF`'s codes use
their BSD-style licenses.

