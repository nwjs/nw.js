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

**Step 2.** Setup `GYP_DEFINES` to `proprietary_codecs=1` to turn on codecs support on Chromium side.

**Step 3.** Regenerate the gyp files again with `gclient runhooks`.

**Step 4.** Rebuild NW.js with `ninja -C out/Release nwjs`.
