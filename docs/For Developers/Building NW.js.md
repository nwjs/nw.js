# Building NW.js {: .doctitle}
---

[TOC]

!!! important
    This document is written for latest **NW 0.13**. For legacy build instructions, please read the [wiki page](https://github.com/nwjs/nw.js/wiki/Building-nw.js) on GitHub.
    See our buildbot for official build configuration and steps: http://buildbot-master.nwjs.io:8010/waterfall

## Prerequisites

NW.js use same build tools and similar steps as Chromium. Read the instructions according to your platform to install `depot_tools` and other prerequistes:

* [Windows](http://www.chromium.org/developers/how-tos/build-instructions-windows)
* [Mac OS X](https://chromium.googlesource.com/chromium/src/+/master/docs/mac_build_instructions.md)
* [Linux](https://chromium.googlesource.com/chromium/src/+/master/docs/linux_build_instructions.md)

!!! note "Windows"
    As suggested by Chromium document, you need to run `set DEPOT_TOOLS_WIN_TOOLCHAIN=0` or set the variable in your global environment.
    CLang is the build tool used by non-Windows platforms. On Windows, it's not supported yet. You need to run `set GYP_DEFINES="clang=0"` to disable CLang on Windows before going to next steps.

!!! note "Xcode 7"
    Mac SDK 10.11 as part of Xcode 7 is not supported yet. If you have upgraded to Xcode 7, either downgrade to Xcode 6 or copy Mac SDK 10.10 from other machines under ```xcode-select -p`/Platforms/MacOSX.platform/Developer/SDKs`` as suggested by [Chromium document](https://chromium.googlesource.com/chromium/src/+/master/docs/mac_build_instructions.md).

## Get the Code

**Step 1.** Create a folder for holding NW.js source code, like `$HOME/nwjs`, and run following command in the folder to generate `.gclient` file:

```bash
mkdir -p $HOME/nwjs
cd $HOME/nwjs
gclient config --name=src https://github.com/nwjs/chromium.src.git@origin/nw13
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

**Step 2.** Run following command in your terminal:
```
gclient sync --with_branch_heads
```

This usually downloads 20G+ from GitHub and Google's Git repos. Make sure you have a good network provider and be patient :stuck_out_tongue:

When finished, you will see a `src` folder created in the same folder as `.gclient`.

!!! note "First Build on Linux"
    If you are building on Linux **for the first time**, you need to run `gclient sync --with_branch_heads --nohooks` and then run `./build/install-build-deps.sh` to install dependencies on Ubuntu. See [Chromium document](http://dev.chromium.org/developers/how-tos/get-the-code) for detailed instructions of getting the source code.

!!! note "First Build on Windows"
    On Windows, you have to install [DirectX SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812) and copy the files into the source folder manually using following bash command:

```bash
mkdir -p $HOME/nwjs/src/third_party/directxsdk/files
cp -r /c/Program\ Files\ \(x86\)/Microsoft\ DirectX\ SDK\ \(June\ 2010\)/* \
$HOME/nwjs/src/third_party/directxsdk/files/
```

## Build

Build files are generated in `out/` folder during `gclient sync`. Run following command in your terminal will generate the Debug build of standard NW.js binaries in `out/Debug` folder:

```bash
cd src
ninja -C out/Debug nwjs
```

!!! tip "Build Time"
    Generally a full build takes hours of time depending on the performance of your machine. Recommended configuration is to build on a PC with multicore CPU (>=8 cores), SSD and large memory (>= 8G). And you can read [Build Faster](#build-faster) section below for some tips to speed up the build.

To generate Release build, switch the second command to `ninja -C out/Release nwjs`.

To build 32-bit/64-bit binaries or non-standard build flavors, you need to setup `GYP_DEFINES` variable in your environment and run `gclient runhooks --force` to generate build files. And then re-run the commands above to generate binaries. Continue to read following sections to find out how to setup `GYP_DEFINES`.

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

### Build Flavors

* Standard: it's generated by default. Same as `GYP_DEFINES="nwjs_sdk=0 disable_nacl=1"`.
* SDK: `GYP_DEFINES="nwjs_sdk=1 disable_nacl=0"`
* NaCl: `GYP_DEFINES="disable_nacl=0"`

See [Build Flavors](../For Users/Advanced/Build Flavors.md) for the differences of all supported build flavors.

### Enable Proprietary Codecs

Due to the license issue, the prebuilt binaries of NW.js doesn't support proprietary codecs, like H.264. So you can't play MP3/MP4 with `<audio>` and `<video>` tags with prebuilt NW.js. To enable those medias, you have to build NW.js from source code by following the document of [Enable Proprietary Codecs](Enable Proprietary Codecs.md).

## Build Faster

From Google's website, there are a few tips to speed up your build. Open the links below to see the tips for your platform:

* [Mac Build Instructions: Faster builds](https://chromium.googlesource.com/chromium/src/+/master/docs/mac_build_instructions.md#Faster-builds)
* [Tips for improving build speed on Linux](https://chromium.googlesource.com/chromium/src/+/master/docs/linux_faster_builds.md)
