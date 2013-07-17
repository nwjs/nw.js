var path = require('path');
var app_test = require('./nw_test_app');
var exec = require('child_process').exec;
var pid1, pid2, pid3, pid4;
var pid1_exist, pid2_exist, pid3_exist, pid4_exist;
var cmd;
var child1, child2, child3, child4;

if (process.platform == 'linux')
    cmd = 'ps -a | grep ';
else if (process.platform == 'win32')
    cmd = 'tasklist | findstr ';
else if (process.platform == 'darwin')
    return;

describe('single-instance', function() {
    this.timeout(0);

    describe('single-instance false', function() {
    	var app_mul_path = path.join(global.tests_dir, 'single_instance', process.platform, 'app_mul');
    	before(function() {
    	    child3 = exec(app_mul_path);
    	    pid3 = child3.pid;
    	    console.log(pid3);

	    // normally we won't launch app one after another immediately
	    setTimeout(function(){
    		child4 = exec(app_mul_path);
    		pid4 = child4.pid;
    		console.log(pid4);
	    },1000);
    	});

    	it('should have a instance', function(done) {
    	    child = exec(cmd + pid3, function (error, stdout, stderr) {
    		if (stdout == "")
    		    pid3_exist = false;
    		else
    		    pid3_exist = true;
		
    		if (pid3_exist, true)
    		    done();
    		else
    		    done("not have a instance");
    	    });
    	});
	
    	it('should have a second instance', function(done) {
    	    setTimeout(function() {
    		child = exec(cmd + pid4, function (error, stdout, stderr) {
    		    if (stdout == "")
    			pid4_exist = false;
    		    else
    			pid4_exist = true;
		    
    		    if (pid4_exist, true)
    			done();
    		    else
    			done('have a second instance');
    		});
    	    }, 5500);
    	});
    });

    describe('single-instance default', function() {
    	var app_path = path.join(global.tests_dir, 'single_instance', process.platform, 'app');
    	before(function(done) {

	    setTimeout(function() {
    		child1 = exec(app_path);
    		pid1 = child1.pid;
    		console.log(pid1);
		
		// normally we won't launch app one after another immediately
		// Otherwise,sometimes it won't work here
		setTimeout(function(){
    		    child2 = exec(app_path);
    		    pid2 = child2.pid;
		    console.log(pid2);
		    done();
		}, 1000);
	    }, 2000)
    	});
	
    	it('should have a instance', function(done) {
    	    child = exec(cmd + pid1, function (error, stdout, stderr) {
    		if (stdout == "")
    		    pid1_exist = false;
    		else
    		    pid1_exist = true;
		
		if (pid1_exist)
    		    done();
		else
		    done('not have a instance');
    	    });
    	});
	
    	it('should not have a second instance', function(done) {
    	    setTimeout(function() {
    		child = exec(cmd + pid2, function (error, stdout, stderr) {
    		    if (stdout == "")
    			pid2_exist = false;
    		    else
    			pid2_exist = true;
		    
		    if (!pid2_exist)
    			done();
		    else
			done("have a second instance");
    		});
    	    }, 5500);
    	});
    });
    
});	
