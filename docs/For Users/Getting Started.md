# Getting Started

---

## Installation

Download the latest NW.js binary in your Node projects:

```shell
npm install nw
```

Check out the [`npm-installer`](https://github.com/nwjs/npm-installer) to learn about it's configuration options. You can also download the binary directly from the official website [here](https://dl.nwjs.io/).

## Write your first NW.js application

Create a `package.json` with a `name` and `main` property. The `main` property references the first page opened by NW.js. This can be an HTML or JavaScript file.

Here’s an example on how to use an HTMl file as the entry point:

```json
{
  "name": "nw-demo",
  "main": "./index.html"
}
```

```html
<!DOCTYPE html>
<html>
  <head>
    <title>Hello World!</title>
  </head>
  <script>
    // Call a Node module
    const os = require("os");
    document.write("You are running on ", os.platform());

    // Call the NW API
    const nwWindow = nw.Window.get();
    nwWindow.showDevTools();

    // You can also use any third party Node modules installed by you
  </script>
  <body>
    <h1>Hello World!</h1>
  </body>
</html>
```

Here’s an example on how to use a JavaScript file as the entry point:

```json
{
  "name": "nw-demo",
  "main": "./main.js"
}
```

```javascript
nw.Window.open("./index.html", {}, () => {});
```

More information can be found in the [API section](../References/App.md).

!!! note "Native Node Modules"
    Native Node modules, built when running `npm install`, are not compatible with NW.js ABI. To use them, you have to rebuild it from source code with [`nw-gyp`](https://github.com/nwjs/nw-gyp). See [Use Native Node Modules](Advanced/Use Native Node Modules.md) for details.

## Run your first NW.js application

To run your application, pass your app's path to the NW runtime. If you downloaded NW directly from the website execute `/path/to/nw /path/to/app`. The exact path is different for different platforms being `nw` on Linux, `nwjs.app/Contents/MacOS/nwjs` on MacOS and `nw.exe` on Windows. If you downloaded NW.js from `npm`, add it as a script:

```json
{
  "scripts": {
    "nw": "nw ."
  }
}
```

The above command executes if your `package.json` is in the above mentioned file path and it has the `main` property pointing to the entry point of the NW application. To learn more about how to use NW.js check out the [API section](../References/App.md).

## Getting Help

There are lots of useful information on [NW.js wiki](https://github.com/nwjs/nw.js/wiki). The wiki is also open for everyone, and you are encouraged to publish your knowledge on wiki.
