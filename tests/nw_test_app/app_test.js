/**
 * console module for app test
 */

var spawn = require('child_process').spawn;
var net = require('net');
var server = global.server;
var port = global.port;
var program = require('commander');


exports.createClient = function(options) {
  var
      argv = options.argv;
      
  
  //command line arguments.
  program
    .option('--auto', 'run app automatic')
    .option('-p, --port <port>', "set port used by socket")
    .parse([ 'node-webkit', 'nw-test' ].concat(argv));
  
  var client_prot = 13013;
  if (program.port)
    client_prot = program.port;
  
  
  if (options.port) client_prot = options.port;
  
  if (program.auto) {
    var client = net.connect({port: client_prot});
    client.setEncoding('utf8');   

    if (options.delay) {
      setTimeout(function(){        
        client.end(JSON.stringify(options.data));
      }, options.delay);
      
    } else {
      client.end(JSON.stringify(options.data));
    }
    
  }
  return {'auto': program.auto};
}

exports.init = function(arg1, arg2) {
  server = arg1;
  port =  arg2;
}

function childProcess() {
  this.socket = null;
  this.app = null;
  
}

childProcess.prototype.a = function(cb) {
  this.socket.on('data', cb(data));
}

/*
 * options: 
 *   execPath: (string)the path of nw.
 *   appPath: (string)the path of app.
 *   end:  (function)we should do the report here, after get child process's result.    
 *           data: JSON object
 *           app: nodejs childProcess.spawn 
 */
exports.createChildProcess = function(options) {
  
  if (options.port) prot = options.port;
  
  var 
      execPath = options.execPath,
      path = options.appPath,
      exec_argv = [path, '--port', port, '--auto'],
      app, cb,
      child = new childProcess();
        
  
  server.on('connection', cb = function(socket){
    socket.setEncoding('utf8');
    console.log('in app connect');
    child.socket = socket;
    socket.on('data', function(data) {
      options.end(JSON.parse(data), app);
      server.removeListener('connection', cb);
      
    });
    

  });
  
  app = spawn(execPath, exec_argv);
  child.app = app;
  
  return child;
}

