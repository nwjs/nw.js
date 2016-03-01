# Screen {: .doctitle}
---

[TOC]

`Screen` is an instance of EventEmitter object, and you're able to use `Screen.on(...)` to respond to native screen's events.

`Screen` is a singleton object, need to be initiated once by calling `nw.Screen.Init()`

## Synopsis

```javascript
//init must be called once during startup, before any function to nw.Screen can be called
nw.Screen.Init();

var screenCB = {
  onDisplayBoundsChanged: function(screen) {
    console.log('displayBoundsChanged', screen);
  },

  onDisplayAdded: function(screen) {
    console.log('displayAdded', screen);
  },

  onDisplayRemoved: function(screen) {
    console.log('displayRemoved', screen)
  }
};

// listen to screen events
nw.Screen.on('displayBoundsChanged', screenCB.onDisplayBoundsChanged);
nw.Screen.on('displayAdded', screenCB.onDisplayAdded);
nw.Screen.on('displayRemoved', screenCB.onDisplayRemoved);
```

## Screen.Init()

Init the Screen singleton object, you only need to call this once

## Screen.screens

Get the array of screen (number of screen connected to the computer)

screen has following structure:
```javascript
screen {
// unique id for a screen
  id: int,

// physical screen resolution, can be negative, not necessarily start from 0,depending on screen arrangement
  bounds: {
    x: int,
    y: int,
    width: int,
    height: int
  },
 
// useable area within the screen bound
  work_area: {
    x: int,
    y: int,
    width: int,
    height: int
  },

  scaleFactor: float,
  isBuiltIn: bool,
  rotation: int,
  touchSupport: int
}
```

## Screen.chooseDesktopMedia (sources, callback)

* `sources` `{String[]}` array of source types. Two types are supported by this API: `"screen"` and `"window"`.
* `callback` `{Function}` callback function with chosed streamId. streamId will be `false` if failed to execute or existing session is alive.

!!! note
    Screen sharing by selection; Currently only working in Windows and OSX and some linux distribution.

Example:

```javascript
nw.Screen.Init(); // you only need to call this once
nw.Screen.chooseDesktopMedia(["window","screen"], 
  function(streamId) {
    var vid_constraint = {
      mandatory: {
        chromeMediaSource: 'desktop', 
        chromeMediaSourceId: streamId, 
        maxWidth: 1920, 
        maxHeight: 1080
      }, 
      optional: []
    };
    navigator.webkitGetUserMedia({audio: false, video: constraint}, success_func, fallback_func);
  }
);
```

## Event: displayBoundsChanged(screen)

Emitted when the screen resolution, arrangement is changed, the callback is called with 1 argument `screen`. See [Screen.screens](#screenscreens) for the format.

## Event: displayAdded (screen)

Emitted when a new screen added, the callback is called with 1 argument `screen`. See [Screen.screens](#screenscreens) for the format.

## Event: displayRemoved (screen)

Emitted when existing screen removed, the callback is called with 1 argument `screen`. See [Screen.screens](#screenscreens) for the format.

## Screen.DesktopCaptureMonitor

This API behaves similar functions as `Screen.chooseDesktopMedia`. But it doesn't have GUI. You can use this API to monitor the changes of screens and windows on desktop and implement your own UI.

`Screen.DesktopCaptureMonitor` is an instance of `EventEmitter`. You can use `Screen.DesktopCaptureMonitor.on()` to listen to the events.

### Synopsis

```javascript
var dcm = nw.Screen.DesktopCaptureMonitor;
nw.Screen.Init();
dcm.on("added", function (id, name, order, type) {
   //select first stream and shutdown
   var constraints = {
      audio: {
         mandatory: {
             chromeMediaSource: "system",
             chromeMediaSourceId: dcm.registerStream(id)
          }
      },
      video: {
         mandatory: {
             chromeMediaSource: 'desktop',
             chromeMediaSourceId: dcm.registerStream(id)
         }
      }
  };

  // TODO: call getUserMedia with contraints

  dcm.stop();
});

dcm.on("removed", function (id) { });
dcm.on("orderchanged", function (id, new_order, old_order) { });
dcm.on("namechanged", function (id, name) { });
dcm.on("thumbnailchanged", function (id, thumbnail) { });
dcm.start(true, true);
```

### Screen.DesktopCaptureMonitor.started

Boolean of whether the DesktopCaptureMonitor is started.

### Screen.DesktopCaptureMonitor.start(should_include_screens, should_include_windows)

* `should_include_screens` `{Boolean}` whether should include screens
* `should_include_windows` `{Boolean}` whether should include windows

The `DesktopCaptureMonitor` will start monitoring the system and trigger the the events. The screen may flicker if while `DesktopCaptureMonitor` is running.

### Screen.DesktopCaptureMonitor.stop()

The `DesktopCaptureMonitor` will stop monitoring the system. `DesktopCaptureMonitor` should be stopped after a stream is selected.

### Screen.DesktopCaptureMonitor.registerStream(id)

Register and return a valid stream id passed into `chromeMediaSourceId` in `getUserMedia` constraints. See [Synopsis](#synopsis_1) for the usage.

### Event: added (id, name, order, type, primary)

!!! warning "Behavior Changed"
    This feature is changed in 0.13.0. See [Migration Notes from 0.12 to 0.13](../For Users/Migration/From 0.12 to 0.13.md).

* `id` `{String}` is the media id. Use `registerStream(id)` to obtain a valid stream id used with `getUserMedia()`. See [registerStream](#screendesktopcapturemonitorregisterstreamid)
* `name` `{String}` is the title of the window or screen
* `order` `{Integer}` is the z-order of the windows, if screens are selected they will appear first
* `type` `{String}` type of the stream: "screen", "window", "other" or "unknown"
* `primary` `{Boolean}` _Windows OS only_ this will be `true` if the source is the primary monitor

Emit when a new source was added.

### Event: removed (order)

* `order` `{Integer}` is the order of the media source that is no longer capturable

Emit when a source was removed.

### Event: orderchanged (id, new_order, old_order)

!!! warning "Behavior Changed"
    This feature is changed in 0.13.0. See [Migration Notes from 0.12 to 0.13](../For Users/Migration/From 0.12 to 0.13.md).

* `id` `{String}` is the media id of the screen or window that has changed z-order
* `new_order` `{Integer}` is the new z-order
* `old_order` `{Integer}` is the old z-order

Emit when the Z-order of a source changed (this may change for windows as others are focused).

### Event: namechanged (id, name)

!!! warning "Behavior Changed"
    This feature is changed in 0.13.0. See [Migration Notes from 0.12 to 0.13](../For Users/Migration/From 0.12 to 0.13.md).

* `id` `{String}` is the media id of the screen or window that has a name changed
* `name` `{String}` is the new new name of the screen or window

Emit when the name of the source has changed. This can happen when a window changes title.

### Event: thumbnailchanged (id, thumbnail)

!!! warning "Behavior Changed"
    This feature is changed in 0.13.0. See [Migration Notes from 0.12 to 0.13](../For Users/Migration/From 0.12 to 0.13.md).

* `id` `{String}` is the media id of the screen or window that has an updated thumbnail
* `thumbnail` `{String}` is the base64 encoded png of the thumbnail

Emit when the thumbnail of a source changed.
