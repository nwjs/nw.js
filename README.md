Introduction
============
This is a NodeJS module for WebKit. With this module, your Javascript code in the DOM context can call NodeJS functions directly. The following demo lists the current directory on the page:

    <html><head>
    <title>testfs</title>
    <script>
    var fs = require('fs');
    
    function test_fs() {
      var output = document.getElementById ('output');
      output.innerHTML = '';
      fs.readdir(".", function (err, files) {
        var result = '';
        files.forEach(function (filename) { result += filename + '<br/>'; } );
        output.innerHTML = result;
      });
    }
    </script></head>
    <body onload="test_fs()">
    <p id="output"></p>
    </body>
    </html>

From NodeJS code, you can manipulate the DOM or run Javascript in the DOM context as well:

    var nwebkit;
    nwebkit = require('nwebkit');

    nwebkit.init ({'url' : "tests/testfs.html"},
	      function() {
		  nwebkit.context.runScript("document.body.style.background='#a3a3a3'");
    });


Dependencies
============

A patched version of CEF (Chromium Embedded Framework) is needed to make this possible. Prebuilt binaries will be available for Linux (Debian and Ubuntu), Windows and OSX.

The source of Chromium is at https://github.com/rogerwang/chromium

Other hacks on the dependencies are in https://github.com/rogerwang/ , e.g.:

https://github.com/rogerwang/node

https://github.com/rogerwang/libuv

The dependencies are managed by .DEPS.git in the 'chromium' repository.

Build from source
=================

Since we are a patch on CEF, the build process is very similiar with CEF's. So it's recommended to read CEF and Chromium's build documentation first. Then download and install 'depot_tools'.

The difference is that we are using our own (git) repositories for some projects needed. So in the next step you need a different .gclient file to download and sync the code:

    solutions = [
      { "name"        : "src",
        "url"         : "git://github.com/rogerwang/chromium.git@node",
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

and then download 'cef' repository under the 'src' directory from:

    git://github.com/rogerwang/cef.git

Then execute 'cef_create_projects.{bat|sh}' in the 'cef' directory.

At last you should be able to build it with make or Visual Studio as documented in the CEF project.

If the build is successful then try to run 'cefclient /path/to/testfs.html'.

Good luck!

How it works
============

* Chromium's (render process) main loop is merged with libev's main loop in NodeJS, so they can run in the same process.

* WebKit is patched so that the V8 context is merged (actually bridged) with the context from NodeJS. This makes NodeJS functions accessible in a web page.

Community
=========

You are welcome to join the discussion on our mailing list:

node-webkit@googlegroups.com

http://groups.google.com/group/node-webkit

subscribe via node-webkit+subscribe@googlegroups.com

Our vision
==========

We believe that the async I/O framework in NodeJS and the Javascript programming language is a perfect combination for client (mobile) side applications. By bringing WebKit to NodeJS, a new way is available to write applications with web technology. We'll continue our development and support for this runtime. More platform and applications will be enabled.

License
=======

MIT License
