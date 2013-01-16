/**
 * console module for app test
 */

var spawn = require('child_process').spawn;
var net = require('net');
var server = global.server;
var port = global.port;
var program = require('commander');


exports.createClient = function(gui, callback) {
  //command line arguments.
  program
    .option('--auto', 'run app automatic')
    .option('-p, --port <port>', "set port used by socket")
    .parse([ 'node-webkit', 'nw-test' ].concat(gui.App.argv));
  
  var client_prot = 13013;
  if (program.port)
    client_prot = program.port;
  if (!program.auto)
    gui.Window.get().show();
  
  if (program.auto) {
    var client = net.connect({port: client_prot});
    client.setEncoding('utf8');
    callback(client);     
    
  }
  
}

exports.init = function(arg1, arg2) {
  server = arg1;
  port =  arg2;
}

exports.createChildProcess = function(path, callback) {
  var exec_argv = [path, '--port', port, '--auto'];
  var app;
  server.on('connection', function(socket){
    socket.setEncoding('utf8');    
    callback(socket, app);
    

  });
  
  app = spawn(process.execPath, exec_argv);
  
  return app;
}

