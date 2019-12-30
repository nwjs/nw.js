exports.message = "hello world";
exports.port = 10000;
exports.ready = false;

var server = require('net').createServer();
server.on('connection',function(socket){
	socket.on('data',function(data){
		socket.write(data);
	});
});

server.on('error',function(){
	try{
		server.close();
	}catch(e){
		exports.port += 1;
		setTimeout(function(){
			server.listen(exports.port);
		},0);
	}
});
server.listen(exports.port,function(){
	exports.ready = true;
});
