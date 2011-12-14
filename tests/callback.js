var nwebkit;
nwebkit = require('../lib');


function onload(err) {
    console.log("onload(), err: " + err);
}

nwebkit.init ({'url' : process.argv[2],
	       'width' : 800,
	       'height' : 600}, onload);
// nwebkit.init({'width' : 400, 'height' : 300});

// setTimeout (function() {
// 		console.log("open()");
// 		//nwebkit.open(process.argv[2]);
// 		//nwebkit.loadString("<html><head><title>testfs</title></head><body><p>hello</p></body></html>");
// 		//nwebkit.setViewMode(2);
// 	    }, 2000);

