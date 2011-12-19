var nwebkit;
nwebkit = require('../lib');

nwebkit.init ({'url' : "tests/testfs.html"},
	      function() {
		  nwebkit.context.runScript("document.body.style.background='#a3a3a3'");
});


