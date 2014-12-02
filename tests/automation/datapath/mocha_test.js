var path = require('path');
var fs = require('fs-extra');
var curDir = fs.realpathSync('.');


describe('data-path', function() {

    var server;

	before(function(done){
		this.timeout(0);
        if (!fs.existsSync(path.join(curDir, 'data-cash')))
	      fs.mkdirsSync(path.join(curDir, 'data-cash'));
        if (!fs.existsSync(path.join(curDir, 'dataPath')))
	      fs.mkdirsSync(path.join(curDir, 'dataPath'));
        if (!fs.existsSync(path.join(curDir, 'dataPath', 'data-cash')))
	      fs.mkdirsSync(path.join(curDir, 'dataPath', 'data-cash'));
        if (!fs.existsSync(path.join(curDir, 'data-cash', 'dataPath')))
	      fs.mkdirsSync(path.join(curDir, 'data-cash', 'dataPath'));

        server = createTCPServer(13013);

	    done();
	  });

	  after(function() {
	    setTimeout(function() {
          server.close();
	      fs.remove(path.join(curDir, 'data-cash'));
	      fs.remove(path.join(curDir, 'dataPath'));
	      fs.remove(path.join(curDir, 'dataPath', 'data-cash'));
	      fs.remove(path.join(curDir, 'data-cash', 'dataPath'));
	    }, 1000);
	  });


  it('setting datapath as ./data-cash/ should pass',
    function(done) {
      this.timeout(0);
      var child = spawnChildProcess(path.join(curDir, 'datacash'));
      server.on('connection', function(socket) {
        socket.setEncoding('utf8');
        socket.on('data', function(data) {
          child.kill();
          server.removeAllListeners('connection');
          done();
      });});
  });

  it('setting datapath as ./dataPath/ should pass',
    function(done) {
      this.timeout(0);

      var child = spawnChildProcess(path.join(curDir, 'datapath'));
      server.on('connection', function(socket) {
        socket.setEncoding('utf8');
        socket.on('data', function(data) {
          child.kill();
          server.removeAllListeners('connection');
          done();
      });});
  });

  it('setting datapath as ./dataPath/data-cash/ should pass',
    function(done) {
      this.timeout(0);
      var child = spawnChildProcess(path.join(curDir, 'datapath-cash'));
      server.on('connection', function(socket) {
        socket.setEncoding('utf8');
        socket.on('data', function(data) {
          child.kill();
          server.removeAllListeners('connection');
          done();
      });});

  });

  it('setting datapath as ./data-cash/dataPath/ should pass',
    function(done) {
      this.timeout(0);

      var child = spawnChildProcess(path.join(curDir, 'datacash-path'));
      server.on('connection', function(socket) {
        socket.setEncoding('utf8');
        socket.on('data', function(data) {
          child.kill();
          server.removeAllListeners('connection');
          done();
      });});

  });


});



