# Command Line Options {: .doctitle}
---

[TOC]

You use following command line options when starting NW.js to change some default behaviors.

## About command line arguments

When a user uses your app to open a file under command line, like `your-app file.txt file2.txt`, the `file.txt file2.txt` will be recorded and you can use [nw.App.argv](App.md) to get the array of command line arguments.  If there is already one running instance of your app, the full command line of the second instance will be passed to the existing instance by sending `open` event to the `App` object.

!!! note
    For now there is a limitation: if the command line argument is the filename of .js file on disk, NW will just run it as upstream Node.js. This is for supporting `child_process.fork()` API.

## `--url`

Load URL with the default app: `--url=http://nwjs.io`

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

## `--disable-devtools`

Disable user's access to devtools feature in SDK build.

## `--enable-node-worker`

Enable the Node.js integration in Web Workers. This will help you offload CPU hogging tasks with new threads, while [exchange large amount of data](https://developer.mozilla.org/en-US/docs/Web/API/Worker/postMessage) with DOM efficiently with the [structured clone](https://developer.mozilla.org/en-US/docs/Web/API/Web_Workers_API/Structured_clone_algorithm) algorithm.

Please note that binary modules of Node.js need to be thread-safe to be used in this way. We've made modifications to Node.js core to make sure core APIs are thread-safe, while we can not promise this with 3rd party binary modules. Pure JS modules are thread safe as long as they depends on thread-safe modules only. And there shouldn't be any side effect when the feature is not turned on.

## `--enable-features=NWESM`

Node.js and Chromium uses different ECMAScript module resolvers. To support loading ECMAScript modules in Node, by adding the switch, those modules can be loaded in Node context via 'import'. You could add your JS script to `node-main` to use this feature since 0.98.2.

Besides that, ES modules from Node can also be loaded directly in DOM contexts just like calling `require()`. The Node.js module resolver and the one from Browser are chained together to achieve this. To turn on this feature, use `--enable-features=NWChainImportNode` (under which the Node resolver is called first) or `--enable-features=NWChainImportDom`. These 2 flags should be used together with NWESM e.g. `--enable-features=NWESM,NWChainImportNode`. 

## `--disable-raf-throttling`

When it's used, requestAnimationFrame() callback will continue to fire when window is minimized or hidden. It's pretty useful for game developer. When the argument is not used, it behaves in the same way as Chrome browser and has no side effects.

## `--disable-cookie-encryption`

By default, the cookie store on disk is encrypted in Chromium. Use this switch to disable the encryption for testing purpose (e.g. sharing the cookie store between different systems).

## `--disable-crash-handler=true`

Disable the crash handler process for single process mode. NOTE: you should explicitly set it to 'true'. When using it and `--single-process` together, there will be only one NW process for your application. The crash dumping feature will be disabled with this switch. This switch must be put in the command line (not in package.json) to be effective.

## `--enable-gcm`

Enable the chrome.gcm API.

## `--enable-transparent-visuals`
## `--disable-transparency`
## `--disable-gpu`
## `--force-cpu-draw`

These options are related to the transparent window feature. See [Transparent Window](../For Users/Advanced/Transparent Window.md) for details.

## Other Chromium Options

You can also use Chromium options listed in https://github.com/nwjs/chromium.src/blob/nw18/chrome/common/chrome_switches.cc and https://github.com/nwjs/chromium.src/blob/nw18/content/public/common/content_switches.cc . See also http://peter.sh/experiments/chromium-command-line-switches/

These options can be put into [`chromium-args` in manifest](Manifest Format.md#chromium-args) to get NW.js running with them each time.

# Node.js Options

!!! note
    The arguments in this section are added for Node.js. They should be in `node-main` field in `package.json`. They are not effective on Chromium and the command line.

## `--nw-node-inspector`

Enable the V8 inspector in Node. Note that this will replace the registered V8 inspector in DOM from Blink, which will in turn breaks console.log in DOM since it relies on the inspector. You should consider use the console from Node instead as a workaround.

# Environment Variables

`NW_PRE_ARGS`'s value is prepended to the value of `chromium-args` in manifest.