# Use Native Node Modules {: .doctitle}
---

[TOC]

## For LTS Releases

If you are using LTS release, native modules installed by `npm` can be supported after hacking as below:

* On Linux and Mac, no hacking is required.
* On Windows, you need to replace the file
`<npm-path>\node_modules\node-gyp\src\win_delay_load_hook.cc` with the one at https://github.com/nwjs/nw.js/blob/nw18/tools/win_delay_load_hook.cc before installing modules with node-gyp or npm.

## For non-LTS Releases

If you are using non-LTS release, you have to rebuild the modules with one of following tools due to [ABI differences in V8](https://github.com/nwjs/nw.js/issues/5025).

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

### Known Issues

So far, you have to rebuild **each native module** with tools above including thoses are indirectly depended modules. Since `binding.gyp` is required for building native modules, you can easily locate all native modules by finding `binding.gyp` file.

