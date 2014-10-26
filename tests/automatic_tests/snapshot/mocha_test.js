var path = require('path');
var app_test = require('./nw_test_app');
var os = require('os');
var fs = require('fs-extra');
var cp = require('child_process');
var snapshotPath;

describe('snapshot', function() {

    describe('demo should work fine', function() {
	before(function(done) {
	    var snapshotExec;
	    if (os.platform() == 'darwin') {
		snapshotExec = '../../../../../../nwsnapshot';
	    }
	    if (os.platform() == 'linux') {
		snapshotExec = 'nwsnapshot';
	    }
	    if (os.platform() == 'win32') {
		snapshotExec = 'nwsnapshot.exe';
	    }

	    snapshotPath = path.join(process.execPath, '..', snapshotExec);
	    console.log("snapshotPath: " + snapshotPath);

	    cp.execFile(snapshotPath,
			['--extra_code', 'mytest.js', 'mytest.bin'],
			{cwd:'./' + global.tests_dir + '/snapshot/'},
			function (error, stdout, stderr) {
			    done();
			}

		       );
	})

	after(function() {
	    fs.unlink(path.join(global.tests_dir, 'snapshot', 'mytest.bin'), function(err) {if(err && err.code !== 'ENOENT') throw err});
	    fs.unlink(path.join(global.tests_dir, 'snapshot', 'v8.log'), function(err) {if(err && err.code !== 'ENOENT') throw err});
	})

	it('the native code could be exectuted',
	   function(done) {
               this.timeout(0);
               var result = false;

               var child = app_test.createChildProcess({
		   execPath: process.execPath,
		   appPath: path.join(global.tests_dir, 'snapshot'),
		   end: function(data, app) {
		       done();
		       app.kill();
		       result = true;
		   }
               });

               setTimeout(function(){
		   if (!result) {
		       done('the native code does not been executed');
		       child.close();
		       //child.removeConnection();
		       //child.app.kill();
		   }
               }, 3000);
               //child.app.stderr.on('data', function(d){ console.log ('app' + d);});

	   })
    })

    describe('1266-snapshot-crash-start', function() {
    	before(function(done) {
    	    cp.execFile(snapshotPath,
    			['--extra_code', 'file_to_snapshot_to_app.bin.js', 'app.bin'],
    			{cwd:'./' + global.tests_dir + '/snapshot/1266-snapshot-crash-start/'},
    			function (error, stdout, stderr) {
    			    done();
    			}
			
    		       );
    	})
	
    	after(function() {
    	    fs.unlink(path.join(global.tests_dir, 'snapshot','1266-snapshot-crash-start', 'app.bin'), function(err) {if(err && err.code !== 'ENOENT') throw err});
    	    fs.unlink(path.join(global.tests_dir, 'snapshot','1266-snapshot-crash-start', 'v8.log'), function(err) {if(err && err.code !== 'ENOENT') throw err});
    	    fs.unlink(path.join(global.tests_dir, 'snapshot','1266-snapshot-crash-start','tmp'), function(err) {if(err && err.code !== 'ENOENT') throw err});
    	})
	
    	it('another demo should close nomally', function(done) {
    	    this.timeout(0);
    	    var ppath = process.execPath + " " + path.join(global.tests_dir, 'snapshot', '1266-snapshot-crash-start');
    	    cp.exec(ppath, function(err, stdout, stderr) {
    	    });
    	    setTimeout(function(){
		fs.exists(path.join(global.tests_dir, 'snapshot','1266-snapshot-crash-start', 'tmp'), function(exists) {
		    if (exists)
			done();
		    else
			done('another demo fails');
		});
    	    }, 3000);
    	  });
    });


})
