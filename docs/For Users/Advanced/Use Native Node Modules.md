# Use Native Node Modules {: .doctitle}
---

[TOC]

## npm and node-gyp (Recommended for 0.13.0 or later)

!!! note "New for 0.13.0"
    Starting from 0.13.0, native modules built by `node-gyp` or `npm` in upstream can be directly supported by NW.js.

`npm` tool provided by Node.js builds the native modules, i.e. [C/C++ Addons](https://nodejs.org/api/addons.html), at the time of installation.

* On Linux and OSX, the native module built with `npm` or `node-gyp` can be loaded by NW.js directly. No workaround is needed.
* On Windows, you'll need to replace the file
`<node-gyp-path>\src\win_delay_load_hook.c` with the one at https://github.com/nwjs/nw.js/blob/nw13/tools/win_delay_load_hook.c before using `npm` or `node-gyp`.

!!! warning "Correct Path of `node-gyp` on Windows"
    If you run `npm install` to install dependencies, you have to patch `node-gyp` **inside npm installation path**. The **last line of output of `npm` command** will show you the path of `npm`. And `node-gyp` used by `npm` is in `<npm-path>\node_modules\node-gyp`.
    If you build native modules manually with `node-gyp`, you will need to patch `node-gyp` **installed globally**. Running `where node-gyp` will show you the path of `node-gyp` (the first one in the list). Then the path of `node-gyp` is in `<where-node-gyp>\node_modules\node-gyp`.

!!! warning "Use Same Version and Architecture of Node.js as NW.js"
    Make sure you are running the same **version** and **architecture** of Node.js as NW.js when building native modules. For example, if targeting for NW.js 0.13.0 x64, your app should be built with Node.js 5.9.0 x64. Different version or architecture will lead to loading error in runtime.

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

