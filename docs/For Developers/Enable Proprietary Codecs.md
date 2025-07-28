# Enable Proprietary Codecs
---

[TOC]

## Supported Codecs in Prebuilt NW.js Binary

As NW.js is based on Chromium, the media components are essentially the same. In the pre-built NW.js, the following codecs are supported:

    - vorbis
    - vp8 & vp9
    - pcm_u8, pcm_s16le, pcm_s24le, pcm_f32le, pcm_s16be & pcm_s24be
    - mp3 (v0.22.1+)

... as well as these demuxers:

    - ogg
    - matroska
    - wav

## Enable Proprietary Codecs in NW.js

!!! warning "License and Patent Fee"
    Using the H.264 codec requires you to pay attention to the patent royalties and the license of the source code. Consult a lawyer if you do not understand the licensing constraints and using patented media formats in your application. For more information about the license of the source code, check [here](https://chromium.googlesource.com/chromium/third_party/ffmpeg.git/+/master/CREDITS.chromium).

!!! warning "Warning"
    If you do **NOT** have the license, using this tip below or moving to other solutions does **NOT** make you eligible to redistribute the codecs.

### Get FFmpeg Binaries from the Community

In recent versions of the Chromium project, FFmpeg was changed to be built-in. So, you can't get the FFmpeg DLL from the official Chrome distribution anymore. However, you can get [precompiled binaries](https://github.com/iteufel/nwjs-ffmpeg-prebuilt/releases) from the community. Another alternative would be to build FFmpeg yourself by following the instructions below.

### Build FFmpeg DLL without Building NW.js

If you are using pre-built NW.js, you can just only rebuild FFmpeg DLL and replace the bundled DLL in pre-built binaries. This will save lots of time in download size (~1G) comparing to building entire NW.js (~20G).

**Step 1.** Download zip package of customized Chromium from GitHub. You can find the corresponding version in https://github.com/nwjs/chromium.src/tags. Extract the zip package into a local folder, such as `~/nw`. The unzipped package will include a sub directory. So the source folder is in `~/nw/<sub-directory-name>`.

**Step 2.** Fetch dependencies

Since you are not building entire NW.js, you have to fetch following dependencies manually.

* Git clone following folders with corresponding commits as in `DEPS` file:
    - `buildtools`
    - `tools/gyp`
    - `third_party/yasm/sources/patched-yasm`
    - `third_party/ffmpeg`
* Download `gn` tools in `buildtools/<os>` by
```bash
download_from_google_storage --no_resume \
                             --platform=<platform> \
                             --no_auth \
                             --bucket chromium-gn \
                             -s buildtools/<os>/<gn-exe>.sha1
```
    - `<platform>`: `win32` for Windows; `darwin` for Mac; `linux*` for Linux
    - `<os>`: `win` for Windows; `mac` for Mac; `linux64` for Linux
    - `<gn-exe>`: `gn.exe` for Windows; `gn` for Mac and Linux
* **[Mac and Linux]** Download `clang` tools in `third_party/llvm-build` by
```bash
python tools/clang/scripts/update.py --if-needed
```
* **[Linux]** Download libraries in `build/linux/*-sysroot` by
```bash
python build/linux/sysroot_scripts/install-sysroot.py --running-as-hook
```
* **[Mac]** Download **libc++-static** library in `third_party/libc++-static` by
```bash
download_from_google_storage --no_resume \
                             --platform=darwin \
                             --no_auth \
                             --bucket chromium-libcpp \
                             -s third_party/libc++-static/libc++.a.sha1
```

!!! tip "For Linux Developers"
    Please run `build/install-build-deps.sh` for the first time building FFmpeg DLL or NW.js before proceeding to the instructions below. You only have to run it once. This script will install the build dependencies automatically for you.

**Step 3.** Replace BUILD.gn

Replace `BUILD.gn` in the root of the source code with following:

```
action("dummy") {
  deps = [
    "//third_party/ffmpeg"
  ]
  script = "dummy"
  outputs = ["$target_gen_dir/dummy.txt"]
}
```

**Step 4.** Generate Ninja Files with GN Tools

```bash
cd path/to/nw/source/folder
gn gen //out/nw \
 --args='is_debug=false is_component_ffmpeg=true target_cpu="<target_cpu>" is_official_build=true ffmpeg_branding="Chrome"'
```

**NOTE**: `<target_cpu>` should be set to `x86` or `x64` for 32-bit or 64-bit build.

**Step 4.** Build ffmpeg DLL

```bash
ninja -C out/nw ffmpeg
```

You will find the DLL in `out/nw` folder. The path and file name varies between platforms:

* Windows: `ffmpeg.dll`
* Mac OS X: `libffmpeg.dylib`
* Linux: `lib/libffmpeg.so`

**Step 5.** Replace the bundled DLL in prebuild NW.js with the one you just built. The path and file name varies between platforms:

* Windows: `ffmpeg.dll`
* Mac OS X: `nwjs.app/Contents/Versions/<chromium-version>/nwjs Framework.framework/libffmpeg.dylib`
* Linux `lib/libffmpeg.so`

### Build Entire NW.js with Proprietary Codecs

If you don't use official pre-built NW.js, you can build entire NW.js with proprietary codecs enabled by following the instructions below. See [Building NW.js](Building NW.js.md) for details of each steps.

**Step 1.** Install prerequists and fetch the source code of NW.js. See *Prerequisits* and *Get the Code* sections in [Building NW.js](Building NW.js.md).

**Step 2.** Set `ffmpeg_branding` to `Chrome` when you configure GN.

**Step 3.** Regenerate the ninja files again.

**Step 4.** Rebuild NW.js.
