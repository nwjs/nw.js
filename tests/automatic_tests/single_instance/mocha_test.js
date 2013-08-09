var func = require('./' + global.tests_dir +'/start_app/script.js');
var execPath = func.getExecPath();
var fs = require('fs-extra');
var os = require('os');
var spawn = require('child_process').spawn;

var app =  new Array();

var mac_app_path = path.join('tmp-nw', 'node-webkit.app')

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
		});
	    }
	    done();
	});
    });
}

function check_have(i, cmd, options, msg, last, done) {
    var result = false;
    app[i] = spawn(cmd, options);
    app[i].on('exit', function() {
	result = true;
    });
    
    if (last == 2) {
	setTimeout(function() {
	    if (result) {
		done();
	    } else {
		done(msg);
	    }
	    app[i].kill();
	    app[i - 1].kill();
	}, 3000);
    } else {
	setTimeout(function() {
	    if (result) {
		done(msg);
	    } else {
		done();
	    }
	    if (last == 1) {
		app[i].kill();
		app[i - 1].kill();
	    }
	}, 3000);
    }
}

describe('single-instance', function() { 
    this.timeout(0);

    describe('single-instance false', function() {
	before(function(done) {
	    make_execuable_file('single_instance/mul', done);
	});

	it('should have a instance', function(done) {
	    check_have(0, execPath, "", 'not have a instance', 0, done);
	});

	if (os.platform() == 'darwin') {
	    it('should have a instance (open app)', function(done) {
		check_have(4, 'open', [mac_app_path], 'not have a instance', 0, done);
	    });
	}

	it('should have a second instance', function(done) {
	    check_have(1, execPath, "", 'not have a second instance', 1, done);
	});

	if (os.platform() == 'darwin') {
	    it('should have a second instance (open app)', function(done) {
		check_have(5, 'open', [mac_app_path], 'not have a second instance', 1, done);
	    });
	}
    })
    

    describe('single-instance default', function() {

	before(function(done) {
	    make_execuable_file('single_instance/single', done);
	});

	after(function() {
	    setTimeout(function() {
		fs.remove('tmp-nw', function (er) {
		    if (er) throw er;
		})
	    }, 1000);
	});

	it('should have a instance', function(done) {
	    check_have(2, execPath, "", 'not have a instance', 0, done);
	});

	if (os.platform() == 'darwin') {
	    it('should have a instance (open app)', function(done) {
		check_have(6, 'open', [mac_app_path], 'not have a instance', 0, done);
	    });
	}

	it('should not have a second instance', function(done) {
	    check_have(3, execPath, "", 'have a second instance', 2, done);
	});

	if (os.platform() == 'darwin') {
	    it('should not have a second instance (open app)', function(done) {
		check_have(7, 'open', [mac_app_path], 'have a second instance', 2, done);
	    });
	}
    });
});
