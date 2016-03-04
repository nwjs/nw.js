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

In recent versions of Chromium project, ffmpeg DLL was changed to be builtin. And we don't have license to redistribute the codec in question. So developers don't have any ways to get the codec without compiling. To make things easier, we build ffmpeg as a separate Dll. The Dll we distribute will not contain any proprietary codecs in question. Developers can recompile the Dll for themselves, which is much easier than compiling NW. Or they can get Dll from someone else [from the community](https://github.com/nwjs/nw.js/issues/4492). Note that developers can redistribute binaries containing proprietary codecs as long as they have the license. If a developer doesn't have the license, using this tip or moving to other solutions doesn't make him/her eligible to redistribute the codecs.

In order to use MP3 and H.264, you'll need to compile ffmpeg with patch and corresponding options.

**Step 1.** Apply following patch to `third_party/ffmpeg/ffmpeg.gyp` to make `ffmpeg` include the codecs:
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

**Step 2.** Regenerate the gyp files again with `gclient runhooks`.

**Step 3.** Rebuild ffmpeg Dll with `ninja -C out/Release ffmpeg`.
