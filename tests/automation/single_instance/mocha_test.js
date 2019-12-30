var path = require('path');
var os = require('os');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');

var func = require(path.join(curDir, '..', 'start_app', 'script.js'));
var execPath = func.getExecPath();

var spawn = require('child_process').spawn;
var exec = require('child_process').exec;

var app =  new Array();
var child1, child2;
var mac_app_path = path.join(curDir, 'tmp-nw', 'node-webkit.app');

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

/////// 1 
    describe('single-instance false', function() {
	before(function(done) {
	    make_execuable_file(path.join(curDir, 'mul'), done);
	});

        after(function() {
	    fs.remove(path.join(curDir, 'tmp-nw'), function (er) {
                    if (er) throw er;
            });
        });

	it('should have a instance', function(done) {
	    check_have(0, execPath, "", 'not have a instance', 0, done);
	});

	it('should have a second instance', function(done) {
	    check_have(1, execPath, "", 'not have a second instance', 1, done);
	});
    });

//////////////// 2
    describe('single-instance default', function() {

	before(function(done) {
	    setTimeout(function() {
		make_execuable_file(path.join(curDir, 'single'), done);
	    }, 3000);
	});

        after(function() {
	    fs.remove(path.join(curDir, 'tmp-nw'), function (er) {
                    if (er) throw er;
            });
        });

	it('should have a instance', function(done) {
	    check_have(2, execPath, "", 'not have a instance', 0, done);
	});

	it('should not have a second instance', function(done) {
	    check_have(3, execPath, "", 'have a second instance', 2, done);
	});
    });

/////////////////////// 3
    if (os.platform() == 'darwin') {
        describe('single-instance false(open app)', function(){
                before(function(done) {
                        setTimeout(function() {
                                make_execuable_file('single_instance/open_mul', done);
                            }, 3000);
                });

                after(function() {
                        fs.remove('tmp-nw', function (er) {
                                if (er) throw er;
                            });
                });

                it('should have a instance (open app)', function(done) {
                        child1 = exec('open ' + mac_app_path);
                        setTimeout(function () {
                                var content = fs.readFileSync('tmp-nw/msg');
                                if (content + "" != "")
                                    done();
                                else
                                    done("not have a instance");
                            }, 6000);
                });
                
                it('should not have a second instance (open app)', function(done) {
                        child2 = exec('open ' + mac_app_path);
                        var content = fs.readFileSync('tmp-nw/msg');
                        if (content + "" == "11")
                            done("have a second instance");
                        else
                            done();
                });

        });

        describe('single-instance default(open app)', function(){
                before(function(done) {
                        setTimeout(function() {
                                make_execuable_file('single_instance/open_single', done);
                            }, 3000);
                });

                after(function() {
                        fs.remove('tmp-nw', function (er) {
                                if (er) throw er;
                            });
                });

                it('should have a instance (open app)', function(done) {
                        child1 = exec('open ' + mac_app_path);
                        setTimeout(function () {
                                var content = fs.readFileSync('tmp-nw/msg_s');
                                if (content + "" != "")
                                    done();
                                else
                                    done("not have a instance");
                            }, 6000);
                 });
                
                it('should not have a second instance (open app)', function(done) {
                        child2 = exec('open ' + mac_app_path);
                        var content = fs.readFileSync('tmp-nw/msg_s');
                        if (content + "" == "11")
                            done("have a second instance");
                        else
                            done();

                });
	    });
	}
});


