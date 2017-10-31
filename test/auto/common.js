var path = require('path');
var fs = require('fs');
var assert = require('assert');
var os = require('os');
var child_process = require('child_process');

exports.testDir = path.dirname(__filename);
exports.fixturesDir = path.join(exports.testDir, 'fixtures');
exports.libDir = path.join(exports.testDir, '../lib');
exports.tmpDirName = 'tmp';
exports.PORT = +process.env.NODE_COMMON_PORT || 12346;

if (process.env.TEST_THREAD_ID) {
  // Distribute ports in parallel tests
  if (!process.env.NODE_COMMON_PORT)
    exports.PORT += +process.env.TEST_THREAD_ID * 100;

  exports.tmpDirName += '.' + process.env.TEST_THREAD_ID;
}
exports.tmpDir = path.join(exports.testDir, exports.tmpDirName);

var opensslCli = null;

// opensslCli defined lazily to reduce overhead of spawnSync
Object.defineProperty(exports, 'opensslCli', {get: function() {
  if (opensslCli !== null) return opensslCli;

  if (process.config.variables.node_shared_openssl) {
    // use external command
    opensslCli = 'openssl';
  } else {
    // use command built from sources included in io.js repository
    opensslCli = path.join(path.dirname(process.execPath), 'openssl-cli');
  }

  if (process.platform === 'win32') opensslCli += '.exe';

  var openssl_cmd = child_process.spawnSync(opensslCli, ['version']);
  if (openssl_cmd.status !== 0 || openssl_cmd.error !== undefined) {
    // openssl command cannot be executed
    opensslCli = false;
  }
  return opensslCli;
}, enumerable: true });

Object.defineProperty(exports, 'hasCrypto', {get: function() {
  return process.versions.openssl ? true : false;
}});

if (process.platform === 'win32') {
  exports.PIPE = '\\\\.\\pipe\\libuv-test';
} else {
  exports.PIPE = exports.tmpDir + '/test.sock';
}

if (process.env.NODE_COMMON_PIPE) {
  exports.PIPE = process.env.NODE_COMMON_PIPE;
  // Remove manually, the test runner won't do it
  // for us like it does for files in test/tmp.
  try {
    fs.unlinkSync(exports.PIPE);
  } catch (e) {
    // Ignore.
  }
}

if (process.platform === 'win32') {
  exports.faketimeCli = false;
} else {
  exports.faketimeCli = path.join(__dirname, "..", "tools", "faketime", "src",
    "faketime");
}

var ifaces = os.networkInterfaces();
exports.hasIPv6 = Object.keys(ifaces).some(function(name) {
  return /lo/.test(name) && ifaces[name].some(function(info) {
    return info.family === 'IPv6';
  });
});

var util = require('util');
for (var i in util) exports[i] = util[i];

function protoCtrChain(o) {
  var result = [];
  for (; o; o = o.__proto__) { result.push(o.constructor); }
  return result.join();
}

exports.indirectInstanceOf = function(obj, cls) {
  if (obj instanceof cls) { return true; }
  var clsChain = protoCtrChain(cls.prototype);
  var objChain = protoCtrChain(obj);
  return objChain.slice(-clsChain.length) === clsChain;
};


exports.ddCommand = function(filename, kilobytes) {
  if (process.platform === 'win32') {
    var p = path.resolve(exports.fixturesDir, 'create-file.js');
    return '"' + process.argv[0] + '" "' + p + '" "' +
           filename + '" ' + (kilobytes * 1024);
  } else {
    return 'dd if=/dev/zero of="' + filename + '" bs=1024 count=' + kilobytes;
  }
};


exports.spawnCat = function(options) {
  var spawn = require('child_process').spawn;

  if (process.platform === 'win32') {
    return spawn('more', [], options);
  } else {
    return spawn('cat', [], options);
  }
};


exports.spawnSyncCat = function(options) {
  var spawnSync = require('child_process').spawnSync;

  if (process.platform === 'win32') {
    return spawnSync('more', [], options);
  } else {
    return spawnSync('cat', [], options);
  }
};


exports.spawnPwd = function(options) {
  var spawn = require('child_process').spawn;

  if (process.platform === 'win32') {
    return spawn('cmd.exe', ['/c', 'cd'], options);
  } else {
    return spawn('pwd', [], options);
  }
};

var knownGlobals = [setTimeout,
                    setInterval,
                    setImmediate,
                    clearTimeout,
                    clearInterval,
                    clearImmediate,
                    console,
                    constructor, // Enumerable in V8 3.21.
                    Buffer,
                    process,
                    global];

if (global.gc) {
  knownGlobals.push(gc);
}

if (global.DTRACE_HTTP_SERVER_RESPONSE) {
  knownGlobals.push(DTRACE_HTTP_SERVER_RESPONSE);
  knownGlobals.push(DTRACE_HTTP_SERVER_REQUEST);
  knownGlobals.push(DTRACE_HTTP_CLIENT_RESPONSE);
  knownGlobals.push(DTRACE_HTTP_CLIENT_REQUEST);
  knownGlobals.push(DTRACE_NET_STREAM_END);
  knownGlobals.push(DTRACE_NET_SERVER_CONNECTION);
}

if (global.COUNTER_NET_SERVER_CONNECTION) {
  knownGlobals.push(COUNTER_NET_SERVER_CONNECTION);
  knownGlobals.push(COUNTER_NET_SERVER_CONNECTION_CLOSE);
  knownGlobals.push(COUNTER_HTTP_SERVER_REQUEST);
  knownGlobals.push(COUNTER_HTTP_SERVER_RESPONSE);
  knownGlobals.push(COUNTER_HTTP_CLIENT_REQUEST);
  knownGlobals.push(COUNTER_HTTP_CLIENT_RESPONSE);
}

