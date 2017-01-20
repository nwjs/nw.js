# Enable Proprietary Codecs
---

[TOC]

## Audio / Video Support in NW.js

Due to legal and license issue, pre-built NW.js supports only non-proprietary codecs as Chromium. And this documentation will show you how to enable proprietary codecs support as **Chrome browser** in NW.js.

| | Demuxers | Decoders |
|:---:|:---|:---|
| NW.js Default | ogg<br>matroska<br>wav | vorbis<br>pcm_u8<br>pcm_s16le<br>pcm_s24le<br>pcm_s32le<br>pcm_f32le<br>pcm_s16be<br>pcm_s24be<br>pcm_mulaw<br>pcm_alaw<br>theora<br>vp8 |
| Proprietary | aac<br>mp3<br>mov | aac<br>h264<br>mp3 |

!!! warning "More Codecs Support"
    More codecs, such as "mpeg4" decoder and "avi" demuxer, can **NOT** be enabled by simply tweaking the arguments and rebuilding ffmpeg. You have to regenerate the configuration files for ffmpeg. And some hack on Chromium source code may also required. It is out of the scope of this document. Please refer to external project [nwjs-ffmpeg-prebuilt](https://github.com/iteufel/nwjs-ffmpeg-prebuilt) on Github and [Compilation Guide for FFmpeg](https://trac.ffmpeg.org/wiki/CompilationGuide) for more details.

## Enable Proprietary Codecs in NW.js

!!! warning "License and Patent Fee"
    Using MP3 and H.264 codecs requires you to pay attention to the patent royalties and the license of the source code. Consult a lawyer if you do not understand the licensing constraints and using patented media formats in your application. For more information about the license of the source code, check [here](https://chromium.googlesource.com/chromium/third_party/ffmpeg.git/+/master/CREDITS.chromium)

!!! warning "Warning"
    If you do **NOT** have the license, using this tip below or moving to other solutions does **NOT** make you eligible to redistribute the codecs.

### Option 1. Get FFmpeg Binaries from the Community

In recent versions of the Chromium project, FFmpeg was changed to be built-in. So, you can't get the FFmpeg DLL from the official Chrome distribution anymore. However, you can get [precompiled binaries](https://github.com/iteufel/nwjs-ffmpeg-prebuilt/releases) from the community.

Then replace the bundled ffmpeg in NW.js with the one got from community. The path and file name of bundled library vary between platforms:

* Windows: `ffmpeg.dll`
* Mac OS X: `nwjs.app/Contents/Versions/<chromium-version>/nwjs Framework.framework/libffmpeg.dylib`
* Linux `lib/libffmpeg.so`

### Option 2. Build FFmpeg DLL without Building Entire NW.js

If you are using pre-built NW.js, you can just only rebuild FFmpeg DLL and replace the bundled DLL in pre-built binaries. This will save lots of time in download size (~1G) comparing to building entire NW.js (~20G).

**Step 0.** Install prerequisites

See [Building NW.js - Prerequisites](Building NW.js.md#prerequisites).

**Step 1.** Fetch base repo for NW.js

First, **create** `.gclient` file in current directory with following command.

```bash
gclient config --unmanaged --name=src https://github.com/nwjs/chromium.src.git@<git/reference>
```

* `<git/reference>` is usually pointed to specific tag of release, for example `tags/nw-v0.19.0`.

Then **fetch** base repo of NW.js into `src` folder with following command.

```bash
git clone --depth=1 -b <branch> --single-branch https://github.com/nwjs/chromium.src.git src
```

* `<branch>` is usually the pointed to specific tag of release as above, for example `nw-v0.19.0`.
* `--depth=1` will do a [shallow clone](https://git-scm.com/docs/git-clone#git-clone---depthltdepthgt) to speed up the download time. It's optional.

**Step 2.** Fetch dependencies

After fetched the base repo, you will find `DEPS` file in `src` folder. It is a Python script defined variables for fetching dependencies and hooks. All of the variables in `DEPS` file are [dictionaries](https://docs.python.org/2/library/stdtypes.html#mapping-types-dict) or [lists](https://docs.python.org/2/library/stdtypes.html#sequence-types-str-unicode-list-tuple-bytearray-buffer-xrange).

First, **customize** `DEPS` as below to define minimal dependencies for building FFmpeg.

* In `vars`, remove all items except those whose key is `chromium_git`
* In `deps`, remove all items except those whose key is in followings:
    - `src/buildtools`
    - `src/tools/gyp`
    - `src/third_party/yasm/sources/patched-yasm`
    - `src/third_party/ffmpeg`
* In `hooks`, remove all items except those whose `name` is in followings:
    - `clang`
    - `gn_win`
    - `gn_mac`
    - `gn_linux64`
    - `sysroot`
* Remove all other variables other than `vars`, `deps` and `hooks` (Optional)

Then **fetch** all dependencies with `gclient sync --no-history`.

!!! tip "For Linux Developers"
    Please run `build/install-build-deps.sh` for the first time building FFmpeg DLL or NW.js before proceeding to the instructions below. You only have to run it once. This script will install the build dependencies automatically for you.

**Step 3.** Generate build files

First, **replace** the content of `BUILD.gn` in `src` folder with following.

```
group("dummy") {
  deps = [
    "//third_party/ffmpeg"
  ]
}
```

Then **generate** build files in `out/nw` by running following command in `src` folder.

```bash
gn args //out/nw
```

... and type in the content below

```none
is_debug=false
is_component_ffmpeg=true
target_cpu="<target_cpu>"
is_official_build=true
ffmpeg_branding="Chrome"
```

* `<target_cpu>` should be set to `x86` or `x64` for 32-bit or 64-bit build.

**Step 4.** Build ffmpeg library

```bash
ninja -C out/nw ffmpeg
```

You will find the library in `out/nw` folder. The path and file name varies between platforms:

* Windows: `ffmpeg.dll`
* Mac OS X: `libffmpeg.dylib`
* Linux: `lib/libffmpeg.so`

**Step 5.** Replace the bundled library in pre-built NW.js with your new build.

### Option 3. Build Entire NW.js with Proprietary Codecs

You can build entire NW.js with proprietary codecs enabled by following the instructions in [Building NW.js](Building NW.js.md). Extra argument `ffmpeg_branding="Chrome"` is required in the step of generating build files for GN.
