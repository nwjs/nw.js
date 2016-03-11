# Use Native Node Modules {: .doctitle}
---

[TOC]

`npm` tool provided by Node.js builds the native modules, i.e. [C/C++ Addons](https://nodejs.org/api/addons.html), at the time of installation.

Starting from 0.13.0, native modules built by node-gyp or npm in upstream can be supported.

In Linux and OSX you can just load the native module directly. In windows you'll need to replace the file
%APPDATA%\npm\node_modules\node-gyp\src\win_delay_load_hook.c with the one at https://github.com/nwjs/nw.js/blob/nw13/tools/win_delay_load_hook.c 

Before 0.13.0, the V8 version and Node API in NW.js is different from official Node.js. To use native Node.js modules with NW.js, you have to rebuild the modules with one of following tools.

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

So far, you have to rebuild **each native module** with tools above including thoses are indirectly depended modules. Since `binding.gyp` is required for building native modules, you can easily locate all native modules by finding `binding.gyp` file.

