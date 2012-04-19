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

A patched version of CEF (Chromium Embedded Framework) is needed to make this possible. Prebuilt binaries are available for Linux (Debian and Ubuntu), Windows and OSX.

The source of Chromium is at https://github.com/rogerwang/chromium

Other hacks on the dependencies are:

https://github.com/rogerwang/node
https://github.com/rogerwang/libuv


Build from source
=================


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
