/**
 * console module for app test
 */

var spawn = require('child_process').spawn;
var net = require('net');
var server = global.server;
var port = global.port;
var program = require('commander');


/*
 * options:
 *   argv: the argv of nw
 *   data: (string|json) data to be sended to control
 *         
 *
 */
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
   
    if (options.data != null) {
    if (options.delay) {
      setTimeout(function(){        
        client.end(JSON.stringify(options.data));
      }, options.delay);
      
    } else {
      client.end(JSON.stringify(options.data));
    }
    
    }
    
  }
  return {'auto': program.auto, 'socket': client};
}

exports.init = function(arg1, arg2) {
  server = arg1;
  port =  arg2;
}

function childProcess() {
  this.socket = null;
  this.app = null;
  this.server_connection_cb = null;
}

childProcess.prototype.removeConnection = function() {
  try {
    server.removeListener('connection', this.server_connection_cb); 
  } catch (e) {
  }
}

childProcess.prototype.close = function() {
  try {
    this.app.kill();
    server.removeListener('connection', this.server_connection_cb); 
  } catch (e) {
    console.log('error');
  }
}

/*
 * options: 
 *   execPath: (string)the path of nw.
 *   appPath: (string)the path of app.
 *   args: (array)the args of the app.
 *   end:  (function)we should do the report here, after get child process's result.    
 *           data: JSON object
 *           app: nodejs childProcess.spawn 
 *   no_connect: (bool) whether listen the connection event
 *
 * notices:
 *   now we can only receive data one time. 
 *   if your client exists error then crash, and casue the data can not been
 *   sended, please call `removeConnection()`. otherwise there would be problem. 
 *
 */
exports.createChildProcess = function(options) {
  
  if (options.port) prot = options.port;
  
  var 
      execPath = options.execPath,
      path = options.appPath,
      exec_argv, 
      app, cb,
      no_connect = options.no_connect || false,
      child = new childProcess();
  
    if (!options.args)
	exec_argv = [path, '--port', port, '--auto'];
    else
	exec_argv = [path].concat(options.args).concat(['--port', port, '--auto']);

  if (!no_connect) {
  
    server.on('connection', cb = function(socket){
      socket.setEncoding('utf8');
      console.log('in app connect');
      child.socket = socket;
      
      socket.on('data', function(data) {
        options.end(JSON.parse(data), app);              
      });
      socket.on('end', function() {
        //console.log('in app end');
        server.removeListener('connection', cb); 
      });
      socket.on('error', function(e) {
        //console.log(e);
      });
      child.server_connection_cb = cb;
    }); //server.on()   
  } //if (no_conect)
  
  app = spawn(execPath, exec_argv);
  child.app = app;
  
  return child;
}

