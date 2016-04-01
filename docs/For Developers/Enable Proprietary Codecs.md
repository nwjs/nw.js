# Enable Proprietary Codecs
---

[TOC]

## Supported Codecs in Prebuilt NW.js Binary

As NW.js is based on Chromium, the media components are essentially the same.

In the pre-built NW.js, following codecs are supported:

```none
theora,vorbis,vp8,pcm_u8,pcm_s16le,pcm_s24le,pcm_f32le,pcm_s16be,pcm_s24be
```

and following demuxers are supported:

```none
ogg,matroska,wav
```

## Enable Proprietary Codecs in NW.js

!!! warning "License and Patent Fee"
    MP3 and H.264 codecs are licensed under the GPL in `ffmpeg` used by NW.js. Make sure your app are released with compatible license of GPL. And you also have to pay patent licensing royalties for using them. Consult a lawyer if you do not understand the licensing constraints and using patented media formats in your application.

!!! warning "Warning"
    If you do **NOT** have the license or **NOT** pay the patent fee properly, using this tip below or moving to other solutions does **NOT** make you eligible to redistribute the codecs.

### Get FFmpeg DLL from Community

In recent versions of Chromium project, ffmpeg was changed to be builtin. So you can't get ffmpeg dll any more from official Chrome. But you can get DLL [from the community](https://github.com/nwjs/nw.js/issues/4492). Or build FFmpeg yourself by following instructions below.

### Build FFmpeg DLL without Building NW.js

If you are using pre-built NW.js, you can just only rebuild FFmpeg DLL and replace the bundled DLL in pre-built binaries. This will save lots of time in download size (~1G) comparing to building entire NW.js (~20G).

**Step 1.** Download zip package of customized Chromium from GitHub. You can find the corresponding version in https://github.com/nwjs/chromium.src/tags. Extract the zip package into a local folder, such as `~/nw`. The unzipped package will include a sub directory. So the source folder is in `~/nw/<sub-directory-name>`.

**Step 2.** Fetch dependencies

Since you are not building entire NW.js, you have to fetch following dependencies manually.

* `build/linux/*-sysroot`: This is Linux only. Use `build/linux/sysroot_scripts/install-sysroot.py --running-as-hook` to install the dependencies.
* `tools/gyp`: See `DEPS` file in the source folder for the repo and commit for this folder.
* `third_party/yasm/sources/patched-yasm`: See `DEPS` file in the source folder for the repo and commit for this folder.
* `third_party/ffmpeg`: see DEPS for commit and repo
* `third_party/llvm-build`: Mac and Linux only. Use `tools/clang/scripts/update.py --if-needed` to download.

**Step 3.** Generate build targets with GYP

!!! tip "For Linux Developers"
    Please run `build/install-build-deps.sh` for the first time building FFmpeg DLL or NW.js before proceeding to the instructions below. You only have to run it once. This script will install the build dependencies automatically for you.

!!! tip "For Windows Developers"
    Please include `clang=0` in `GYP_DEFINES` below because Clang build is not enabled on Windows.

```bash
cd path/to/nw/source/folder
export GYP_GENERATORS=ninja
export GYP_DEFINES="branding=Chrome ffmpeg_component=shared_library"
export PYTHONPATH="$PWD/build"
./tools/gyp/gyp -I build/common.gypi --depth=. ./third_party/ffmpeg/ffmpeg.gyp
```

**Step 4.** Build ffmpeg DLL

```bash
ninja -C out/Release ffmpeg
```

You will find the DLL in `out/Release` folder. The path and file name varies between platforms:

* Windows: `ffmpeg.dll`
* Mac OS X: `libffmpeg.dylib`
* Linux: `lib/libffmpeg.so`

**Step 5.** Replace the bundled DLL in prebuild NW.js with the one you just built. The path and file name varies between platforms:

* Windows: `ffmpeg.dll`
* Mac OS X: `nwjs.app/Contents/Versions/<chromium-version>/nwjs Framework.framework/libffmpeg.dylib`
* Linux `lib/libffmpeg.so`

### Build Entire NW.js with Proprietary Codecs

If you don't use official pre-built NW.js, you can build entire NW.js with proprietary codecs enabled by following the instructions below.

**Step 1.** Install prerequists and fetch the source code of NW.js. See *Prerequisits* and *Get the Code* sections in [Building NW.js](Building NW.js.md).

**Step 2.** Apply following patch to `third_party/ffmpeg/ffmpeg.gyp` to make `ffmpeg` include the codecs:
```diff
diff --git a/ffmpeg.gyp b/ffmpeg.gyp                   
index 294dd2e..7dfcd3a 100755                          
--- a/ffmpeg.gyp
+++ b/ffmpeg.gyp
@@ -72,7 +72,7 @@ 
       ['chromeos == 1', {
         'ffmpeg_branding%': '<(branding)OS',
       }, {  # otherwise, assume Chrome/Chromium.
-        'ffmpeg_branding%': '<(branding)',
+        'ffmpeg_branding%': 'Chrome'
       }],
     ],
```

**Step 3.** Regenerate the gyp files again with `gclient runhooks`.

**Step 4.** Rebuild NW.js with `ninja -C out/Release nwjs`. Then you will get NW.js as well as FFmpeg DLL with proprietary codecs. If you just want to rebuild the DLL, running `ninja -C out/Release ffmpeg` will do. The generated binaries can be found in `out/Release`.
