var fs = require('fs');
var path = require('path');
var child_process = require('child_process');
var colors = require('colors');
//var toMarkdown = require('to-markdown').toMarkdown;

var g_quiet = false;
var g_outputDir;
var g_outputLog;
var g_format;

function print(str, err) {
  if (!g_quiet) {
    if (err) {
      console.error(str);
    } else {
      console.log(str);
    }
  }
  if (g_outputLog) {
    fs.appendFile(g_outputLog, str + '\n');
  }
}

function preTest() {
  print(colors.green('==============='));
  print(colors.green('Begin testing'));
  print(colors.green('==============='));
}

function postTest() {
  print(colors.green('==============='));
  print(colors.green('End testing'));
  print(colors.green('==============='));

  if (g_format === 'markdown' && fs.existsSync(g_outputDir)) {
    // convert xml to md for better reading
    var subFiles = fs.readdirSync(g_outputDir);
    for (var i = 0, len = subFiles.length; i < len; i++) {
      var filePath = subFiles[i];
      if (path.extname(filePath) === '.xml') {
        var conent = fs.readFileSync(path.join(g_outputDir, filePath));
        content = encodeURIComponent(conent);
        var mdData = toMarkdown(content);
        fs.writeFileSync(path.join(g_outputDir, path.basename(filePath)+'.md'), mdData);
      }
    }
  }
}

function performTests(config) {

  g_quiet = config.quiet;

  var curPath = fs.realpathSync('.');
  if (!fs.existsSync('output'))
    fs.mkdirSync('output');
  g_outputDir = path.join(curPath, 'output', (new Date()).toUTCString());
  if (!fs.existsSync(g_outputDir))
    fs.mkdirSync(g_outputDir);
  g_outputLog = path.join(g_outputDir, 'summary.log');
  print('The test results will output to : ' + colors.underline(g_outputDir) + '\n');
 
  var subDirs = fs.readdirSync(curPath);
  var testSuites = [];
  var isExcluded = function(testsuite) {
    for (var i = 0, len = config.exclude.length; i < len; i++) {
      if (config.exclude[i] === testsuite)
        return true;
    }
    return false;
  };

  if (config.single && config.single !== '' && fs.existsSync(path.join(curPath, config.single))) {
    testSuites.push(config.single);
  } else {
    for (var i = 0, len = subDirs.length; i < len;  i++) {
      var subDir = subDirs[i];
      var status = fs.statSync(path.join(curPath, subDir));
      if (status && status.isDirectory() && !isExcluded(subDir)) {
        testSuites.push(subDir);
      }
    }
  }

  /* ============ running test cases ================ */
  if (testSuites.length === 0) {
    print('No test cases are found in ' + curPath);
    return;
  }
  preTest();
  g_format = config.format;
  var arg_timeout = config.timeout ? ['--timeout', config.timeout] : [];
  var arg_slow = config.slow ? ['--slow', config.slow] : [];
  var argv = undefined;
  var cmd = undefined;
  var child = undefined;
  var env = process.env;
  var timerId = undefined;
  var children = [];

  var timeout = parseInt(config.timeout);
  if (Number.isNaN(timeout) || timeout <= 0)
    timeout = 5000;

  env['out_dir'] = g_outputDir;

  var spawTestProcessByNW = function(testcase) {
    cmd = 'nw';
    argv = [testcase];
    print('Running: ' + colors.underline(testcase));
    child = child_process.spawn(cmd, argv, {env: env});
    child.testcase = testcase;
    return child;
  };

  var spawTestProcessByMocha = function(testcase) {
    cmd = 'mocha';
    argv = [testcase + path.sep + 'mocha_test'].concat(['-R', 'xunit-file'])
      .concat(arg_timeout).concat(arg_slow);
    env['XUNIT_FILE'] = path.join(g_outputDir, testcase + '.xml');
    print('Running:' + colors.underline(testcase));
    child = child_process.spawn(cmd, argv, {env: env});
    child.testcase = testcase;
    return child;
  };

  var curIdx = -1;
  var curChildProcess = undefined;
  var prefix;

//  for (var i =0, len = testSuites.length; i < len; i++ ) {
//    console.log('Test suite ' + i + ':' + testSuites[i]); 
//  }

  var runNextTestByNW = function() {
    curIdx += 1;
    if (curIdx < testSuites.length) {
      resetTimeout(curIdx);
      curChildProcess = spawTestProcessByNW(testSuites[curIdx]);
      children.push(curChildProcess);
      curChildProcess.on('exit', function(code, signal) {
        (function(p, c) {
          prefix = (code === 0 ? colors.green('[Success]') : colors.red('[Failure]'));
          print(prefix + ' : ' + colors.underline(p.testcase) + ' with exit code ' + c);
          clearTimeout(timerId);
          p.removeAllListeners('exit');
          p.kill('SIGKILL');
          runNextTestByNW();
        })(curChildProcess, code);
      });
    } else {
      if (timerId) {
        clearTimeout(timerId);
        timerId = undefined;
      }
      postTest();
    }
  };

  var resetTimeout = function(idx) {
    if (timerId)
      clearTimeout(timerId);
    (function(idx){

    timerId = setTimeout(function() {

      timerId = undefined;
      if (idx || idx >= 0) {
        var p = children[idx];
        print(colors.yellow('[Timeout]') + ' : ' + colors.underline(p.testcase));
        p.removeAllListeners('exit');
        p.kill('SIGKILL');  // 'SIGKILL', 'SIGTERM'
        p = undefined;
        runNextTestByNW();
      }
     }, timeout);

    })(idx);

  };

  runNextTestByNW();

}   // end of performTests()

var content = fs.readFileSync('./config.json');
if (content)
  performTests(JSON.parse(content));
else
  print('Failed to read configuration.', true);


