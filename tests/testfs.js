var fs = require('fs');

function test_fs() {
var output = document.getElementById ('output');
output.innerHTML = '';
fs.readdir(".", function (err, files) {
           var result = '';
	       files.forEach(function (filename) {
				 //console.log(filename);
				  result += filename + '<br/>';
				 }
			    );
          output.innerHTML = result;
                });
}
