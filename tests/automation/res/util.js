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

window.unitTestApplicationFailed = function() {
  describe('Manually verify the application', function() {
    it('Application should succeed', function(done){
      done('Application failed: ' + appName);
    });
  });
};

window.unitTestApplicationSucceed = function() {
  describe('Manually verify the application', function() {
    it('Application should succeed', function(done){
      done();
    });
  });
};

window.unitTestApplicationTimeout = function() {
  describe('Manually verify the application', function() {
    it('Application should not timeout', function(done){
      done('Application timeout');
    });
  });
};

window.addEventListener('load', function(){
  mocha.run();
  mocha.suite.afterAll(function(){
    outputAsXML();
  });
});
