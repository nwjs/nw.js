# Use Native Node Modules {: .doctitle}

[TOC]

## Install with NPM (v0.111.3 and later)

NW.js provides standard Node.js-compatible headers tarballs, so native modules can be built with upstream `node-gyp` directly. No `nw-gyp` or custom environment variables are required.

```bash
npm rebuild --target=0.111.3 --disturl=https://dl.nwjs.io/
```

Or during `npm install`:

```bash
npm install --target=0.111.3 --disturl=https://dl.nwjs.io/
```

`node-gyp` will automatically download the headers tarball from the NW.js download server.

!!! warning "Note for Windows"
    You need a compiler toolchain installed (Visual Studio with "Desktop development with C++" workload).

## Install with NPM (before v0.111.3)

!!! warning "Use Same Version and Architecture of Node.js and NW.js"
    Following instructions only works if you are using the same version and architecture of Node.js and NW.js.

### For LTS Releases

If you are using LTS release, native modules installed by `npm` can be supported after hacking as below:

* On Linux and Mac, no hacking is required.
* On Windows, you need to replace the file
`<npm-path>\node_modules\node-gyp\src\win_delay_load_hook.cc` with the one at https://github.com/nwjs/nw.js/blob/nw18/tools/win_delay_load_hook.cc before installing modules with node-gyp or npm.

### For non-LTS Releases

!!! warning "Use Node.js v14.21.3"
    The following steps were tested with Node.js v14.21.3.

If you are using non-LTS release, `nw-gyp` is required to be pre-installed globally to build modules due to ABI differences in V8. These instructions below works for LTS releases as well.

To install native modules for NW.js, run following commands in your terminal:

```bash
# Install nw-gyp globally
npm install -g nw-gyp
# Setup target NW.js version
export npm_config_target=0.18.5
# Setup build architecture, ia32 or x64
export npm_config_arch=x64
export npm_config_target_arch=x64
# Setup env for modules built with node-pre-gyp
export npm_config_runtime=node-webkit
export npm_config_build_from_source=true
# Setup nw-gyp as node-gyp
export npm_config_node_gyp=$(which nw-gyp)
# Run npm install
npm install
```

!!! warning "Note for Windows"
    You will need to have a compiler installed. You can get one by installing the [Visual C++ Build Tools](http://landinghub.visualstudio.com/visual-cpp-build-tools). You will also need Python 2.7 (version 3.x is not supported).
    The `npm_config_node_gyp` environment variable has to be set to `path\to\global\node_modules\nw-gyp\bin\nw-gyp.js` instead of the `nw-gyp.cmd` batch script. This is a [bug of npm](https://github.com/npm/npm/issues/14543).
    You must use `set` instead of `export` to set environment variables in Windows. Here is a full example (Windows 10, Visual C++ Build Tool v2015):

```Batchfile
set PYTHON=C:\Python27\python.exe
set npm_config_target=0.21.6
set npm_config_arch=x64
set npm_config_runtime=node-webkit
set npm_config_build_from_source=true
set npm_config_node_gyp=C:\Users\xxxxxxxxx\AppData\Roaming\npm\node_modules\nw-gyp\bin\nw-gyp.js
npm install --msvs_version=2015
```

## Manually Rebuild

!!! tip "Full NPM Install is Recommended"
    After switching NW.js versions, delete `node_modules` and do a full `npm install` as described above.

### node-gyp (v0.111.3+)

Standard [`node-gyp`](https://github.com/nodejs/node-gyp) works directly with NW.js headers:

```bash
cd myaddon
node-gyp rebuild --target=0.111.3 --disturl=https://dl.nwjs.io/
```

### nw-gyp (before v0.111.3)

[`nw-gyp`](https://github.com/nwjs/nw-gyp) is a modified `node-gyp` to support NW.js specific headers and libraries.

```bash
npm install -g nw-gyp
cd myaddon
nw-gyp rebuild --target=0.13.0 --arch=x64
```

### node-pre-gyp

Some packages use [`node-pre-gyp`](https://github.com/mapbox/node-pre-gyp):

```bash
cd myaddon
node-pre-gyp build --runtime=node-webkit --target=0.111.3 --target_arch=x64
```
