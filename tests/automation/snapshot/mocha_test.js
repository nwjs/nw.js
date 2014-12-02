var path = require('path');
var os = require('os');
var fs = require('fs-extra');
var cp = require('child_process');
var curDir = fs.realpathSync('.');

var myTestJSPath = path.join(curDir, 'internal', 'mytest.js');
var myTestBinPath = path.join(curDir, 'internal', 'mytest.bin');

var snapshotPath;
var server;

describe('snapshot', function() {

/////////// 1

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
			['--extra_code', myTestJSPath, myTestBinPath],
			{cwd: curDir},
			function (error, stdout, stderr) {

                server = createTCPServer(13013);

			    done();
			}
        );

	});

	after(function() {
	    fs.unlink(myTestBinPath, function(err) {if(err && err.code !== 'ENOENT') throw err});
	    fs.unlink(path.join(curDir, 'internal', 'v8.log'), function(err) {if(err && err.code !== 'ENOENT') throw err});
        server.close();
	});

	it('the native code could be exectuted',
	   function(done) {
               this.timeout(0);
               var result = false;


            var child = spawnChildProcess(path.join(curDir, 'internal'));
    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        result = true;
        child.kill();
        done();
      });
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

	   });
    });


////////////////// 2


    describe('1266-snapshot-crash-start', function() {
    	before(function(done) {
    	    cp.execFile(snapshotPath,
    			['--extra_code', 'file_to_snapshot_to_app.bin.js', 'app.bin'],
    			{cwd: path.join(curDir, 'internal', '1266-snapshot-crash-start')},
    			function (error, stdout, stderr) {
    			    done();
    			}
			
    		       );
    	});
	
    	after(function() {
    	    fs.unlink(path.join(curDir, 'internal','1266-snapshot-crash-start', 'app.bin'), function(err) {if(err && err.code !== 'ENOENT') throw err});
    	    fs.unlink(path.join(curDir, 'internal','1266-snapshot-crash-start', 'v8.log'), function(err) {if(err && err.code !== 'ENOENT') throw err});
    	    fs.unlink(path.join(curDir, 'internal','1266-snapshot-crash-start','tmp'), function(err) {if(err && err.code !== 'ENOENT') throw err});
    	});
	
    	it('another demo should close nomally', function(done) {
    	    this.timeout(0);
    	    var ppath = process.execPath + " " + path.join(curDir, 'internal', '1266-snapshot-crash-start');
    	    cp.exec(ppath, function(err, stdout, stderr) {
    	    });
    	    setTimeout(function(){
		fs.exists(path.join(curDir, 'internal', '1266-snapshot-crash-start', 'tmp'), function(exists) {
		    if (exists)
			done();
		    else
			done('another demo fails');
		});
    	    }, 3000);
    	  });
    });



});


