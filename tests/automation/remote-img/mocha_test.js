var path = require('path');
var spawn = require('child_process').spawn;
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');

describe('remote-file-access', function(){
	var os = require('os');
	var platform = os.platform();

	before(function(done){
		this.timeout(0);
		if(platform == "win32"){
			fs.copy(path.join(curDir, 'star.jpg'),'/star.jpg');
		}
		else {
			fs.copy(path.join(curDir, 'star.jpg'),'/tmp/star.jpg');
		}
		done();
	});

	after(function() {
		this.timeout(0);
		if (platform == "win32"){
			fs.remove('/star.jpg');
		}
		else {
			fs.remove('/tmp/star.jpg');
		}
	  });

	it ('remote img should work', function(done){
		this.timeout(0);
		var exec_argv = [path.join(curDir, 'imgnotshown')]
		var result  =false;
		var app = spawn(process.execPath, exec_argv);
		app.on('exit', function(code){
			result = true;
			done();
		})
		setTimeout(function(){
			if(!result){
				app.kill();
				done();
			}
		}, 3000)
	});

	it ('local img should work', function(done){
		this.timeout(0);
		var exec_argv = [path.join(curDir, 'imgshown')]
		var result = false;
		var app = spawn(process.execPath, exec_argv);
		app.on('exit', function(code){
			result = true;
			done();
		});

		setTimeout(function(){
			if(!result){
				app.kill();
				done();
				}
		}, 3000);

	});

});


