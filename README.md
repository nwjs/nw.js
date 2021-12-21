## node-webkit is renamed NW.js

[![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/nwjs/nw.js?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)  
Official site: https://nwjs.io  
## Introduction

NW.js is an app runtime based on `Chromium` and `node.js`. You can 
write native apps in HTML and JavaScript with NW.js. It also lets you
call Node.js modules directly from the DOM and enables a new way of writing
native applications with all Web technologies.

It was created in the Intel Open Source Technology Center.

[Building a Cross-platform Desktop App with NW.js](https://www.sitepoint.com/cross-platform-desktop-app-nw-js/)     
[Creating Desktop Applications With node-webkit](https://strongloop.com/strongblog/creating-desktop-applications-with-node-webkit/)     
[WebApp to DesktopApp with node-webkit (slides)](http://oldgeeksguide.github.io/presentations/html5devconf2013/wtod.html)  
[Essay on the history and internals of the project](http://yedingding.com/2014/08/01/node-webkit-intro-en.html)

## Features

* Apps written in modern HTML5, CSS3, JS and WebGL.
* Complete support for [Node.js APIs](https://nodejs.org/api/) and all its [third party modules](https://www.npmjs.com/).
* Good performance: Node and WebKit run in the same thread: Function calls are made straightforward; objects are in the same heap and can just reference each other.
* Easy to package and distribute apps.
* Available on Linux, Mac OS X and Windows.

## Downloads
* **v0.59.1:** (Dec 2nd, 2021, based off of Node.js v17.3.0, Chromium 96.0.4664.110) : [release notes](https://nwjs.io/blog/v0.59.1/)  
 **NOTE** You might want the **SDK build**. Please read the release notes.
 * Linux: [32bit](https://dl.nwjs.io/v0.59.1/nwjs-v0.59.1-linux-ia32.tar.gz) / [64bit](https://dl.nwjs.io/v0.59.1/nwjs-v0.59.1-linux-x64.tar.gz)
 * Windows: [32bit](https://dl.nwjs.io/v0.59.1/nwjs-v0.59.1-win-ia32.zip) / [64bit](https://dl.nwjs.io/v0.59.1/nwjs-v0.59.1-win-x64.zip)
 * Mac 10.10+: [64bit](https://dl.nwjs.io/v0.59.1/nwjs-v0.59.1-osx-x64.zip)
 * Use [Legacy build](http://nwjs.io/downloads/) for Win XP and early OSX.

* **latest nightly build from git tip**: https://dl.nwjs.io/live-build/

* [Previous versions](https://dl.nwjs.io); See [the mapping file](https://nwjs.io/versions.json) for the version info in previous releases.

### Demos and real apps
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
Official documentation: http://docs.nwjs.io/  

For more information on how to write/package/run apps, see:

* [How to run apps](https://github.com/nwjs/nw.js/wiki/How-to-run-apps)
* [How to package and distribute your apps](https://github.com/nwjs/nw.js/wiki/How-to-package-and-distribute-your-apps)
* [How to use Node.js modules in node-webkit](https://github.com/nwjs/nw.js/wiki/Using-Node-modules)

And our [Wiki](https://github.com/nwjs/nw.js/wiki) for much more.

## Community

We use the [google group](https://groups.google.com/d/forum/nwjs-general) as
our mailing list (use English only). Subscribe via [nwjs-general+subscribe@googlegroups.com](mailto:nwjs-general+subscribe@googlegroups.com).

*NOTE*: Links to the old google group (e.g. `https://groups.google.com/forum/#!msg/node-webkit/doRWZ07LgWQ/4fheV8FF8zsJ`) that are no longer working can be fixed by replacing `node-webkit` with `nwjs-general` (e.g `https://groups.google.com/forum/#!msg/nwjs-general/doRWZ07LgWQ/4fheV8FF8zsJ`).

Issues are being tracked here on GitHub.

The source code for NW.js and the daily development spans across multiple repositories in this organization. This repository is for the purpose of issue tracking, landing page and part of the source code.

### Verifying Binaries

Starting from 0.32.0 the stable and nightly download directories contain a SHASUMS256.txt
file that lists the SHA checksums for each file available for download, as well as the
checksums for the files inside the download package.

The SHASUMS256.txt can be downloaded using `curl`.

```console
$ curl -O https://dl.nwjs.io/vx.y.z/SHASUMS256.txt
```

To check that a downloaded file matches the checksum, run
it through `sha256sum` with a command such as:

```console
$ grep nwjs-vx.y.z.tar.gz SHASUMS256.txt | sha256sum -c -
```

The stable releases (but not Nightlies) also have the GPG detached
signature of SHASUMS256.txt available as SHASUMS256.txt.asc. You can use `gpg`
to verify that SHASUMS256.txt has not been tampered with.

To verify SHASUMS256.txt has not been altered, you will first need to import
the GPG key of NW.js maintainer to create releases.
Use this command to import the key:

```console
$ gpg --keyserver pool.sks-keyservers.net --recv-keys 78680FA9E21BB40A
```
```
(Key fingerprint is 1E8B EE8D 5B0C 4CBC D6D1  9E26 7868 0FA9 E21B B40A)
```

Next, download the SHASUMS256.txt.asc for the release:

```console
$ curl -O https://dl.nwjs.io/vx.y.z/SHASUMS256.txt.asc
```

After downloading the appropriate SHASUMS256.txt and SHASUMS256.txt.asc files,
you can then use `gpg --verify SHASUMS256.txt.asc SHASUMS256.txt` to verify
that the file has been signed by an authorized member of the NW.js team.

Once verified, use the SHASUMS256.txt file to get the checksum for
the binary verification command above.

## License

`NW.js`'s code in this repo uses the MIT license, see our `LICENSE` file. To redistribute the binary, see [How to package and distribute your apps](https://github.com/nwjs/nw.js/wiki/How-to-package-and-distribute-your-apps)

