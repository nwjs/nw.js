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

  var resetTimeout = function() {
    if (timerId)
      clearTimeout(timerId);
    timerId = setTimeout(function() {
      timerId = undefined;
      for (var i = 0, len = children.length; i < len; i++) {
        print(colors.red('[Timeout]: ') + colors.underline(children[i].testcase));
        children[i].kill('SIGTERM');
      }
     }, timeout);
  };

  env['out_dir'] = g_outputDir;

  var bindChildExit = function(child, code, signal) {
    child.on('exit', function(code, signal) {
      var prefix = code === 0 ? '[Success]' : '[Failure]';
      print((code === 0 ? colors.green(prefix) : colors.red(prefix)) 
          + ': ' + colors.underline(child.testcase) + ' with exit code ' + code);
      //console.log('Done: ' + child.testcase + ' with exit code ' + code);
      var idx = children.indexOf(child);
      if (idx >= 0) {
        children.splice(idx, 1);
        resetTimeout();
      }
      if (children.length === 0) {
        if (timerId)
          clearTimeout(timerId);
        postTest();
      }
    });
  };

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

  for (var i = 0, len = testSuites.length; i < len; i++) {
    child = spawTestProcessByNW(testSuites[i]);
    if (child) {
      child.testcase = testSuites[i];
      bindChildExit(child);
      //(function(theChild) {
      //  theChild.on('exit', function(code, signal) {
      //    onChildExit(theChild, code, signal);
      //  });
      //})(child);
      children.push(child);
     }
   }
  resetTimeout();
}   // end of performTests()

var content = fs.readFileSync('./config.json');
if (content)
  performTests(JSON.parse(content));
else
  print('Failed to read configuration.', true);


