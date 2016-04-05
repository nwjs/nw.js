# Command Line Options {: .doctitle}
---

[TOC]

You use following command line options when starting NW.js to change some default behaviors.

## `--mixed-context`

Run NW.js in [Mixed Context Mode](../For Users/Advanced/JavaScript Contexts in NW.js.md#mixed-context-mode) instead of Separate Context Mode.

## `--nwapp`

An alternative way to specify the path of the app. This parameter is especially useful when [testing with ChromeDriver](../For Users/Advanced/Test with ChromeDriver.md).

## `--user-data-dir`

Specify the data directory for your app, which contains the stored data, caches and crash dumps etc. By default the data directory is in following directories according to your platform:

* Windows: `%LOCALAPPDATA%/<name-in-manifest>/`
* Mac: `~/Library/Application Support/<name-in-manifest>/`
* Linux: `~/.config/<name-in-manifest>`

Where `<name-in-manifest>` is the [`name` field in manifest](Manifest Format.md#name).

## `--disable-raf-throttling`

When it's used, requestAnimationFrame() callback will continue to fire when window is minimized or hidden. It's pretty useful for game developer. When the argument is not used, it behaves in the same way as Chrome browser and has no side effects.

## `--enable-gcm`

Enable the chrome.gcm API.

## `--enable-transparent-visuals`
## `--disable-transparency`
## `--disable-gpu`
## `--force-cpu-draw`

These options are related to the transparent window feature. See [Transparent Window](../For Users/Advanced/Transparent Window.md) for details.

## Other Chromium Options

You can also use Chromium options listed in https://github.com/nwjs/chromium.src/blob/nw13/chrome/common/chrome_switches.cc and https://github.com/nwjs/chromium.src/blob/nw13/content/public/common/content_switches.cc . See also http://peter.sh/experiments/chromium-command-line-switches/

These options can be put into [`chromium-args` in manifest](Manifest Format.md#chromium-args) to get NW.js running with them each time.