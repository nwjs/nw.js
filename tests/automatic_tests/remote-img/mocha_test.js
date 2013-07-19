var path = require('path');
var spawn = require('child_process').spawn;
var fs = require('fs-extra');
describe('remote-file-access', function(){
	before(function(done){
		this.timeout(0);
		fs.exists('/tmp/', function (exists) {
			fs.copy(global.tests_dir+'/remote-img/star.jpg','/tmp/star.jpg');
		});
		done();
	})
	after(function() {
		this.timeout(0);
		fs.exists('/tmp/', function (exists) {
	    	fs.remove('/tmp/star.jpg');	
		})
	  })
	it ('remote img should work', function(done){
		this.timeout(0);
		var exec_argv = [path.join(global.tests_dir, 'remote-img/imgnotshown')]
		var result  =false;
		var app = spawn(process.execPath, exec_argv);
		app.on('exit', function(code){
			result = true;
			var url = "file:///tmp/star.jpg";
			var img  = new Image();
			img.src = url;
			if (img.complete)
				done();
			else
				done('the image is not shown');
		})
		setTimeout(function(){
			if(!result){
				app.kill();
				var url = "file:///tmp/star.jpg";
				var img  = new Image();
				img.src = url;
				if (img.complete)
					done();
				else
					done('the image is not shown');
				}
		}, 3000)
	})
	it ('local img should work', function(done){
		this.timeout(0);
		var exec_argv = [path.join(global.tests_dir, 'remote-img/imgshown')]
		var result = false;
		var app = spawn(process.execPath, exec_argv);
		app.on('exit', function(code){
			result = true;
			var url = "file:///tmp/star.jpg";
			var img  = new Image();
			img.src = url;
			if (img.complete)
				done();
			else
				done('the image is not shown');
		})
		setTimeout(function(){
			if(!result){
				app.kill();
				var url = "file:///tmp/star.jpg";
				var img  = new Image();
				img.src = url;
				if (img.complete)
					done();
				else
					done('the image is not shown');
				}
		}, 3000)
	})
})