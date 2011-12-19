Introduction
============
This is a NodeJS module for WebKit. With this module, your Javascript code in HTML can call NodeJS functions directly:

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

From NodeJS code, you can manipulate DOM or run Javascript in DOM context as well:

    var nwebkit;
    nwebkit = require('nwebkit');

    nwebkit.init ({'url' : "tests/testfs.html"},
	      function() {
		  nwebkit.context.runScript("document.body.style.background='#a3a3a3'");
    });


Dependencies
============

A patched version of WebKit (libwebkitnode) is need to make this possible. The source of libwebkitnode is at:

  https://github.com/rogerwang/WebKit

Prebuilt binaries for Debian and Ubuntu can be downloaded from:

Intallation
===========

* The following package is needed to build this:

    nodejs-dev libwebkitnode-dev

* Build:

    node-waf configure build

* Run sample:

    node tests/helloworld.js tests/testfs.html

How it works
============

* WebKitGtk's (glib based) main loop is merged with libev's main loop in NodeJS (Thanks to the code from node-gtk and EV::Glib project), so they can run in the same process.

* WebKit is patched so we can switch the Javascript engine from jsc to V8, and then the V8 context is merged (actually bridged) with the context from NodeJS. This makes NodeJS functions accessiable in web page. (Thanks to Nayan Kumar for the webkit-v8 work)