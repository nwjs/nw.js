var path = require('path');
var assert = require('assert');
var fs = require('fs');
var curDir = fs.realpathSync('.');

var results;
var expected = new Array(
    'New York',
    '<span class="console-formatted-number source-code">18</span>',
    '<span class="console-formatted-boolean source-code">true</span>', 
    '<span class="console-formatted-boolean source-code">false</span>', 
    '<span class="console-formatted-array source-code"><div class="section"><div class="header monospace"><div class="subtitle"></div><div class="title"><span>Array[3]</span><span class="object-info-state-note" title="Object state below is captured upon first expansion"></span></div></div><ol class="properties properties-tree monospace"></ol></div></span>',
    '<span class="console-formatted-object source-code"><div class="section"><div class="header monospace"><div class="subtitle"></div><div class="title"><span>Object</span><span class="object-info-state-note" title="Object state below is captured upon first expansion"></span></div></div><ol class="properties properties-tree monospace"></ol></div></span>',
    '<span class="console-formatted-undefined source-code">undefined</span>',
    '<span class="console-formatted-null source-code">null</span>',
    'Node count: <span class="console-formatted-number source-code">2</span> Done',
    'Sam',
    '100',
    '2',
    '12.23',
    '<span class="console-formatted-node source-code"><ol class="elements-tree-outline outline-disclosure single-node" tabindex="0"><li title="" draggable="true"><span class="highlight"><span class="webkit-html-doctype">&lt;!DOCTYPE html&gt;</span></span></li></ol></span>',
    '<span class="console-formatted-node source-code"><ol class="elements-tree-outline outline-disclosure single-node" tabindex="0"><li title="" draggable="true"><span class="highlight"><span class="webkit-html-doctype">&lt;!DOCTYPE html&gt;</span></span></li></ol></span>',
    '<span class="console-formatted-object source-code"><div class="section"><div class="header monospace"><div class="subtitle"></div><div class="title"><span>&lt;!DOCTYPE html&gt;</span><span class="object-info-state-note" title="Object state below is captured upon first expansion"></span></div></div><ol class="properties properties-tree monospace"></ol></div></span>',
    '<span style="color: orange; background-color: blue; font-size: 16pt; background-position: initial initial; background-repeat: initial initial;"> Sam has 100 points and 2 pencils.He carrys 12.23 kg of water</span>', 
    '<span style="color: orange; background-color: blue; font-size: 16pt; background-position: initial initial; background-repeat: initial initial;"></span><span style="color: orange; background-color: blue; font-size: 16pt; background-position: initial initial; background-repeat: initial initial;">Sam</span><span style="color: orange; background-color: blue; font-size: 16pt; background-position: initial initial; background-repeat: initial initial;"> has </span><span style="color: orange; background-color: blue; font-size: 16pt; background-position: initial initial; background-repeat: initial initial;">100</span><span style="color: orange; background-color: blue; font-size: 16pt; background-position: initial initial; background-repeat: initial initial;"> points and </span><span style="color: orange; background-color: blue; font-size: 16pt; background-position: initial initial; background-repeat: initial initial;">2</span><span style="color: orange; background-color: blue; font-size: 16pt; background-position: initial initial; background-repeat: initial initial;"> pencils.He carrys </span><span style="color: orange; background-color: blue; font-size: 16pt; background-position: initial initial; background-repeat: initial initial;">12.23</span><span style="color: orange; background-color: blue; font-size: 16pt; background-position: initial initial; background-repeat: initial initial;"> kg of water. </span><span class="console-formatted-node source-code"><ol class="elements-tree-outline outline-disclosure single-node" tabindex="0"><li title="" draggable="true"><span class="highlight"><span class="webkit-html-doctype">&lt;!DOCTYPE html&gt;</span></span></li></ol></span><span style="color: orange; background-color: blue; font-size: 16pt; background-position: initial initial; background-repeat: initial initial;"> </span><span class="console-formatted-object source-code"><div class="section"><div class="header monospace"><div class="subtitle"></div><div class="title"><span>&lt;!DOCTYPE html&gt;</span><span class="object-info-state-note" title="Object state below is captured upon first expansion"></span></div></div><ol class="properties properties-tree monospace"></ol></div></span><span style="color: orange; background-color: blue; font-size: 16pt; background-position: initial initial; background-repeat: initial initial;"></span>');


describe('console.log', function() {

  var child, server;

  before(function(done) {
    this.timeout(0);

    child = spawnChildProcess(path.join(curDir, 'internal'));

    server = createTCPServer(13013);
    server.on('connection', function(socket) {
      socket.setEncoding('utf8');
      socket.on('data', function(data) {
        results = JSON.parse(data);
        done();
      });
    });

    setTimeout(function() {
      done('timeout');
    }, 4500);

  });

  after(function(done){
    this.timeout(0);
    child.kill();
    server.close();
    done();
  });

    describe('output data types', function() {
	it('should display string correctly', function() {
	    assert.equal(results[0], expected[0]);
	});
	
	it('should display number correctly', function() {
	    assert.equal(results[1], expected[1]);
	});
	
	it('should display boolean correctly', function() {
	    assert.equal(results[2], expected[2]);
	    assert.equal(results[3], expected[3]);
	});
	
	it('should display array correctly', function() {
	    assert.equal(results[4], expected[4]);
	});
	
	it('should display object correctly', function() {
	    assert.equal(results[5], expected[5]);
	});	
	
	it('should display undefined correctly', function() {
	    assert.equal(results[6], expected[6]);
	});

	it('should display null correctly', function() {
	    assert.equal(results[7], expected[7]);
	});
    });


    describe('concatenate expressions', function() {
	it('should display string correctly', function() {
	    assert.equal(results[8], expected[8]);
	});
    });

    describe('format output', function() {
	it('should display correctly with %s', function() {
	    assert.equal(results[9], expected[9]);
	});
	
	it('should display correctly with %d', function() {
	    assert.equal(results[10], expected[10]);
	});
	
	it('should display correcty with %i', function() {
	    assert.equal(results[11], expected[11]);
	});

	it('should display correcty with %f', function() {
	    assert.equal(results[12], expected[12]);
	});

	it('should display correcty object', function() {
	    assert.equal(results[13], expected[13]);
	});

	it('should display correcty with %o', function() {
	    assert.equal(results[14], expected[14]);
	});

	it('should display correcty with %O', function() {
	    assert.equal(results[15], expected[15]);
	});

	it('should display correcty with %c', function() {
	    assert.equal(results[16], expected[16]);
	});

	it('should display correcty with all formats', function() {
	    assert.equal(results[17], expected[17]);
	});
    })
});


