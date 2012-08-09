# Introduction

`node-webkit` is a web runtime based on `Chromium` and `node.js`. Though its
name suggests that is's a module of `node.js`, it is in fact a standalone 
runtime that runs apps written in HTML, CSS and node.js.

# Features

* Write apps in modern HTML, CSS, JS and WebGL
* Strong network and native APIs from node.js
* Support modules written in JS and C++ 
* Easy to package and distribute apps

# Downloads

Prebuilt binaries:

* Linux: [32bit](http://s3.amazonaws.com/node-webkit/nw_release_linux_x32.tar.gz) / [64bit](https://s3.amazonaws.com/node-webkit/nw_release_linux_x64.tar.gz)
* Windows: [win32](http://s3.amazonaws.com/node-webkit/nw_release_win32.zip)
* Mac: [32bit](http://s3.amazonaws.com/node-webkit/nw_release_mac.zip)

Demos:

* [Hello World](http://s3.amazonaws.com/node-webkit/hello_world.nw)
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

* [How to run apps](https://github.com/rogerwang/node-webkit/wiki/How-to-run-apps)
* [How to package and distribute your apps](https://github.com/rogerwang/node-webkit/wiki/How-to-package-and-distribute-your-apps)
* [Manifest format](https://github.com/rogerwang/node-webkit/wiki/Manifest-format) - format of `package.json`
* [Node.js Manual](http://nodejs.org/api/)

# For developers

* [Building node-webkit](https://github.com/rogerwang/node-webkit/wiki/Building-node-webkit)
* [How node.js is integrated with Chromium](https://github.com/rogerwang/node-webkit/wiki/How-node.js-is-integrated-with-Chromium)

# Community

We use [node-webkit | Google Groups](groups.google.com/group/node-webkit) as
our mailing list, subscribe via [node-webkit+subscribe@googlegroups.com](mailto:node-webkit+subscribe@googlegroups.com).

# License

`node-webkit`'s code uses the MIT license, `Chromium` and `CEF`'s codes use
their BSD-style licenses.

