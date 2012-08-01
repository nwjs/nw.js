Introduction
============

`node-webkit` is a web runtime based on `Chromium` and `node.js`. Though its
name suggests that is's module of `node.js`, it is in fact a standalone 
executable that runs your app written in HTML and node.js. And `node.js` is
a part of it that provides native APIs and a module system.

Downloads
=========

Binaries on all platforms will come soon.

How to use
==========

There're tree ways to run an app.

Run an app that is packaged:

````bash
$ nw 'path-to-package.nw'
````

Run an app that resides in a directory (mainly helps developing):

````bash
$ nw 'path-to-directory'
````

Explicitly run an url:

````bash
$ nw 'file://path-of-index-html'
````

How to compile
==============

Since this project relies heavily on `Chromium`, which is a quite large one,
our compilation will follow chromium's standard, by using `gclient` and
`gyp`.

Our steps will generally follow `Chromium`'s [Get the Code](http://www.chromium.org/developers/how-tos/get-the-code)
and `CEF`'s [BranchesAndBuilding](http://code.google.com/p/chromiumembedded/wiki/BranchesAndBuilding),
so make sure of reading them before you continue.

Prerequisite
------------

1. [Get the Chromium depot_tools](http://www.chromium.org/developers/how-tos/install-depot-tools).
2. Setup building enviroment.
   On Windows, see [Build Instructions (Windows)](http://www.chromium.org/developers/how-tos/build-instructions-windows#TOC-Build-environment).
   On Linux, see [LinuxBuildInstructionsPrerequisites](http://code.google.com/p/chromium/wiki/LinuxBuildInstructionsPrerequisites).
   On Mac, see [MacBuildInstructions](http://code.google.com/p/chromium/wiki/MacBuildInstructions#Prerequisites).

Get Chromium's code
-------------------

1. Find a place to put the code, it will take up about 14G disk place
after compilation. Assume you store code under `node-webkit` folder, our
final directory architecture will be like:

    node-webkit/
    |-- .gclient
    `-- src/
        |-- many-stuff
        |-- ...
        `-- cef

2. Then create `.gclient` file under `node-webkit`, its content is:

    solutions = [
       { "name"        : "src",
         "url"         : "https://github.com/zcbenz/chromium.git@dbc6308eb7",
         "deps_file"   : ".DEPS.git",
         "managed"     : True,
         "custom_deps" : {
           "src/third_party/WebKit/LayoutTests": None,
           "src/chrome_frame/tools/test/reference_build/chrome": None,
           "src/chrome_frame/tools/test/reference_build/chrome_win": None,
           "src/chrome/tools/test/reference_build/chrome": None,
           "src/chrome/tools/test/reference_build/chrome_linux": None,
           "src/chrome/tools/test/reference_build/chrome_mac": None,
           "src/chrome/tools/test/reference_build/chrome_win": None,
         },
         "safesync_url": "",
       },
    ]

3. Execute the syncing under `node-webkit` (it would spend a few hours
depending on your network condition):

````bash
$ gclient sync
````

Note: if you're on Linux and you get any dependency errors during gclient
sync (like 'nss' or 'gtk+-2.0'), run ./src/build/install-build-deps.sh,
then re-run gclient sync:

    cd /path/to/node-webkit
    # Do this to boot strap dependencies on Linux:
    gclient sync --nohooks
    ./src/build/install-build-deps.sh
    gclient sync

If you encountered other problems, see [UsingNewGit](http://code.google.com/p/chromium/wiki/UsingNewGit).

Get CEF's code
--------------

Currently our project just patches `CEF` and is independent of `Chromium`,
so you still need to put `CEF`'s code under `node-webkit/src`:

    cd /path/to/node-webkit/src
    git clone https://github.com/zcbenz/cef.git

Configure and compile
---------------------

1. Run the cef_create_projects script (.bat on Windows, .sh on OS-X and 
Linux) to generate the build files based on the GYP configuration.

    cd /path/to/node-webkit/src/cef
    ./cef_create_projects.sh

2. Build node-webkit

* Windows - Open the Visual Studio solution file and build. 
* Mac OS-X - Open the Xcode project file and build. 
* Linux - Run "make -j4 nw" from the Chromium "src" directory. 

Alternately, the build_projects script (.bat on Windows, .sh on OS-X
and Linux) can be used to build on the command line using the default
system tools. 

    cd /path/to/chromium/src/cef/tools
    build_projects.sh Debug

Run
---

The binary is generated under `out/Debug` folder, you can just execute it:

How node.js is integrated with Chromium
====================================

Currently `node.js` uses `libuv` as its message loop, and creates a lot of
`V8` objects to provide standard library. So our job needs just two steps:

1. Replace chromium's message loop with libuv
---------------------------------------------

Thanks to chromium's clean design of `MessageLoop` class, this step is 
relatively easy.

First we create `MessageLoopForUV` subclass in `base/message_loop.h`, which
uses `MessagePumpUV` as its message pump. Then we define `MessagePumpUV` in
`base/message_pump_uv.h` and `message_pump_uv.cc`, which uses `libuv` as 
its lowlevel events loop engine.

(Note the message loop under Mac is done by modifying `MessagePumpMac`, 
which would break in future)

Then we change `base/message_loop.cc:179` and 
`content/renderer/render_thread_impl.cc:195` to make `MessageLoop` actually
use our `MessagePumpUV`.

2. Initialize node.js
---------------------


