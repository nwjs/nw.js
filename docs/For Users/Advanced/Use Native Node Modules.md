# Use Native Node Modules {: .doctitle}
---

[TOC]

## npm and node-gyp

!!! note "New for 0.13.0"
    Starting from 0.13.0, native modules built by `node-gyp` or `npm` in upstream can be directly supported by NW.js.

`npm` tool provided by Node.js builds the native modules, i.e. [C/C++ Addons](https://nodejs.org/api/addons.html), at the time of installation.

On Linux and OSX, the native module built with `npm` or `node-gyp` can be loaded by NW.js directly.

On Windows you'll need to replace the file
`<npm-path>\node_modules\node-gyp\src\win_delay_load_hook.c` with the one at https://github.com/nwjs/nw.js/blob/nw13/tools/win_delay_load_hook.c before using `npm` or `node-gyp`. Typically `<npm-path>` is located in the installation directory of Node.js, such as `C:\Program Files\nodejs\node_modules\npm` if Node.js is installed with MSI installer.

!!! warning "Use Same Version and Architecture of Node.js as NW.js"
    Make sure you are running the same **version** and **architecture** of Node.js as NW.js when building native modules. For example, if targeting for NW.js 0.13.0 x64, your app should be built with Node.js 5.7.0 x64. Different version or architecture will lead to loading error in runtime.

Before 0.13.0, the V8 version and Node ABI in NW.js is different from official Node.js. To use native Node.js modules with previous releases of NW.js, you have to rebuild the modules with one of following tools.

## nw-gyp

[`nw-gyp`](https://github.com/nwjs/nw-gyp) is a hack on `node-gyp` to support NW.js specific headers and libraries. 

The usage is the same with `node-gyp`, except that you need to specify the version and arch (`x64` or `ia32`) of NW.js manually. 

````bash
npm install -g nw-gyp
cd myaddon
nw-gyp rebuild --target=0.13.0 --arch=x64
````

See https://github.com/nwjs/nw-gyp for more details.

## node-pre-gyp

Some packages uses [node-pre-gyp](https://github.com/mapbox/node-pre-gyp), which supports building for both Node.js and NW.js by using either `node-gyp` or `nw-gyp`.

The usage of `node-pre-gyp` is as following:
````bash
npm install -g node-pre-gyp
cd myaddon
node-pre-gyp build --runtime=node-webkit --target=0.13.0 --target_arch=x64
````

See https://github.com/mapbox/node-pre-gyp for more details.

## Known Issues

By using `nw-gyp` or `noe-pre-gyp`, you have to rebuild **each native module** including thoses are indirectly depended. Since `binding.gyp` is required for building native modules, you can locate all native modules by finding `binding.gyp` file.

