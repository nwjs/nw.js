var div = document.createElement('div');
div.id = 'mocha';
//div.style.visibility = 'hidden';
document.body.appendChild(div);

// useful when need to keep app stay on window:
// $ quiet=false nw <app>
//
var g_quiet = process.env['quiet'] || true;
if (g_quiet === 'false') g_quiet = false;

if (!g_quiet) {
  var link = document.createElement('link');
  link.setAttribute('rel', 'stylesheet');
  link.setAttribute('type', 'text/css');
  link.setAttribute('href', '../res/mocha.css');
  document.getElementsByTagName('head')[0].appendChild(link);
}

var fs = require('fs');
var path = require('path');
var net = require('net');
var spawn = require('child_process').spawn;

var createTCPServer = function(port, autoClose) {
  var server = net.createServer();
  if (autoClose) {
    process.exit = function() {
      server.close();
      process.quit();
    };
  }
  port = port || 13013;
  server.on('error', function(e) {
    console.error('Failed to launch TCP server: '+ e.code);
  });
  server.listen(port);
  return server;
};

var connectToTCPServer = function(port, data, keepOpen) {
  port = port || 13013;
  var socket = net.connect({port: port});
  socket.setEncoding('utf8');
  if (data) {
    socket.write(JSON.stringify(data));
  }
  if (!keepOpen) {
    socket.end();
    socket = undefined;
  }
  return socket;
};

var spawnChildProcess = function(appPath) {
  var child = spawn(process.execPath, [appPath]);
  return child;
};

mocha.setup({ui:'bdd'});

var outputAsXML = function() {
  var outputDir = process.env['out_dir'];
  if (!outputDir) {
    outputDir = fs.realpathSync('.');
    console.log('====Test results will be at: ' + outputDir);
    //return;
  }
  var ret = 0;
  if (window.XMLSerializer) {
    var xmlParser = new XMLSerializer();
    var appName = process.cwd();
    appName = appName.substring(appName.lastIndexOf(path.sep) + 1);
    var outputPath = path.join(outputDir, appName + '.xml');
    var reportNode = document.getElementById('mocha-report');
    var mochaNode = document.getElementById('mocha');
    var data = xmlParser.serializeToString(mochaNode); //(reportNode);
    //data = data.replace('/', '//');
    //data = data.replace('%20', ' ');
    data = decodeURIComponent(data);
    fs.writeFileSync(outputPath, data);
  }

  // check failures
  var stats = document.getElementById('mocha-stats');
  var failureNodes = stats.getElementsByClassName('failures');
  var failureNumNode = undefined;
  var failures = '';

  if (failureNodes && failureNodes.length > 0) {
    failureNumNode = failureNodes[0].getElementsByTagName('em');
    if (failureNumNode && failureNumNode.length > 0) {
      failures = failureNumNode[0].innerText;
      if (failures !== '0') {
        ret = 1;
      }
    }
  }

  if (g_quiet)
    process.exit(ret);

  return 0;
};

window.assertRunningResult = function(result, errMsg) {

  describe('Assert test running result', function() {
    it('the test should succeed', function(done){
      if (result) {
        done();
      } else {
        done('Assertion failed: ' + errMsg);
      }
    });
  });
};

window.addEventListener('load', function(){
  mocha.run();
  mocha.suite.afterAll(function(){
    outputAsXML();
  });
});
