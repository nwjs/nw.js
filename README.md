## Introduction

node-webkit is an app runtime based on `Chromium` and `node.js`. You can 
write native apps in HTML and Javascript with node-webkit. It also lets you
to call Node.js modules directly from DOM and enables a new way of writing
native applications with all Web technologies.

It's created and developed in Intel Open Source Technology Center.

[Introduction to node-webkit (slides)](https://speakerdeck.com/u/zcbenz/p/node-webkit-app-runtime-based-on-chromium-and-node-dot-js).

## Features

* Apps written in modern HTML5, CSS3, JS and WebGL.
* Complete support for [Node.js APIs](http://nodejs.org/api/) and all its [third party modules](https://npmjs.org).
* Good performance: Node and WebKit runs in the same thread: Function calls are made straightforward; objects are in the same heap and can just reference each other;
* Easy to package and distribute apps.
* Available on Linux, Mac OSX and Windows

## Downloads

[v0.4.1 release note](https://groups.google.com/d/msg/node-webkit/1JgTNLOEpIs/e2e0FwH9TE4J)

Prebuilt binaries (v0.4.1 — Feb 1, 2013):

* Linux: [32bit](https://s3.amazonaws.com/node-webkit/v0.4.1/node-webkit-v0.4.1-linux-ia32.tar.gz) / [64bit](https://s3.amazonaws.com/node-webkit/v0.4.1/node-webkit-v0.4.1-linux-x64.tar.gz)
* Windows: [win32](https://s3.amazonaws.com/node-webkit/v0.4.1/node-webkit-v0.4.1-win-ia32.zip)
* Mac: [32bit, 10.7+](https://s3.amazonaws.com/node-webkit/v0.4.1/node-webkit-v0.4.1-osx-ia32.zip)

[Looking for older versions?](https://github.com/rogerwang/node-webkit/wiki/Downloads-of-old-versions)

###Demos and real apps
You may also be interested in [our demos repository](https://github.com/zcbenz/nw-sample-apps) and the [List of apps and companies using node-webkit](https://github.com/rogerwang/node-webkit/wiki/List-of-apps-and-companies-using-node-webkit).

## Quick Start

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

## Documents

For more information on how to write/package/run apps, see:

* [How to run apps](https://github.com/rogerwang/node-webkit/wiki/How-to-run-apps)
* [How to package and distribute your apps](https://github.com/rogerwang/node-webkit/wiki/How-to-package-and-distribute-your-apps)
* [How to use 3rd party node.js modules in node-webkit](https://github.com/rogerwang/node-webkit/wiki/How-to-use-3rd-party-node.js-modules-in-node-webkit)

And our [Wiki](https://github.com/rogerwang/node-webkit/wiki) for much more.

## Community

We use [node-webkit group](http://groups.google.com/group/node-webkit) as
our mailing list, subscribe via [node-webkit+subscribe@googlegroups.com](mailto:node-webkit+subscribe@googlegroups.com).

## License

`node-webkit`'s code uses the MIT license, see our `LICENSE` file.
