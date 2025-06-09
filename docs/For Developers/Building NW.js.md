# Building NW.js {: .doctitle}
---

[TOC]

!!! important
    Starting from 0.17, we switched to the new build configuration system GN and Ninja in Chromium upstream. The Node.js in NW is still build with GYP/Ninja
    This document is written for latest **NW 0.13** or later. For legacy build instructions, please read the [wiki page](https://github.com/nwjs/nw.js/wiki/Building-nw.js) on GitHub.
    See our buildbot for official build configuration and steps: http://buildbot-master.nwjs.io:8010/waterfall
    Debug version should be built with component build configuration. See the [discussion in upstream](https://groups.google.com/a/chromium.org/d/msg/chromium-dev/LWRR3fMlvQ4/qos4EaBtBgAJ).

## Prerequisites

NW.js use same build tools and similar steps as Chromium. Read the instructions according to your platform to install `depot_tools` and other prerequistes:

* [Windows](https://chromium.googlesource.com/chromium/src/+/master/docs/windows_build_instructions.md)
* [Mac OS X](https://chromium.googlesource.com/chromium/src/+/master/docs/mac_build_instructions.md)
* [Linux](https://chromium.googlesource.com/chromium/src/+/master/docs/linux_build_instructions.md)

!!! note "Windows"
    As suggested by Chromium document, you need to run `set DEPOT_TOOLS_WIN_TOOLCHAIN=0` or set the variable in your global environment.

!!! note "Xcode 7"
    Mac SDK 10.11 as part of Xcode 7 is not supported yet. If you have upgraded to Xcode 7, either downgrade to Xcode 6 or copy Mac SDK 10.10 from other machines under ```xcode-select -p`/Platforms/MacOSX.platform/Developer/SDKs`` as suggested by [Chromium document](https://chromium.googlesource.com/chromium/src/+/master/docs/mac_build_instructions.md).

## Get the Code

**Step 1.** Create a folder for holding NW.js source code, like `$HOME/nwjs`, and run following command in the folder to generate `.gclient` file:

```bash
mkdir -p $HOME/nwjs
cd $HOME/nwjs
gclient config --name=src https://github.com/nwjs/chromium.src.git@origin/nw17
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

Manually clone and checkout correct branches for following repositories:

| path | repo |
|:---- |:---- |
| src/content/nw | https://github.com/nwjs/nw.js |
| src/third_party/node-nw | https://github.com/nwjs/node |
| src/v8 | https://github.com/nwjs/v8 |

**Step 2.** Run following command in your terminal:
```
gclient sync --with_branch_heads
```

This usually downloads 20G+ from GitHub and Google's Git repos. Make sure you have a good network provider and be patient :stuck_out_tongue:

When finished, you will see a `src` folder created in the same folder as `.gclient`.

!!! note "First Build on Linux"
    If you are building on Linux **for the first time**, you need to run `gclient sync --with_branch_heads --nohooks` and then run `./build/install-build-deps.sh` to install dependencies on Ubuntu. See [Chromium document](http://dev.chromium.org/developers/how-tos/get-the-code) for detailed instructions of getting the source code.

## Generate ninja build files with GN for Chromium

```bash
cd src
gn gen out/nw  (changing the 'nw' part of the path is not supported. You'd better not changing the 'out' part if you're new to this)
```

Use flags like our official build:
````
is_debug=false
is_component_ffmpeg=true
target_cpu="x64"
````
We support component build: `is_component_build = true` for faster development cycle. It should be used when you are trying to build debug version.

See the upstream documentation for the mapping between GN and GYP flags: https://chromium.googlesource.com/chromium/src/+/master/tools/gn/docs/cookbook.md#Variable-mappings

## Generate ninja build files with GYP for Node

Set the following environment variables:
````
GYP_DEFINES=target_arch=x64 building_nw=1 clang=1 icu_use_data_file_flag=1 host_arch=x64 nwjs_sdk=1 disable_nacl=0 buildtype=Official
GYP_GENERATORS=ninja
GYP_CHROMIUM_NO_ACTION=0
GYP_GENERATOR_FLAGS=output_dir=out
````

```bash
cd src
python3 third_party/node-nw/tools/gyp/gyp_main.py -I third_party/node-nw/common.gypi -D msvs_version=2021 -D build_type=Release_x64 third_party/node-nw/node.gyp
```

or use the following if you're doing a component build:
```bash
python3 third_party/node-nw/tools/gyp/gyp_main.py -D component=shared_library -I third_party/node-nw/common.gypi -D msvs_version=2021 -D build_type=Release_x64 third_party/node-nw/node.gyp
```
To change the build configuration for Node, you need to setup the GYP_DEFINES environment variable:

### 32-bit/64-bit Build

* Windows
    - 32-bit: is the default build target
    - 64-bit: `set GYP_DEFINES="target_arch=x64"` and rebuild in `out/Debug_x64` or `out/Release_x64` folder
* Linux
    - 32-bit: **TODO: chroot**
    - 64-bit: is the default build target
* Mac
    - 32-bit: `export GYP_DEFINES="host_arch=ia32 target_arch=ia32"` and rebuild in `out/Debug` or `out/Release` folder
    - 64-bit: is the default build target

## Build nwjs

ninja build files are generated in `out/nw` folder after you run GN. Run following command in your terminal will generate the Debug build of standard NW.js binaries in `out/nw` folder:

```bash
cd src
ninja -C out/nw nwjs
```

!!! tip "Build Time"
    Generally a full build takes hours of time depending on the performance of your machine. Recommended configuration is to build on a PC with multicore CPU (>=8 cores), SSD and large memory (>= 8G). And you can read [Build Faster](#build-faster) section below for some tips to speed up the build.

To build 32-bit/64-bit binaries or non-standard build flavors, you need to edit out/nw/args.gn file And then re-run the commands above to generate binaries.

## Build Node

```bash
cd src
ninja -C out/Release node
```

After building Node, the final step is to copy the build Node library to the nwjs binary folder:

```bash
cd src
ninja -C out/nw copy_node
```

## Build Flavors

* Standard: 'nwjs_sdk=false'
* SDK: `enable_nacl=true`

See [Build Flavors](../For Users/Advanced/Build Flavors.md) for the differences of all supported build flavors.

## Enable Proprietary Codecs

Due to the license issue, the prebuilt binaries of NW.js doesn't support proprietary codecs, like H.264. So you can't play MP3/MP4 with `<audio>` and `<video>` tags with prebuilt NW.js. To enable those medias, you have to build NW.js from source code by following the document of [Enable Proprietary Codecs](Enable Proprietary Codecs.md).

## Build Faster

From Google's website, there are a few tips to speed up your build. Open the links below to see the tips for your platform:

* [Mac Build Instructions: Faster builds](https://chromium.googlesource.com/chromium/src/+/master/docs/mac_build_instructions.md#Faster-builds)
* [Tips for improving build speed on Linux](https://chromium.googlesource.com/chromium/src/+/master/docs/linux_faster_builds.md)
