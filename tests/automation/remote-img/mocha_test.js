var path = require('path');
var spawn = require('child_process').spawn;
var fs = require('fs-extra');
var os = require('os');
var platform = os.platform();
var curDir = fs.realpathSync('.');

var result, app, exec_argv, timerId;

process.on('exit', function() {
  if (app) {
    app.kill('SIGKILL');
  }
});

describe('remote-file-access', function(){

	before(function(done){
		this.timeout(0);
		if(platform == "win32"){
			fs.copySync(path.join(curDir, 'star.jpg'),'/star.jpg');
		}
		else {
			fs.copySync(path.join(curDir, 'star.jpg'),'/tmp/star.jpg');
		}
		done();
	});

	after(function(done) {
		this.timeout(0);
		if (platform == "win32"){
			fs.unlinkSync('/star.jpg');
		}
		else {
			fs.unlinkSync('/tmp/star.jpg');
		}
        done();
	  });


	it ('remote img should work', function(done){
		this.timeout(0);
		exec_argv = [path.join(curDir, 'imgnotshown')];
		result  =false;
		app = spawn(process.execPath, exec_argv);
		app.on('exit', function(code){

            clearTimeout(timerId);
			result = true;
            app.kill('SIGKILL');
            app = undefined;
			done();
		});

		timerId = setTimeout(function(){
			if(!result){
                app.removeAllListeners('exit');
				app.kill('SIGKILL');
                app = undefined;
				done();
			}
		}, 2000);
	});


	it ('local img should work', function(done){
		this.timeout(0);
		exec_argv = [path.join(curDir, 'imgshown')];
		result = false;
		app = spawn(process.execPath, exec_argv);
		app.on('exit', function(code){
            clearTimeout(timerId);
			result = true;
            app.kill('SIGKILL');
            app = undefined;
			done();
		});

		timerId=setTimeout(function(){
			if(!result){
                app.removeAllListeners('exit');
				app.kill('SIGKILL');
                app = undefined;
				done();
		    }
		}, 2000);

	});


});


