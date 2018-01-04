# Protect JavaScript Source Code {: doctitle}
---

[TOC]

The JavaScript source code of your application can be protected by compiling to native code and loaded by NW.js. You only have to distribute the compiled code with your app for production.

### Compilation

JS source code is compiled to native code with the tool `nwjc` , which is provided in the SDK build.

To use it:
```bash
nwjc source.js binary.bin
```

The `*.bin` file is needed to be distributed with your application. You can name it whatever you want.

If you are compiling a module, `--nw-module` argument is needed.

### Load the Compiled JavaScript

```javascript
nw.Window.get().evalNWBin(frame, 'binary.bin');
```
The arguments of the [win.evalNWBin()](../../References/Window.md#winevalnwbinframe-path) method are similar with the `Window.eval()` method, where the first parameter is the target iframe (`null` for main frame), and the 2nd parameter is the binary code file.

If you are loading binary for modules, [win.evalNWBinModule()](../../References/Window.md#winevalnwbinmoduleframe-path-module_path) should be used instead.

### Load Compiled JavaScript from Remote

Compiled JavaScript can be fetched from remote (e.g. with AJAX) and executed on the fly.

```javascript
var xhr = new XMLHttpRequest();
xhr.responseType = 'arraybuffer'; // make response as ArrayBuffer
xhr.open('GET', url, true);
xhr.send();
xhr.onload = () => {
  // xhr.response contains compiled JavaScript as ArrayBuffer
  nw.Window.get().evalNWBin(null, xhr.response);
}
```

!!! note
    The compiled code is executed in [Browser Context](JavaScript Contexts in NW.js.md#browser-context). You can use any Web APIs (such as DOM) and [access NW.js API and Node API](JavaScript Contexts in NW.js.md#access-nodejs-and-nwjs-api-in-browser-context) like other scripts running in browser context.

### Use in Web Workers

A function `importNWBin(ArrayBuffer)` is introduced in worker context. The binary file can be read to array buffer in the main thread, posted to worker and then executed there with the new function.

### Known Issues

Before 0.22, the compiled code runs **slower than normal JS**: ~30% performance according to v8bench. Other non-compiled JS source code will not be affected. In 0.22.0-beta1, this issue has been fixed. Please check our blog post: https://nwjs.io/blog/js-src-protect-perf/

The compiled code is **not cross-platform nor compatible between versions** of NW.js. So you'll need to run `nwjc` for each of the platforms when you package your application.
