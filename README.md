## Introduction

node-webkit is an app runtime based on `Chromium` and `node.js`. You can 
write native apps in HTML and Javascript with node-webkit. It also lets you
call Node.js modules directly from the DOM and enables a new way of writing
native applications with all Web technologies.

It's created and developed in the Intel Open Source Technology Center.

[Introduction to node-webkit (slides)](https://speakerdeck.com/u/zcbenz/p/node-webkit-app-runtime-based-on-chromium-and-node-dot-js).

## Features

* Apps written in modern HTML5, CSS3, JS and WebGL.
* Complete support for [Node.js APIs](http://nodejs.org/api/) and all its [third party modules](https://npmjs.org).
* Good performance: Node and WebKit runs in the same thread: Function calls are made straightforward; objects are in the same heap and can just reference each other;
* Easy to package and distribute apps.
* Available on Linux, Mac OSX and Windows

## Downloads
[v0.7.3 release notes](https://groups.google.com/d/msg/node-webkit/QKLiSHSwRFw/kNq3ZQZmFYUJ)

Prebuilt binaries (v0.7.3 - Sep 12, 2013):

* Linux: [32bit](https://s3.amazonaws.com/node-webkit/v0.7.3/node-webkit-v0.7.3-linux-ia32.tar.gz) / [64bit] (https://s3.amazonaws.com/node-webkit/v0.7.3/node-webkit-v0.7.3-linux-x64.tar.gz)
* Windows: [win32](https://s3.amazonaws.com/node-webkit/v0.7.3/node-webkit-v0.7.3-win-ia32.zip)
* Mac: [32bit, 10.7+](https://s3.amazonaws.com/node-webkit/v0.7.3/node-webkit-v0.7.3-osx-ia32.zip)

[Looking for older versions?](https://github.com/rogerwang/node-webkit/wiki/Downloads-of-old-versions)

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

Compress `index.html` and `package.json` into a zip archive called `app.nw`:

````bash
$ zip app.nw index.html package.json
````

This should create a structure like this:

```
app.nw
|-- package.json
`-- index.html
```

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
* [How to use Node.js modules in node-webkit](https://github.com/rogerwang/node-webkit/wiki/Using-Node-modules)

And our [Wiki](https://github.com/rogerwang/node-webkit/wiki) for much more.

## Community

We use the [node-webkit group](http://groups.google.com/group/node-webkit) as
our mailing list. Subscribe via [node-webkit+subscribe@googlegroups.com](mailto:node-webkit+subscribe@googlegroups.com).
Issues are being tracked here on GitHub.

## License

`node-webkit`'s code in this repo uses the MIT license, see our `LICENSE` file. To redistribute the binary, see [How to package and distribute your apps](https://github.com/rogerwang/node-webkit/wiki/How-to-package-and-distribute-your-apps)
