var nwebkit;
nwebkit = require('../lib');


nwebkit.init ({'url' : "tests/testfs.html", 'width' : 800, 'height' : 600});
// nwebkit.init({'width' : 400, 'height' : 300});

setTimeout (function() {
 		console.log("open()");
		nwebkit.context.runScript("document.getElementById('output').innerHTML='changed'");
 	    }, 2000);

