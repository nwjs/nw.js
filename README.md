# Introduction

node-webkit is an app runtime based on `Chromium` and `node.js`. You can 
write native apps in HTML and Javascript with node-webkit. It also lets you
to call Node.js modules directly from DOM and enables a new way of writing
native applications with all Web technologies.

It's created and developed in Intel Open Source Technology Center.

[Introduction to node-webkit (slides)](https://speakerdeck.com/u/zcbenz/p/node-webkit-app-runtime-based-on-chromium-and-node-dot-js).

# Features

* Apps written in modern HTML5, CSS3, JS and WebGL.
* Complete support for [Node.js APIs](http://nodejs.org/api/) and all its [third party modules](https://npmjs.org).
* No extra performance lose in calling Node.js APIs.
* Easy to package and distribute apps.

# Downloads

[v0.3.2 release note](https://groups.google.com/forum/?fromgroups=#!topic/node-webkit/8JktpVPZTNs)

Prebuilt binaries (v0.3.2):

* Linux: [32bit](http://s3.amazonaws.com/node-webkit/nw_release_linux_x32.tar.gz) / [64bit](https://s3.amazonaws.com/node-webkit/nw_release_linux_x64.tar.gz)
* Windows: [win32](http://s3.amazonaws.com/node-webkit/nw_release_win32.zip)
* Mac: [32bit](http://s3.amazonaws.com/node-webkit/nw_release_mac.zip)

[Looking for older versions?](https://github.com/rogerwang/node-webkit/wiki/Downloads-of-old-versions)

You may also be interested in [our demos repository](https://github.com/zcbenz/nw-sample-apps) and the [List of apps and companies using node-webkit](https://github.com/rogerwang/node-webkit/wiki/List-of-apps-and-companies-using-node-webkit).

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

* [How to run apps](https://github.com/rogerwang/node-webkit/wiki/How-to-run-apps)
* [How to package and distribute your apps](https://github.com/rogerwang/node-webkit/wiki/How-to-package-and-distribute-your-apps)

And our [Wiki](https://github.com/rogerwang/node-webkit/wiki) for much more.

# Community

We use [node-webkit group](http://groups.google.com/group/node-webkit) as
our mailing list, subscribe via [node-webkit+subscribe@googlegroups.com](mailto:node-webkit+subscribe@googlegroups.com).

# License

`node-webkit`'s code uses the MIT license, see our `LICENSE` file.
