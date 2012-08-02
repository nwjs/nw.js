# Introduction

`node-webkit` is a web runtime based on `Chromium` and `node.js`. Though its
name suggests that is's a module of `node.js`, it is in fact a standalone 
runtime that runs your app written in HTML and node.js. `node.js` is
a part of it that provides native APIs and module system.

# Features

* Write apps in modern HTML, CSS, JS and WebGL
* Strong network and native APIs from node.js
* Support modules written in JS and C++ 
* Easy packaging and distributing apps

# Downloads

Binaries built for all platforms will come soon.

# Quick Start

Make a folder `app`, and save file `index.html` under `app`:

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

Save file `package.json` under `app`:

````json
{
  "main": "index.html"
}
````

Compress all files under `app` (`package.json` should be under root) into
a zip archive, and then rename it to `app.nw`:

    app.nw
    |-- package.json
    `-- index.html

Download our `nw` binary for your platform and then use `nw` to open the
`app.nw` file:

````bash
$ ./nw app.nw
````

Note: on Windows, you can drag the `app.nw` to `nw.exe` to open it.

For more information on how to write/package/run apps, see:

* [How to run apps](wiki/How-to-run-apps)
* [How to package and distribute your apps](wiki/How-to-package-and-distribute-your-apps)
* [Manifest format](wiki/Manifest-format)
* [Node.js Manual](http://nodejs.org/api/)

# For developers

* [Building node-webkit](wiki/Building-node-webkit)
* [How node.js is integrated with Chromium](wiki/How-node.js-is-integrated-with-Chromium)