if (global.LTTNG_HTTP_SERVER_RESPONSE) {
  knownGlobals.push(LTTNG_HTTP_SERVER_RESPONSE);
  knownGlobals.push(LTTNG_HTTP_SERVER_REQUEST);
  knownGlobals.push(LTTNG_HTTP_CLIENT_RESPONSE);
  knownGlobals.push(LTTNG_HTTP_CLIENT_REQUEST);
  knownGlobals.push(LTTNG_NET_STREAM_END);
  knownGlobals.push(LTTNG_NET_SERVER_CONNECTION);
}

if (global.ArrayBuffer) {
  knownGlobals.push(ArrayBuffer);
  knownGlobals.push(Int8Array);
  knownGlobals.push(Uint8Array);
  knownGlobals.push(Uint8ClampedArray);
  knownGlobals.push(Int16Array);
  knownGlobals.push(Uint16Array);
  knownGlobals.push(Int32Array);
  knownGlobals.push(Uint32Array);
  knownGlobals.push(Float32Array);
  knownGlobals.push(Float64Array);
  knownGlobals.push(DataView);
}

// Harmony features.
if (global.Proxy) {
  knownGlobals.push(Proxy);
}

if (global.Symbol) {
  knownGlobals.push(Symbol);
}

function leakedGlobals() {
  var leaked = [];

  for (var val in global)
    if (-1 === knownGlobals.indexOf(global[val]))
      leaked.push(val);

  return leaked;
};
exports.leakedGlobals = leakedGlobals;

// Turn this off if the test should not check for global leaks.
exports.globalCheck = false;

process.on('exit', function() {
  if (!exports.globalCheck) return;
  var leaked = leakedGlobals();
  if (leaked.length > 0) {
    console.error('Unknown globals: %s', leaked);
    assert.ok(false, 'Unknown global found');
  }
});


var mustCallChecks = [];


function runCallChecks(exitCode) {
  if (exitCode !== 0) return;

  var failed = mustCallChecks.filter(function(context) {
    return context.actual !== context.expected;
  });

  failed.forEach(function(context) {
    console.log('Mismatched %s function calls. Expected %d, actual %d.',
                context.name,
                context.expected,
                context.actual);
    console.log(context.stack.split('\n').slice(2).join('\n'));
  });

  if (failed.length) process.exit(1);
}


exports.mustCall = function(fn, expected) {
  if (typeof expected !== 'number') expected = 1;

  var context = {
    expected: expected,
    actual: 0,
    stack: (new Error).stack,
    name: fn.name || '<anonymous>'
  };

  // add the exit listener only once to avoid listener leak warnings
  if (mustCallChecks.length === 0) process.on('exit', runCallChecks);

  mustCallChecks.push(context);

  return function() {
    context.actual++;
    return fn.apply(this, arguments);
  };
};

exports.checkSpawnSyncRet = function(ret) {
  assert.strictEqual(ret.status, 0);
  assert.strictEqual(ret.error, undefined);
};

var etcServicesFileName = path.join('/etc', 'services');
if (process.platform === 'win32') {
  etcServicesFileName = path.join(process.env.SystemRoot, 'System32', 'drivers',
    'etc', 'services');
}

/*
 * Returns a string that represents the service name associated
 * to the service bound to port "port" and using protocol "protocol".
 *
 * If the service is not defined in the services file, it returns
 * the port number as a string.
 *
 * Returns undefined if /etc/services (or its equivalent on non-UNIX
 * platforms) can't be read.
 */
exports.getServiceName = function getServiceName(port, protocol) {
  if (port == null) {
    throw new Error("Missing port number");
  }

  if (typeof protocol !== 'string') {
    throw new Error("Protocol must be a string");
  }

  /*
   * By default, if a service can't be found in /etc/services,
   * its name is considered to be its port number.
   */
  var serviceName = port.toString();

  try {
    /*
     * I'm not a big fan of readFileSync, but reading /etc/services asynchronously
     * here would require implementing a simple line parser, which seems overkill
     * for a simple utility function that is not running concurrently with any
     * other one.
     */
    var servicesContent = fs.readFileSync(etcServicesFileName,
      { encoding: 'utf8'});
    var regexp = util.format('^(\\w+)\\s+\\s%d/%s\\s', port, protocol);
    var re = new RegExp(regexp, 'm');

    var matches = re.exec(servicesContent);
    if (matches && matches.length > 1) {
      serviceName = matches[1];
    }
  } catch(e) {
    console.error('Cannot read file: ', etcServicesFileName);
    return undefined;
  }

  return serviceName;
}

exports.hasMultiLocalhost = function hasMultiLocalhost() {
  var TCP = process.binding('tcp_wrap').TCP;
  var t = new TCP();
  var ret = t.bind('127.0.0.2', exports.PORT);
  t.close();
  return ret === 0;
};

exports.isValidHostname = function(str) {
  // See http://stackoverflow.com/a/3824105
  var re = new RegExp(
    '^([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\\-]{0,61}[a-zA-Z0-9])' +
    '(\\.([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\\-]{0,61}[a-zA-Z0-9]))*$');

  return !!str.match(re) && str.length <= 255;
}

exports.fileExists = function(pathname) {
  try {
    fs.accessSync(pathname);
    return true;
  } catch (err) {
    return false;
  }
};

exports.done = function(n, cb) {
  var doneCounter = 0;
  return function () {
    doneCounter++;
    if (doneCounter >= n)
      cb();
  };
};
