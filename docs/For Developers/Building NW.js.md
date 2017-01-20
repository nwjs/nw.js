# Building NW.js {: .doctitle}
---

[TOC]

!!! important
    This document is written for latest **NW 0.13** or later. For legacy build instructions, please read the [wiki page](https://github.com/nwjs/nw.js/wiki/Building-nw.js) on GitHub.
    See our buildbot for official build configuration and steps: http://buildbot-master.nwjs.io:8010/waterfall
    Debug version should be built with component build configuration. See the [discussion in upstream](https://groups.google.com/a/chromium.org/d/msg/chromium-dev/LWRR3fMlvQ4/qos4EaBtBgAJ).

## Prerequisites

NW.js use same build tools and similar steps as Chromium. Read the instructions according to your platform to install `depot_tools` and other prerequistes:

* [Windows](http://www.chromium.org/developers/how-tos/build-instructions-windows)
* [Mac OS X](https://chromium.googlesource.com/chromium/src/+/master/docs/mac_build_instructions.md)
* [Linux](https://chromium.googlesource.com/chromium/src/+/master/docs/linux_build_instructions.md)

!!! note "Windows"
    As suggested by Chromium document, you need to run `set DEPOT_TOOLS_WIN_TOOLCHAIN=0` or set the variable in your global environment.

## Get the Code

**Step 1.** Create a folder for holding NW.js source code, like `$HOME/nwjs`, and run following command in the folder to generate `.gclient` file:

```bash
mkdir -p $HOME/nwjs
cd $HOME/nwjs
gclient config --name=src https://github.com/nwjs/chromium.src.git@origin/nw19
```

Generally if you are not interested in running Chromium tests, you don't have to sync the test cases and reference builds, which saves you lot of time. Open the `.gclient` file you just created and replace `custom_deps` section with followings:

```python
"custom_deps" : {
    "src/third_party/WebKit/LayoutTests": None,
    "src/chrome_frame/tools/test/reference_build/chrome": None,
    "src/chrome_frame/tools/test/reference_build/chrome_win": None,
    "src/chrome/tools/test/reference_build/chrome": None,
    "src/chrome/tools/test/reference_build/chrome_linux": None,
    "src/chrome/tools/test/reference_build/chrome_mac": None,
    "src/chrome/tools/test/reference_build/chrome_win": None,
}
```

**Step2.** Manually clone and checkout correct branches for following repositories:

| path | repo |
|:---- |:---- |
| src/content/nw | https://github.com/nwjs/nw.js |
| src/third_party/node | https://github.com/nwjs/node |
| src/v8 | https://github.com/nwjs/v8 |

**Step 3.** Syncing full source code with following command:
```bash
gclient sync --with_branch_heads
```

This usually downloads 20G+ from GitHub and Google's Git repos. Make sure you have a good network provider and be patient :stuck_out_tongue:

When finished, you will see a `src` folder created in the same folder as `.gclient`.

!!! note "First Build on Linux"
    If you are building on Linux **for the first time**, you need to do this in 3 steps:

    * `gclient sync --with_branch_heads --nohooks`
    * `./build/install-build-deps.sh` to install dependencies on Ubuntu
    * `gclient runhooks` to download common dependencies

## Generate build files

Chromium has shifted from [`gyp`](https://gyp.gsrc.io/) to [`gn`](https://chromium.googlesource.com/chromium/src/+/master/tools/gn/docs/quick_start.md) to generate build files. But Node.js is still based on `gyp`. So you have to generate build files for both of them.

### Generate ninja build files with GN for Chromium

Run `gn args out/nw` in `src` folder and it will open an editor for you to input build arguments. Typically you can type in following build arguments for **non-debugable** **64-bit** **normal build** NW.js with **ffmpeg built as dynamic library**:

````
is_debug=false
is_component_ffmpeg=true
target_cpu="x64"
nwjs_sdk=false
````

Then it will generate ninja build files in `out/nw`. You can re-run `gn args out/nw` to edit the arguments later.

Here are some useful GN arguments:

* `nwjs_sdk` (`bool`): build NW.js SDK flavor with DevTools enabled
* `is_debug` (`bool`): build in debug mode with debugable symbols and no optimization
* `is_component_build` (`bool`): link binaries into separate dynamic libraries. This will speed up the build in debug mode.
* `enable_nacl` (`bool`): enable NaCl
* `target_cpu` (`string`): "x64" and "x86" for 64-bit and 32-bit binaries
* `is_official_build` (`bool`): enable more optimizations for official release
* `is_component_ffmpeg` (`bool`): build ffmpeg as dynamic library

See Google's [GN build configuration](https://www.chromium.org/developers/gn-build-configuration) for more details.

### Generate ninja build files with GYP for Node

Run following scripts in `src` folder:

```bash
export GYP_DEFINES="target_arch=x64"
./build/gyp_chromium -I third_party/node/common.gypi third_party/node/node.gyp
```

This will generate ninja build files in `out/Debug` and `out/Release` folders.

!!! note "Build Folders for 64-bit Binaries on Windows"
    On Windows, build folders for 64-bit binaries are located in `out/Debug_x64` and `out/Release_x64`.

Here are some useful GYP arguments set with `GYP_DEFINES` environment:

* `nwjs_sdk` (`bool`): set to 1 to build NW.js SDK flavor with DevTools enabled.
* `target_arch` (`string`): "x64" and "ia32" for 64-bit and 32-bit binaries
* `component` (`string`): set to "shared_library" to link binaries into separate dynamic libraries. This will speed up the build in debug mode.
* `clang` (`bool`): set to 1 to enable building with `clang` toolchain. **This should be set to 0 on Windows**.

## Build

### Build nwjs

Ninja build files are generated in `out/nw` folder after you run GN. Run following command in `src` folder will generate NW.js binaries in `out/nw` folder:

```bash
ninja -C out/nw nwjs
```

!!! tip "Build Time"
    Generally a full build takes hours of time depending on the performance of your machine. Recommended configuration is to build on a PC with multicore CPU (>=8 cores), SSD and large memory (>= 8G). And you can read [Build Faster](#build-faster) section below for some tips to speed up the build.

### Build Node

Build Node by running following scripts in `src` folder:

```bash
ninja -C out/Release node
```

Then copy the build Node library to the nwjs binary folder:

```bash
ninja -C out/nw copy_node
```

To switch between 32-bit/64-bit binaries or non-standard build flavors, you need to update `GYP_DEFINES` environment and [regenerate ninja files](#generate-build-files) and then re-run the commands above to generate binaries.

### Generate NW.js Distribution Packages (Optional)

Run following script sin `src` folder:

```bash
ninja -C out/nw dist
```

You will get NW.js packages in `out/nw/dist` including binaries, symbols and nw-gyp headers (generated on macOS only) and libraries (generated on Windows only).

You can only generate packages with `is_debug=false`.

## Build Flavors

For different build flavors, you need to set corresponding arguments for GN and GYP as below:

|     | GN | GYP |
|:---:|:---|:---|
| Normal | nwjs_sdk=false | nwjs_sdk=0 |
| SDK | nwjs_sdk=true<br>enable_nacl=true | nwjs_sdk=1 |

See [Build Flavors](../For Users/Advanced/Build Flavors.md) for the differences of all supported build flavors.

## Enable Proprietary Codecs

Due to the license issue, the prebuilt binaries of NW.js doesn't support proprietary codecs, like H.264. So you can't play MP3/MP4 with `<audio>` and `<video>` tags with prebuilt NW.js. To enable those medias, you have to build NW.js from source code by following the document of [Enable Proprietary Codecs](Enable Proprietary Codecs.md).

## Build Faster

From Google's website, there are a few tips to speed up your build. Open the links below to see the tips for your platform:

* [Mac Build Instructions: Faster builds](https://chromium.googlesource.com/chromium/src/+/master/docs/mac_build_instructions.md#Faster-builds)
* [Linux Build Instructions: Faster builds](https://chromium.googlesource.com/chromium/src/+/master/docs/linux_build_instructions.md#faster-builds)
* [Windows Build Instructions: Faster builds](https://chromium.googlesource.com/chromium/src/+/master/docs/windows_build_instructions.md#Faster-builds)
