var path = require('path');
var os = require('os');
var cp = require('child_process');
var assert = require('assert');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');

var func = require(path.join(curDir, '..', 'start_app', 'script.js'));
var execPath = func.getExecPath();


var mac_app_path = path.join(curDir, 'tmp-nw', 'node-webkit.app');

var temp_path;

if (os.platform() == 'win32') {
    execPath = path.join(curDir, 'tmp-nw', 'app.exe');
} else if (os.platform() == 'linux') {
    execPath = path.join(curDir, 'tmp-nw', 'app');
} else if (os.platform() == 'darwin') {
    execPath = path.join(curDir, 'tmp-nw', 'node-webkit.app', 'Contents', 'MacOS', 'node-webkit');
}

function make_execuable_file(folder_path, done) {
    func.copyExecFiles(function() {
	func.copySourceFiles(folder_path); 
	func.zipSourceFiles(function() {
	    func.makeExecuableFile();
	    if (os.platform() == 'darwin') {
		var app_path = 'tmp-nw/node-webkit.app/Contents/Resources/app.nw';
		fs.mkdir(app_path, function(err) {
		    if(err && err.code !== 'EEXIST') throw err
		    fs.copy('tmp-nw/index.html', path.join(app_path, 'index.html'));
		    fs.copy('tmp-nw/package.html', path.join(app_path, 'package.html'));
		    setTimeout(done, 3000);

		});
	    } else {
		setTimeout(function() {
		    var child = cp.spawn(execPath, curDir); // [path.join(curDir, 'internal')]);
		    child.on('exit', function() {
			temp_path = path.dirname(fs.readFileSync(path.join(curDir, 'tmp-nw','path.org'))).substring(8);
			done();
		    });
		}, 3000);
	    }
	});
    });
}

describe('temp_dir', function() { 
    this.timeout(0);

    before(function(done) {
	make_execuable_file('internal', done);
    });

    after(function() {
    	fs.remove('tmp-nw', function (er) {
            if (er) throw er;
        });
    });

    it('should be removed after app exit', function() {
	var exist = fs.existsSync(temp_path);
	assert.equal(exist, false);
    });
});


