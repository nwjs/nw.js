var assert = require('assert');
var func = require('./' + global.tests_dir + '/start_app/script.js');
var result = [];
var spawn = require('child_process').spawn;
var fs = require('fs-extra');
var is_chromedriver_exists = false;
var path = require('path');

describe("buff_from_string", function() {
	var results = [];
	var server = global.server;
	var getResult = function(socket){
		socket.on('data',function(data){
			results = JSON.parse(data.toString());
		});
	};
	before(function(done) {
		this.timeout(0);
		server.on('connection',getResult);
		func.copyExecFiles(function() {
			var src_files = [
				"automatic_tests/buff_from_string/index.html",
				"automatic_tests/buff_from_string/package.json",
				"automatic_tests/buff_from_string/test.py"
			];
			var dst_files = [
				"tmp-nw/index.html",
				"tmp-nw/package.json",
				"tmp-nw/test.py"
			];

			if (process.platform == 'win32'){
				src_files.push("automatic_tests/buff_from_string/chromedriver2_server.exe");
				dst_files.push("tmp-nw/chromedriver2_server.exe");
			} else {
				src_files.push("automatic_tests/buff_from_string/chromedriver2_server");
				dst_files.push("tmp-nw/chromedriver2_server");
			}

			for (var i=0;i<src_files.length;++i){
				fs.copySync(src_files[i],dst_files[i]);
			}
			if (process.platform != 'win32'){
				fs.chmodSync("tmp-nw/chromedriver2_server",365);
			}
			var test_file = path.join("tmp-nw","test.py");
			setTimeout(function(){
				var child = spawn('python',[test_file]);
				child.on('exit',function(code,signal){
					console.log(code,signal);
					done();
				});
				child.on('error',function(err){
					console.log(err);
				});
				child.stderr.on('data',function(data){
					console.log(data.toString());
				});
				child.stdout.on('data',function(data){
					console.log(data.toString());
				});
			},1000);
		});
	});

	it("string and buff.toString() should equal", function(done) {
		assert.equal(results.length,1);
		assert.equal(results[0],true);
		done();
	});

	after(function(done) {
		this.timeout(0);
		server.removeListener('connection',getResult);
		setTimeout(function(){
			fs.removeSync("tmp-nw");	
			done();
		},2000);
	});
});