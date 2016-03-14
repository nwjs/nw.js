# Transparent Window {: .doctitle}
---

[TOC]

## Basic Requirements

The transparent feature is supposed to work with frameless window.

### Windows

The transparent feature is only supported on Vista and above with DWM (Desktop Window Manager) enabled. Transparency might not work on classic theme / basic version of the OS, or using remote desktop.

### Linux

You need to run NW.js with these params and your window manager needs to support **compositing**:
```params
--enable-transparent-visuals --disable-gpu
```

## Make a Transparent Window

In the HTML body, specify the alpha of the background colour:
```html
<body style="background-color:rgba(0,0,0,0);">
```

and specify [`transparent` field](../../References/Manifest Format.md#transparent) to `true` in manifest:
```json
  "window": {
    "frame": false,
    "transparent": true
  }
```

## Click Through (Windows and Mac)

You can enable transparency clickthrough on Windows and Mac. This feature enables you to click the object under the window **at the point whose alpha value is `0`**.

To enable transparency clickthrough, you need following command line options:
```params
--disable-gpu --force-cpu-draw
```

!!! note
    The click through is only supported for **frameless**, **non resizable** frame, though it might work for other configuration depending on the OS.
