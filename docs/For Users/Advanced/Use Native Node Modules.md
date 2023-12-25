# Use Native Node Modules {: .doctitle}
---

[TOC]

## Install with NPM

### For LTS Releases

!!! warning "Use Same Version and Architecture of Node.js and NW.js"
    Following instructions only works if you are using the same version and architecture of Node.js and NW.js.

If you are using LTS release, native modules installed by `npm` can be supported after hacking as below:

* On Linux and Mac, no hacking is required.
* On Windows, you need to replace the file
`<npm-path>\node_modules\node-gyp\src\win_delay_load_hook.cc` with the one at https://github.com/nwjs/nw.js/blob/nw18/tools/win_delay_load_hook.cc before installing modules with node-gyp or npm.

### For non-LTS Releases

If you are using non-LTS release, `nw-gyp` is required to be pre-installed globally to build modules due to [ABI differences in V8](https://github.com/nwjs/nw.js/issues/5025). These instructions below works for LTS releases as well.

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
    After a version switch of NW.js, the recommended approach is to delete `node_modules` folder and do a full npm install as [the instructions above](#install-with-npm).

Once you switch to a new version of NW.js, you can rebuild **each** of the native modules with the tools below **without reinstall all the modules**. Since `binding.gyp` is required for building native modules, you can easily locate all native modules by finding `binding.gyp` file.

!!! warning "Rebuild ALL Native Modules"
    Make sure you **rebuilt all native modules**. Or you will meet various issues, even crashes. Once manually rebuild don't work for you, try to do a full npm install.

### nw-gyp

[`nw-gyp`](https://github.com/nwjs/nw-gyp) is a hack on `node-gyp` to support NW.js specific headers and libraries.

The usage is the same with `node-gyp`, except that you need to specify the version and arch (`x64` or `ia32`) of NW.js manually.

````bash
npm install -g nw-gyp
cd myaddon
nw-gyp rebuild --target=0.13.0 --arch=x64
````

See https://github.com/nwjs/nw-gyp for more details.

### node-pre-gyp

Some packages uses [node-pre-gyp](https://github.com/mapbox/node-pre-gyp), which supports building for both Node.js and NW.js by using either `node-gyp` or `nw-gyp`.

The usage of `node-pre-gyp` is as following:

````bash
npm install -g node-pre-gyp
cd myaddon
node-pre-gyp build --runtime=node-webkit --target=0.13.0 --target_arch=x64
````

See https://github.com/mapbox/node-pre-gyp for more details.

### node-gyp

Since v0.83.0, it is possible to build Node addons using `node-gyp`.

!!! warning "Some Node addons may not build"
    Due to differences between NW.js's V8 and Node's V8, some addons may not build. See [this issue](https://github.com/nwjs/nw.js/issues/5025) for more info If this happens, please file a GitHub issue.

1. Download NW.js's Node headers. For v0.83.0, use `https://dl.nwjs.io/v0.83.0/nw-headers-v0.83.0.tar.gz`.
2. Install `node-gyp` via `npm`.
3. Point `node-gyp` to the node headers via the `--nodedir` argument and rebuild it.

Please consult the [NW.js Utilities website](https://nwutils.io/#codeexamples) for code examples.
